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

QtWindow::QtWindow( IQtFramework & qtFramework, QIODevice & source )
    : qtFramework_( qtFramework )
    , application_(nullptr)
    , isMaximizedInPreference_(false)
    , firstTimeShow_(true)
    , loadingPreferences_( false )
{
	QUiLoader loader;

	// Load optional plugins that may have custom widgets
	auto& pluginPath = qtFramework.getPluginPath();
	if( !pluginPath.empty() )
		loader.addPluginPath( pluginPath.c_str() );
	
	auto qMainWindow = qobject_cast< QMainWindow * >( loader.load( &source ) );

	if (qMainWindow == nullptr)
	{
		return;
	}

	mainWindow_.reset( qMainWindow );

    init();
}

QtWindow::QtWindow(IQtFramework & qtFramework, std::unique_ptr<QMainWindow> && mainWindow)
    : qtFramework_(qtFramework)
    , mainWindow_(std::move(mainWindow))
    , application_(nullptr)
    , isMaximizedInPreference_(false)
    , firstTimeShow_(true)
    , loadingPreferences_( false )
{
    if (mainWindow_== nullptr)
    {
        return;
    }
    
    init();
}

QtWindow::~QtWindow()
{
	if (mainWindow_ != nullptr)
	{
		this->savePreference();
		mainWindow_ = nullptr;
	}
}

const char * QtWindow::id() const
{
	return id_.c_str();
}

const char * QtWindow::title() const
{
	if (mainWindow_.get() == nullptr)
	{
		return "";
	}

	return mainWindow_->windowTitle().toUtf8().constData();
}

void QtWindow::update()
{
	for (auto & menu : menus_)
	{
		menu->update();
	}
}

void QtWindow::close()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}

	mainWindow_->close();
}

bool QtWindow::isReady() const
{
	if (!firstTimeShow_)
	{
		return true;
	}
	return false;
}

void QtWindow::setIcon(const char* path)
{
	if(!path || !mainWindow_)
		return;

	mainWindow_->setWindowIcon(QIcon(path));
}

void QtWindow::show( bool wait /* = false */)
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
		emit windowReady();
	}
	firstTimeShow_ = false;
	if (wait)
	{
		waitForWindowExposed();
	}
	
}

void QtWindow::showMaximized( bool wait /* = false */)
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}
	mainWindow_->setWindowModality( modalityFlag_ );
	
	mainWindow_->showMaximized();

	if(firstTimeShow_)
	{
		emit windowReady();
	}
	firstTimeShow_ = false;
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QtWindow::showModal()
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
		emit windowReady();
	}
	firstTimeShow_ = false;
}

void QtWindow::hide()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}

	mainWindow_->hide();
}

const Menus & QtWindow::menus() const
{
	return menus_;
}

const Regions & QtWindow::regions() const
{
	return regions_;
}

void QtWindow::setApplication( IUIApplication * application )
{
	application_ = application;
}

IUIApplication * QtWindow::getApplication() const
{
	return application_;
}

IStatusBar* QtWindow::statusBar() const
{
	return statusBar_.get();
}

QMainWindow * QtWindow::window() const
{
	return mainWindow_.get();
}

QMainWindow * QtWindow::releaseWindow()
{
    return mainWindow_.release();
}

void QtWindow::waitForWindowExposed()
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

void QtWindow::init()
{
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
            regions_.emplace_back(new QtDockRegion(qtFramework_, *this, *dockWidget));
        }
    }

    auto tabWidgets = getChildren< QTabWidget >(*mainWindow_);
    for (auto & tabWidget : tabWidgets)
    {
        if (tabWidget->property("layoutTags").isValid())
        {
            regions_.emplace_back(new QtTabRegion(qtFramework_, *tabWidget));
        }
    }

    auto statusBar = getChildren<QStatusBar>( *mainWindow_ );
    if( statusBar.size() > 0 )
    {
      statusBar_.reset(new QtStatusBar(*statusBar.at(0)));
    }
    modalityFlag_ = mainWindow_->windowModality();
    mainWindow_->installEventFilter(this);
    loadPreference();
	mainWindow_->setStyleSheet(
		"QMainWindow::separator:vertical{background: palette(dark); width: 4px; border-right: 1px solid palette(midlight);}"
		"QMainWindow::separator:vertical{background: palette(dark); width: 4px; border-left: 1px solid palette(midlight);}"
		"QMainWindow::separator:horizontal{background: palette(dark); height: 4px; border-bottom: 1px solid palette(midlight);}"
		"QMainWindow::separator:horizontal{background: palette(dark); height: 4px; border-top: 1px solid palette(midlight);}");
    auto globalSettings = qtFramework_.qtGlobalSettings();
    qtConnections_ += QObject::connect( globalSettings, &QtGlobalSettings::prePreferencesChanged,
        this, &QtWindow::onPrePreferencesChanged );
    qtConnections_ += QObject::connect( globalSettings, &QtGlobalSettings::postPreferencesChanged,
        this, &QtWindow::onPostPreferencesChanged );
    qtConnections_ += QObject::connect( globalSettings, &QtGlobalSettings::prePreferencesSaved,
        this, &QtWindow::onPrePreferencesSaved );
}

