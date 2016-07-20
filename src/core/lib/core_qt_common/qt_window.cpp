#include "qt_window.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_view.hpp"
#include "i_qt_framework.hpp"
#include "qt_context_menu.hpp"
#include "qt_dock_region.hpp"
#include "qt_menu_bar.hpp"
#include "qt_status_bar.hpp"
#include "qt_tab_region.hpp"
#include "qt_tool_bar.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/binary_block.hpp"
#include <QApplication>
#include <QDockWidget>
#include <QElapsedTimer>
#include <QEvent>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QUiLoader>
#include <QWindow>
#include <cassert>
#include <chrono>
#include <thread>
#include "qt_global_settings.hpp"

namespace wgt
{
namespace
{
	template< typename T >
	std::vector< T * > getChildren( const QObject & parent )
	{
		std::vector< T * > children;
		foreach ( auto child, parent.children() )
		{
			T * childT = qobject_cast< T * >( child );
			if (childT != nullptr)
			{
				children.push_back( childT );
			}
			auto grandChildren = getChildren< T >( *child );
			children.insert( 
				children.end(), grandChildren.begin(), grandChildren.end() );
		}
		return children;
	}

	const char * g_internalPreferenceId = "E28A7FA9-08D4-464F-B073-47CB9DD20F62";
}

struct QtWindow::Impl
	: Depends< IQtFramework >
{
	QtWindow& window_;
	std::unique_ptr< QMainWindow > mainWindow_;

	std::string id_;
	Menus menus_;
	Regions regions_;
	std::unique_ptr<IStatusBar> statusBar_;
	Qt::WindowModality modalityFlag_;
	IUIApplication * application_;
	bool isMaximizedInPreference_;
	bool firstTimeShow_;
	bool loadingPreferences_;
	QtConnectionHolder qtConnections_;

	Impl( IComponentContext & context, QIODevice & source, QtWindow & window )
		: Depends( context )
		, window_( window )
		, isMaximizedInPreference_(false)
		, firstTimeShow_(true)
		, loadingPreferences_( false )
	{
		QUiLoader loader;

		// Load optional plugins that may have custom widgets
		auto qtFramework = get<IQtFramework>();
		assert( qtFramework != nullptr );
		auto& pluginPath = qtFramework->getPluginPath();
		if( !pluginPath.empty() )
		{
			loader.addPluginPath( pluginPath.c_str() );
		}
		auto qMainWindow = qobject_cast< QMainWindow * >( loader.load( &source ) );
		if (qMainWindow == nullptr)
		{
			return;
		}
		mainWindow_.reset( qMainWindow );
		//init();
	}
	Impl( IComponentContext & context, std::unique_ptr<QMainWindow> && mainWindow, QtWindow & window )
		: Depends( context )
		, window_( window )
		, isMaximizedInPreference_(false)
		, firstTimeShow_(true)
		, loadingPreferences_( false )
	{
		if (mainWindow_== nullptr)
		{
			return;
		}
		//init();
	}

	~Impl()
	{
		if (mainWindow_ != nullptr)
		{
			this->savePreference();
			mainWindow_ = nullptr;
		}
	}

	void init()
	{
		auto qtFramework = get<IQtFramework>();
		assert( qtFramework != nullptr );
		auto idProperty = mainWindow_->property("id");
		if (idProperty.isValid())
		{
			id_ = idProperty.toString().toUtf8().operator const char *();
		}

		auto windowMaximizedProperty = mainWindow_->property("windowMaximized");
		if (windowMaximizedProperty.isValid())
		{
			isMaximizedInPreference_ = windowMaximizedProperty.toBool();
		}

		auto menuBars = getChildren< QMenuBar >(*mainWindow_);
		for (auto & menuBar : menuBars)
		{
			if (menuBar->property("path").isValid())
			{
				menus_.emplace_back(new QtMenuBar(*menuBar, id_.c_str()));
			}
		}

		auto toolBars = getChildren< QToolBar >(*mainWindow_);
		for (auto & toolBar : toolBars)
		{
			if (toolBar->property("path").isValid())
			{
				menus_.emplace_back(new QtToolBar(*toolBar, id_.c_str()));
			}
		}

		auto dockWidgets = getChildren< QDockWidget >(*mainWindow_);
		for (auto & dockWidget : dockWidgets)
		{
			if (dockWidget->property("layoutTags").isValid())
			{
				regions_.emplace_back(new QtDockRegion(*this, window_, *dockWidget));
			}
		}

		auto tabWidgets = getChildren< QTabWidget >(*mainWindow_);
		for (auto & tabWidget : tabWidgets)
		{
			if (tabWidget->property("layoutTags").isValid())
			{
				regions_.emplace_back(new QtTabRegion(*this, *tabWidget));
			}
		}

		auto statusBar = getChildren<QStatusBar>( *mainWindow_ );
		if( statusBar.size() > 0 )
		{
			statusBar_.reset(new QtStatusBar(*statusBar.at(0)));
		}
		modalityFlag_ = mainWindow_->windowModality();
		mainWindow_->installEventFilter(&window_);
		loadPreference();
		window_.onPaletteChanged();
		auto globalSettings = qtFramework->qtGlobalSettings();
		qtConnections_ += QObject::connect( globalSettings, &QtGlobalSettings::prePreferencesChanged,
			&window_, &QtWindow::onPrePreferencesChanged );
		qtConnections_ += QObject::connect( globalSettings, &QtGlobalSettings::postPreferencesChanged,
			&window_, &QtWindow::onPostPreferencesChanged );
		qtConnections_ += QObject::connect( globalSettings, &QtGlobalSettings::prePreferencesSaved,
			&window_, &QtWindow::onPrePreferencesSaved );
		qtConnections_ += QObject::connect( qApp, &QApplication::paletteChanged,
			&window_, &QtWindow::onPaletteChanged );
	}
	void savePreference()
	{
		if(!window_.isReady())
		{
			return;
		}
		auto qtFramework = get<IQtFramework>();
		assert( qtFramework != nullptr );
		auto preferences = qtFramework->getPreferences();
		if (preferences == nullptr)
		{
			return;
		}
		std::string key = (id_ == "") ? g_internalPreferenceId : id_;
		auto & preference = preferences->getPreference( key.c_str() );
		QByteArray geometryData = mainWindow_->saveGeometry();
		QByteArray layoutData = mainWindow_->saveState();
		std::shared_ptr< BinaryBlock > geometry = 
			std::make_shared< BinaryBlock >(geometryData.constData(), geometryData.size(), false );
		std::shared_ptr< BinaryBlock > state = 
			std::make_shared< BinaryBlock >(layoutData.constData(), layoutData.size(), false );
		preference->set( "geometry", geometry );
		preference->set( "layoutState", state );
		bool isMaximized = mainWindow_->isMaximized();
		preference->set( "maximized", isMaximized );
		if (!isMaximized)
		{
			auto pos = mainWindow_->pos();
			auto size = mainWindow_->size();
			preference->set( "pos",Vector2( pos.x(), pos.y() ) );
			preference->set( "size",Vector2( size.width(), size.height() ) );
		}
	}

	bool loadPreference()
	{
        auto logDebugMessage = [](const std::string& message)
        {
            NGT_DEBUG_MSG( ( "\nQt Window Preferences Failed: " + message + "\n" ).c_str() );
        };

		auto qtFramework = get<IQtFramework>();
		assert( qtFramework != nullptr );
		auto preferences = qtFramework->getPreferences();
		if (preferences == nullptr)
		{
			logDebugMessage( "Could not get preferences" );
            return false;
		}

		std::string key = (id_ == "") ? g_internalPreferenceId : id_;
        if( !preferences->preferenceExists( key.c_str() ))
        {
            return false; // No preference has been loaded for this key
        }
        auto & preference = preferences->getPreference( key.c_str() );

        //check the preference data first
        BinaryBlock geometry;
        BinaryBlock state;
        bool isMaximized = false;
        Vector2 pos;
        Vector2 size;
        bool isOk = false;

		auto accessor = preference->findProperty( "geometry" );
		if (!accessor.isValid())
		{
			logDebugMessage( key + " No geometry property" );
            return false;
		}

		isOk = preference->get( "geometry", geometry );
		if (!isOk)
		{
			logDebugMessage( key + " Geometry property failed" );
            return false;
		}

		accessor = preference->findProperty( "layoutState" );
		if (!accessor.isValid())
		{
            logDebugMessage( key + " No layoutState property" );
			return false;
		}

		isOk = preference->get( "layoutState", state );
		if (!isOk)
		{
            logDebugMessage( key + " LayoutState property failed" );
			return false;
		}

		accessor = preference->findProperty( "maximized" );
		if (!accessor.isValid())
		{
            logDebugMessage( key + " No maximized property" );
			return false;
		}

		isOk = preference->get( "maximized", isMaximized );
		if (!isOk)
		{
            logDebugMessage( key + " Maximized property failed" );
			return false;
		}

		if (!isMaximized)
		{
			accessor = preference->findProperty( "pos" );
			if (!accessor.isValid())
			{
                logDebugMessage( key + " No pos property" );
				return false;
			}

			isOk = preference->get( "pos", pos );
			if (!isOk)
			{
                logDebugMessage( key + " Pos property failed" );
				return false;
			}

			accessor = preference->findProperty( "size" );
			if (!accessor.isValid())
			{
                logDebugMessage( key + " No size property" );
				return false;
			}

			isOk = preference->get( "size", size );
			if (!isOk)
			{
                logDebugMessage( key + " Size property failed" );
				return false;
			}
		}

		// restore preferences
		isMaximizedInPreference_ = isMaximized;
		isOk = mainWindow_->restoreGeometry( QByteArray( geometry.cdata(), static_cast<int>(geometry.length()) ) );
		if (!isOk)
		{
            logDebugMessage( key + " Could not restore geometry" );
			return false;
		}

		isOk = mainWindow_->restoreState( QByteArray( state.cdata(), static_cast<int>(state.length()) ) );
		if (!isOk)
		{
            logDebugMessage( key + " Could not restore state" );
			return false;
		}

		if (!isMaximized)
		{
			mainWindow_->move( QPoint( static_cast<int>( pos.x ), static_cast<int>( pos.y ) ) );
			mainWindow_->resize( QSize( static_cast<int>( size.x ), static_cast<int>( size.y ) ) );
		}

		return true;
	}

	void show( bool wait )
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		mainWindow_->setWindowModality( modalityFlag_ );
		if(firstTimeShow_ && isMaximizedInPreference_)
		{
			mainWindow_->setWindowState( Qt::WindowMaximized );
		}
		mainWindow_->show();

		if (firstTimeShow_)
		{
			emit window_.windowReady();
		}
		firstTimeShow_ = false;
		if (wait)
		{
			waitForWindowExposed();
		}
	}

