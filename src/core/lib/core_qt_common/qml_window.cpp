#include "qml_window.hpp"
#include "qt_dock_region.hpp"
#include "qt_menu_bar.hpp"
#include "qt_status_bar.hpp"
#include "qt_tab_region.hpp"
#include "qt_tool_bar.hpp"
#include "i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_logging/logging.hpp"
#include <cassert>
#include <thread>
#include <chrono>
#include <QQmlComponent>
#include <QDockWidget>
#include <QQuickWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QQmlContext>
#include <QQmlEngine>
#include <QApplication>
#include <QWindow>
#include <QElapsedTimer>
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "core_qt_script/qt_script_object.hpp"

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

    const char * g_internalPreferenceId = "13CC6CD7-E935-488D-9E3D-8BED9454F554";
}

QmlWindow::QmlWindow( IQtFramework & qtFramework, QQmlEngine & qmlEngine )
	: qtFramework_( qtFramework )
		, qmlEngine_( qmlEngine )
		, qmlContext_( new QQmlContext( qmlEngine.rootContext() ) )
		, mainWindow_( new QQuickWidget( &qmlEngine, nullptr ) )
		, released_( false )
		, application_( nullptr )
		, isMaximizedInPreference_( false )
		, firstTimeShow_( true )
	{
		mainWindow_->setMinimumSize( QSize( 100, 100 ) );
		QQmlEngine::setContextForObject( mainWindow_, qmlContext_.get() );
	}

QmlWindow::~QmlWindow()
{
	if (!released_)
	{
        this->savePreference();
		mainWindow_->removeEventFilter( this );
		delete mainWindow_;
	}
    qmlEngine_.collectGarbage();
    // call sendPostedEvents to give chance to QScriptObject's DeferredDeleted event get handled in time
    QApplication::sendPostedEvents( nullptr, QEvent::DeferredDelete );
}

void QmlWindow::setContextObject( QObject * object )
{
	auto qtScriptObject = dynamic_cast<QtScriptObject *>( object );
	if(qtScriptObject)
	{
		qtScriptObject->setParent( qmlContext_.get() );
	}
	else
	{
		object->setParent(qmlContext_.get());
	}
	qmlContext_->setContextObject( object );
}

void QmlWindow::setContextProperty(
	const QString & name, const QVariant & property )
{
    if (property.canConvert< QObject * >())
    {
        auto object = property.value< QObject * >();
        if(!object->isWidgetType() && !object->isWindowType())
        {
			auto qtScriptObject = dynamic_cast<QtScriptObject *>( object );
			if(qtScriptObject)
			{
				qtScriptObject->setParent( qmlContext_.get() );
			}
			else
			{
				object->setParent(qmlContext_.get());
			}
        }
    }
	qmlContext_->setContextProperty( name, property );
}

void QmlWindow::error( QQuickWindow::SceneGraphError error, const QString &message )
{
	NGT_ERROR_MSG( "QmlWindow::error, rendering error: %s\n",
		message.toLatin1().constData() );
}

const char * QmlWindow::id() const
{
	return id_.c_str();
}

const char * QmlWindow::title() const
{
	return title_.c_str();
}

void QmlWindow::update()
{
	for (auto & menu : menus_)
	{
		menu->update();
	}
}

void QmlWindow::close()
{
	mainWindow_->close();
}

void QmlWindow::setIcon(const char* path)
{
	if(!path || !mainWindow_)
		return;
	mainWindow_->setWindowIcon(QIcon(path));
}