bool QtWindow::eventFilter(QObject * obj, QEvent * event)
{
	if (obj == mainWindow_.get())
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

void QtWindow::savePreference()
{
    if(!isReady())
    {
        return;
    }
	auto preferences = qtFramework_.getPreferences();
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

bool QtWindow::loadPreference()
{
	//check the preference data first
	do 
	{
		std::shared_ptr< BinaryBlock > geometry;
		std::shared_ptr< BinaryBlock > state;
		bool isMaximized = false;
		Vector2 pos;
		Vector2 size;
		bool isOk = false;
		auto preferences = qtFramework_.getPreferences();
		if (preferences == nullptr)
		{
			break;
		}
		std::string key = (id_ == "") ? g_internalPreferenceId : id_;
		auto & preference = preferences->getPreference( key.c_str() );
		// check the preferences
		auto accessor = preference->findProperty( "geometry" );
		if (!accessor.isValid())
		{
			break;
		}
		isOk = preference->get( "geometry", geometry );
		if (!isOk)
		{
			break;
		}
		accessor = preference->findProperty( "layoutState" );
		if (!accessor.isValid())
		{
			break;
		}
		isOk = preference->get( "layoutState", state );
		if (!isOk)
		{
			break;
		}
		accessor = preference->findProperty( "maximized" );
		if (!accessor.isValid())
		{
			break;
		}
		isOk = preference->get( "maximized", isMaximized );
		if (!isOk)
		{
			break;
		}
		if (!isMaximized)
		{
			accessor = preference->findProperty( "pos" );
			if (!accessor.isValid())
			{
				break;
			}
			isOk = preference->get( "pos", pos );
			if (!isOk)
			{
				break;
			}
			accessor = preference->findProperty( "size" );
			if (!accessor.isValid())
			{
				break;
			}
			isOk = preference->get( "size", size );
			if (!isOk)
			{
				break;
			}
		}

		// restore preferences
		isMaximizedInPreference_ = isMaximized;
		isOk = mainWindow_->restoreGeometry( QByteArray( geometry->cdata(), static_cast<int>(geometry->length()) ) );
		if (!isOk)
		{
			break;
		}
		isOk = mainWindow_->restoreState( QByteArray( state->cdata(), static_cast<int>(state->length()) ) );
		if (!isOk)
		{
			break;
		}
		if (!isMaximized)
		{
			mainWindow_->move( QPoint( static_cast<int>( pos.x ), static_cast<int>( pos.y ) ) );
			mainWindow_->resize( QSize( static_cast<int>( size.x ), static_cast<int>( size.y ) ) );
		}

		return true;

	} while (false);
	NGT_DEBUG_MSG( "Load Qt Window Preferences Failed.\n" );
	return false;
}


void QtWindow::onPrePreferencesChanged()
{
    savePreference();
}

void QtWindow::onPostPreferencesChanged()
{
    loadingPreferences_ = true;
    loadPreference();
    for(auto& region : regions_)
    {
        QtDockRegion* iRegion = dynamic_cast<QtDockRegion*>(region.get());
        if(iRegion != nullptr)
        {
            iRegion->restoreDockWidgets();
        }
    }
    loadingPreferences_ = false;
}

void QtWindow::onPrePreferencesSaved()
{
   savePreference();
}

bool QtWindow::isLoadingPreferences() const
{
    return loadingPreferences_;
}
} // end namespace wgt