	void showMaximized( bool wait )
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		mainWindow_->setWindowModality( modalityFlag_ );

		mainWindow_->showMaximized();

		if(firstTimeShow_)
		{
			emit window_.windowReady();
		}
		firstTimeShow_ = false;
		if (wait)
		{
			waitForWindowExposed();
		}
	}

	void showModal()
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		mainWindow_->setWindowModality( Qt::ApplicationModal );
		if(firstTimeShow_ && isMaximizedInPreference_)
		{
			mainWindow_->setWindowState( Qt::WindowMaximized );
		}
		mainWindow_->show();
		if (firstTimeShow_)
		{
			emit window_.windowReady();
		}
		firstTimeShow_ = false;
	}

	void hide()
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}

		mainWindow_->hide();
	}

	void waitForWindowExposed()
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		enum { TimeOutMs = 10 };
		QElapsedTimer timer;
		const int timeout = 1000;
		if (!mainWindow_->windowHandle())
		{
			mainWindow_->createWinId();
		}
		auto window = mainWindow_->windowHandle();
		timer.start();
		while (!window->isExposed()) 
		{
			const int remaining = timeout - int(timer.elapsed());
			if (remaining <= 0)
			{
				break;
			}
			QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
			QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
			std::this_thread::sleep_for( std::chrono::milliseconds( uint32_t( TimeOutMs ) ) );
		}
	}
};