void QmlWindow::show( bool wait /* = false */)
{
	mainWindow_->setWindowModality( modalityFlag_ );
    if(firstTimeShow_ && isMaximizedInPreference_)
    {
        mainWindow_->setWindowState( Qt::WindowMaximized );
    }
	mainWindow_->show();
	if ( title() )
	{
		mainWindow_->setWindowTitle(title());
	}
    firstTimeShow_ = false;
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QmlWindow::showMaximized( bool wait /* = false */)
{
	mainWindow_->setWindowModality( modalityFlag_ );
	
	mainWindow_->showMaximized();
    firstTimeShow_ = false;
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QmlWindow::showModal()
{
	mainWindow_->setWindowModality( Qt::ApplicationModal );
	if ( title() )
	{
		mainWindow_->setWindowTitle(title());
	}
    if(firstTimeShow_ && isMaximizedInPreference_)
    {
        mainWindow_->setWindowState( Qt::WindowMaximized );
    }
	mainWindow_->show();
    firstTimeShow_ = false;
}

void QmlWindow::hide()
{
	mainWindow_->hide();
}

const Menus & QmlWindow::menus() const
{
	return menus_;
}

const Regions & QmlWindow::regions() const
{
	return regions_;
}

void QmlWindow::setApplication( IUIApplication * application )
{
	application_ = application;
}

IUIApplication * QmlWindow::getApplication() const
{
	return application_;
}

IStatusBar* QmlWindow::statusBar() const
{
	return statusBar_.get();
}

QQuickWidget * QmlWindow::release()
{
	released_ = true;
	return window();
}

QQuickWidget * QmlWindow::window() const
{
	return mainWindow_;
}

void QmlWindow::waitForWindowExposed()
{
	if (mainWindow_ == nullptr)
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

bool QmlWindow::load( QUrl & qUrl )
{
	auto qmlEngine = qmlContext_->engine();
	auto qmlComponent = std::unique_ptr< QQmlComponent >(
		new QQmlComponent( qmlEngine, qUrl, mainWindow_ ) );
	assert( !qmlComponent->isLoading() );
	if (!qmlComponent->isReady())
	{
		NGT_WARNING_MSG( "Error loading control %s\n",
			qPrintable( qmlComponent->errorString() ) );
		return false;
	}

	auto content = std::unique_ptr< QObject >(
		qmlComponent->create( qmlContext_.get() ) );

	QVariant windowProperty = content->property( "id" );
	if (windowProperty.isValid())
	{
		id_ = windowProperty.toString().toUtf8().data();
	}

    auto windowMaximizedProperty = content->property("windowMaximized");
    if (windowMaximizedProperty.isValid())
    {
        isMaximizedInPreference_ = windowMaximizedProperty.toBool();
    }

	QVariant titleProperty = content->property( "title" );
	if (titleProperty.isValid())
	{
		title_ = titleProperty.toString().toUtf8().data();
	}

	auto menuBars = getChildren< QMenuBar >( *mainWindow_ );
	for (auto & menuBar : menuBars)
	{
		if (menuBar->property( "path" ).isValid())
		{
			menus_.emplace_back( new QtMenuBar( *menuBar, id_.c_str() ) );
		}
	}

	auto toolBars = getChildren< QToolBar >( *mainWindow_ );
	for (auto & toolBar : toolBars)
	{
		if (toolBar->property( "path" ).isValid())
		{
			menus_.emplace_back( new QtToolBar( *toolBar, id_.c_str() ) );
		}
	}
	auto dockWidgets = getChildren< QDockWidget >( *mainWindow_ );
	if (!dockWidgets.empty())
	{
		NGT_WARNING_MSG( "Qml window doesn't support docking" );
	}
	auto tabWidgets = getChildren< QTabWidget >( *mainWindow_ );
	for (auto & tabWidget : tabWidgets)
	{
		if ( tabWidget->property( "layoutTags" ).isValid() )
		{
			regions_.emplace_back( new QtTabRegion( qtFramework_, *tabWidget ) );
		}
	}
	auto statusBar = getChildren<QStatusBar>(*mainWindow_);
	if ( statusBar.size() > 0 )
	{
		statusBar_.reset(new QtStatusBar(*statusBar.at(0)));
	}

	auto preferences = qtFramework_.getPreferences();
	auto preference = preferences->getPreference( id_.c_str() );
	auto value = qtFramework_.toQVariant( preference, qmlContext_.get() );
	this->setContextProperty( QString( "Preference" ), value );

	mainWindow_->setContent( qUrl, qmlComponent.release(), content.release() );
	mainWindow_->setResizeMode( QQuickWidget::SizeRootObjectToView );
	QObject::connect( mainWindow_, SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)),
		this, SLOT(error(QQuickWindow::SceneGraphError, const QString&)) );
	mainWindow_->installEventFilter( this );
    loadPreference();
	modalityFlag_ = mainWindow_->windowModality();
	return true;
}

bool QmlWindow::eventFilter( QObject * object, QEvent * event )
{
	if (object == mainWindow_)
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
	return QObject::eventFilter( object, event );
}

void QmlWindow::savePreference()
{
    auto preferences = qtFramework_.getPreferences();
    if (preferences == nullptr)
    {
        return;
    }
    std::string key = id_ + g_internalPreferenceId ;
    auto & preference = preferences->getPreference( key.c_str() );
    QByteArray geometryData = mainWindow_->saveGeometry();
    std::shared_ptr< BinaryBlock > geometry = 
        std::make_shared< BinaryBlock >(geometryData.constData(), geometryData.size(), false );
    preference->set( "geometry", geometry );
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

bool QmlWindow::loadPreference()
{


    //check the preference data first
    do 
    {
        std::shared_ptr< BinaryBlock > geometry;
        bool isMaximized = false;
        Vector2 pos;
        Vector2 size;
        bool isOk = false;
        auto preferences = qtFramework_.getPreferences();
        if (preferences == nullptr)
        {
            break;
        }
        std::string key = id_ + g_internalPreferenceId ;
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
        if (!isMaximized)
        {
            mainWindow_->move( QPoint( static_cast<int>( pos.x ), static_cast<int>( pos.y ) ) );
            mainWindow_->resize( QSize( static_cast<int>( size.x ), static_cast<int>( size.y ) ) );
        }

        return true;

    } while (false);
    NGT_DEBUG_MSG( "Load Qml Window Preferences Failed.\n" );
    return false;
}
} // end namespace wgt