QtWindow::QtWindow( IComponentContext & context, QIODevice & source )
    : impl_( new Impl( context, source, *this ) )
{
	impl_->init();
}

QtWindow::QtWindow(IComponentContext & context, std::unique_ptr<QMainWindow> && mainWindow)
    : impl_( new Impl( context, std::forward< std::unique_ptr<QMainWindow> && >(mainWindow), *this ) )
{
	impl_->init();
}

QtWindow::~QtWindow()
{
}

const char * QtWindow::id() const
{
	return impl_->id_.c_str();
}

const char * QtWindow::title() const
{
	if (impl_->mainWindow_.get() == nullptr)
	{
		return "";
	}

	return impl_->mainWindow_->windowTitle().toUtf8().constData();
}

void QtWindow::update()
{
	for (auto & menu : impl_->menus_)
	{
		menu->update();
	}
}

void QtWindow::close()
{
	if (impl_->mainWindow_.get() == nullptr)
	{
		return;
	}

	impl_->mainWindow_->close();
}

bool QtWindow::isReady() const
{
	if (!impl_->firstTimeShow_)
	{
		return true;
	}
	return false;
}

void QtWindow::setIcon(const char* path)
{
	if(!path || !impl_->mainWindow_)
		return;

	impl_->mainWindow_->setWindowIcon(QIcon(path));
}

void QtWindow::show( bool wait /* = false */)
{
	impl_->show( wait );
}

void QtWindow::showMaximized( bool wait /* = false */)
{
	impl_->showMaximized( wait );
}

void QtWindow::showModal()
{
	impl_->showModal();
}

void QtWindow::hide()
{
	impl_->hide();
}

const Menus & QtWindow::menus() const
{
	return impl_->menus_;
}

const Regions & QtWindow::regions() const
{
	return impl_->regions_;
}

IStatusBar* QtWindow::statusBar() const
{
	return impl_->statusBar_.get();
}

QMainWindow * QtWindow::window() const
{
	return impl_->mainWindow_.get();
}

QMainWindow * QtWindow::releaseWindow()
{
    return impl_->mainWindow_.release();
}

void QtWindow::onPaletteChanged()
{
	impl_->mainWindow_->setStyleSheet(
		"QMainWindow::separator:vertical{background: palette(dark); width: 4px; border-right: 1px solid palette(midlight);}"
		"QMainWindow::separator:vertical{background: palette(dark); width: 4px; border-left: 1px solid palette(midlight);}"
		"QMainWindow::separator:horizontal{background: palette(dark); height: 4px; border-bottom: 1px solid palette(midlight);}"
		"QMainWindow::separator:horizontal{background: palette(dark); height: 4px; border-top: 1px solid palette(midlight);}");
}

void QtWindow::init()
{
	impl_->init();
}

bool QtWindow::eventFilter(QObject * obj, QEvent * event)
{
	if (obj == impl_->mainWindow_.get())
	{
		if (event->type() == QEvent::Close)
		{
			bool shouldClose = true;

			this->signalTryClose( shouldClose );

			if (shouldClose)
			{
				this->signalClose();
			}
			else
			{
				event->ignore();
			}

			return true;
		}
	}
	return QObject::eventFilter( obj, event );
}

void QtWindow::onPrePreferencesChanged()
{
    impl_->savePreference();
}

void QtWindow::onPostPreferencesChanged()
{
    impl_->loadingPreferences_ = true;
    impl_->loadPreference();
    for(auto& region : impl_->regions_)
    {
        QtDockRegion* iRegion = dynamic_cast<QtDockRegion*>(region.get());
        if(iRegion != nullptr)
        {
            iRegion->restoreDockWidgets();
        }
    }
    impl_->loadingPreferences_ = false;
}

void QtWindow::onPrePreferencesSaved()
{
   impl_->savePreference();
}

bool QtWindow::isLoadingPreferences() const
{
    return impl_->loadingPreferences_;
}
} // end namespace wgt
