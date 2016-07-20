#include "progress_manager.hpp"

#include "core_command_system/command_instance.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/i_command_event_listener.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"
#include "core_logging_system/log_level.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"

#include <QAbstractButton>
#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QObject>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QTimer>


namespace wgt
{
ProgressManager::ProgressManager( IComponentContext & contextManager )
	: Depends( contextManager )
	, rootObject_( nullptr )
	, progressValue_( 0 )
	, isMultiCommandProgress_( false )
	, view_( nullptr )
	, isViewVisible_( false )
	, timer_( nullptr )
	, prevFocusedWindow_( nullptr )
{
}

ProgressManager::~ProgressManager()
{
    if( view_ != nullptr )
    {
        view_->removeEventFilter( this );
        view_->hide();

        delete view_;
        view_ = nullptr;
    }

    if( timer_ != nullptr )
    {
        timer_->stop();

        delete timer_;
        timer_ = nullptr;
    }
}

/// Cache the context manager and register command status listener
void ProgressManager::init()
{
	ICommandManager * commandSystemProvider = get< ICommandManager >();
	assert ( nullptr != commandSystemProvider );

	commandSystemProvider->registerCommandStatusListener( this );

	auto qtFramework = get< IQtFramework >();
	if (qtFramework == nullptr)
	{
		return;
	}

	// Thread that the view is created
	threadId_ = std::this_thread::get_id();

	auto engine = qtFramework->qmlEngine();

	auto context = new QQmlContext( engine->rootContext(), engine->rootContext() );

	view_ = new QQuickView( engine, nullptr );

	QQmlEngine::setContextForObject( view_, context );

	QUrl qurl = QtHelpers::resolveQmlPath( *engine, "WGProgressManager/progress_manager.qml" );

	auto qmlComponent = new QQmlComponent( engine, view_ );

	QmlComponentLoaderHelper helper(qmlComponent, qurl );
	using namespace std::placeholders;
	helper.data_->sig_Loaded_.connect([ this, qurl, context ] ( QQmlComponent * component )
	{
		rootObject_ = component->create( context );

		view_->setContent( qurl, component, rootObject_ );
		view_->setResizeMode( QQuickView::SizeRootObjectToView );
		view_->setModality( Qt::ApplicationModal );

		QObject::connect( rootObject_, SIGNAL( progressCancelled( bool ) ), this, SLOT( onProgressCancelled( bool ) ) );

		// TODO: Need a way to detect the thread changed event (back to the main thread) and
		//		 replace this polling to hide the view_.
		//
		// Currently, using a timer here to make sure we are back to the main thread where we've
		// created the view_ to avoid the thread warning when we try to send a message to
		// the view_ from a different thread.
		timer_ = new QTimer();
		QObject::connect( timer_, SIGNAL( timeout() ), this, SLOT( timedUpdate() ) );
		timer_->setInterval( 1000 );
		timer_->start();

		view_->installEventFilter( this );
	});
	helper.load( true );
}


/// Deregister command status listener
void ProgressManager::fini()
{
	ICommandManager * commandSystemProvider = get< ICommandManager >();
	assert ( nullptr != commandSystemProvider );
	commandSystemProvider->deregisterCommandStatusListener( this );
}


/// ICommandEventListener implementation.
/// Let the QML know about the status change.
void ProgressManager::statusChanged( const CommandInstance & commandInstance ) const
{
	ExecutionStatus curCommandStatus = commandInstance.getExecutionStatus();
	switch( curCommandStatus )
	{
	case Queued:
		{
			createQQuickView( commandInstance.getCommandId() );
			break;
		}

	case Complete:
		{
			progressCompleted( commandInstance.getCommandId() );
			break;
		}

	case Running:	// Intentional fall through
	default:
		{
			// Do nothing!
			break;
		}
	}
}


/// ICommandEventListener implementation.
/// Let the QML know about the progress change.
void ProgressManager::progressMade( const CommandInstance & commandInstance ) const
{
	ExecutionStatus curCommandStatus = commandInstance.getExecutionStatus();
	switch( curCommandStatus )
	{
	case Running:
		{
			perform();
			break;
		}

	case Complete:
		{
			// Set the progress control value to 100
			break;
		}

	case Queued:	// Intentional fall through
	default:
		{
			// Do nothing!
			break;
		}
	}
}


/// ICommandEventListener implementation.
/// Handle multi commands begin / complete events
void ProgressManager::multiCommandStatusChanged( MultiCommandStatus multiCommandStatus ) const
{
	// TODO: Incoming command is multi (batch / compound)
	//		 command. Prepare the widget for it.
	switch ( multiCommandStatus )
	{
	case ICommandEventListener::MultiCommandStatus_Begin:
		{
			isMultiCommandProgress_ = true;
			createQQuickView();
			break;
		}

	case ICommandEventListener::MultiCommandStatus_Cancel:
		{
			break;
		}

	case ICommandEventListener::MultiCommandStatus_Complete:
		{
			isMultiCommandProgress_ = false;
			progressCompleted();
			break;
		}

	default:
		{
			// Do nothing!
			break;
		}
	}
}


/// ICommandEventListener implementation.
/// Display status (progress bar) on the command being queued
void ProgressManager::handleCommandQueued( const char * commandId ) const
{
	// The command being queued with this commandId will show the status (progress dialog)
	commandIdList_.push_back( commandId );
}


/// ICommandEventListener implementation.
/// Present the non-blocking process to the user ( with the alert manager )
void ProgressManager::onNonBlockingProcessExecution( const char * commandId ) const
{
	auto loggingSystem = get< ILoggingSystem >();
	if ( loggingSystem == nullptr )
	{
		// Logging system is not available, nothing to handle.
		assert( false && "Missing - plg_logging_system" );

		// Early return here!
		return;
	}
	else
	{
		AlertManager * alertManager = loggingSystem->getAlertManager();
		if ( alertManager != nullptr )
		{
			loggingSystem->log( LOG_ALERT, "Command [ %s ] is executed!", commandId);
		}
	}
}


/// Create a QQuickView
void ProgressManager::createQQuickView( const char * commandId ) const
{
	progressValue_ = 0;

	curCommandId_ = std::find( commandIdList_.begin(), commandIdList_.end(), commandId );
	// We only care the commandIds in our list, see if this is in our list commandId 
	if ( commandIdList_.end() == curCommandId_ )
	{
		return;
	}

	if ( !isViewVisible_ )
	{
		if ( rootObject_ && commandId )
		{
			bool isSet = false;
			isSet = rootObject_->setProperty( "progressCommandId", commandId );
			assert( true == isSet );
		}

		view_->show();

		isViewVisible_ = true;
	}
	else if ( isMultiCommandProgress_ )
	{
		if ( rootObject_ && commandId )
		{
			bool isSet = false;
			isSet = rootObject_->setProperty( "progressCommandId", commandId );
			assert( true == isSet );
		}
	}
}


/// Clean up when a command is completed
void ProgressManager::progressCompleted( const char * commandId ) const
{
	if ( true == isViewVisible_ && false == isMultiCommandProgress_ )
	{
		progressValue_ = 100;

		setProgressValueProperty();

		// Hide the view
		isViewVisible_ = false;
	}

	// Remove this commandId
	removeCommand( commandId );
}


/// Increment the progress dialog value when the command made a progress
void ProgressManager::perform() const
{
	if ( isViewVisible_ )
	{
		progressValue_++;

		setProgressValueProperty();
	}
}


/// Create auto close message box
void ProgressManager::createAutoCloseMessageBox( const QString & title, const QString & message ) const
{
	QMessageBox * msgBox = new QMessageBox();
	msgBox->setAttribute( Qt::WA_DeleteOnClose );
	msgBox->setIcon( QMessageBox::Information );
	msgBox->setStandardButtons( QMessageBox::Ok );
	// Auto close in 5 seconds
	msgBox->button( QMessageBox::Ok )->animateClick( 5000 );
	msgBox->setWindowTitle( title );
	msgBox->setText( message );
	msgBox->setModal( false );
	msgBox->open();
}


/// Set "ProgressValue" QML property value with current progressValue_
void ProgressManager::setProgressValueProperty() const
{
	if ( rootObject_ )
	{
		bool isSet = false;
		isSet = rootObject_->setProperty( "progressValue", progressValue_ );
		assert( true == isSet );

		// Process pending dialog events thread, so the dialog displays the updated value
		QCoreApplication::processEvents( QEventLoop::DialogExec );
	}
}


/// Remove a command from our list
void ProgressManager::removeCommand( const char * commandId ) const
{
	auto commandIdToRemove = std::remove( commandIdList_.begin(), commandIdList_.end(), commandId );
	commandIdList_.erase( commandIdToRemove, commandIdList_.end() );

	// Reset the current
	curCommandId_ = commandIdList_.end();
}


/// Handle events from the QQuickView
bool ProgressManager::eventFilter( QObject * object, QEvent * event )
{
	switch ( event->type() )
	{
	case QEvent::Close:
		{
			// We should cancel the command if we close the view before the command is completed
			if ( isCurrentCommandActive() )
			{
				// Need to cancel current command
				cancelCurrentCommand();
			}
		}
		break;

	case QEvent::Show:
		{
			// Keep track of the focused window before we show the progress manager view, so we can request activation of
			// the previous window when we hide the view. We are tracking of the previous window because we want the view to be
			// modal that take focus away from the main window and our Qt framework has a bug in it.
			//
			// NOTE: It seems there is a bug when using the Qt::WindowDoesNotAcceptFocus flag that views don't handle
			//		 the mouse position correctly. Our progress manager view could be setup with Qt::WindowDoesNotAcceptFocus
			//		 flag to disable windows title bar buttons (minimize, maximize, and close), but the buttons in our qml
			//		 doesn't handle the mouse position correctly - i.e. the user has to click below a button to trigger
			//		 a button pressed event.
			prevFocusedWindow_ = QApplication::focusWindow();
		}
		break;

	case QEvent::Hide:
		{
			if ( nullptr != prevFocusedWindow_ )
			{
				// Request activation of the previous window, so it is brought to the top level when activated
				prevFocusedWindow_->requestActivate();
				prevFocusedWindow_ = nullptr;
			}
		}
		break;
	default:
		break;
	}

	return QObject::eventFilter(object, event );
}


/// Cancel the current command
void ProgressManager::cancelCurrentCommand()
{
	// TODO: This is broken. This does not actually cancel the current command.
	// It simply waits for the current command to finish and then undoes it.

	// Make sure the current command is in our list to cancel
	if (isCurrentCommandActive())
	{
		ICommandManager * commandSystemProvider = get< ICommandManager >();
		assert(nullptr != commandSystemProvider);

		// NOTE: By calling undo function here, the status changes to Complete which will trigger our
		//		 progressCompleted function that will remove a command from our list.
		if (commandSystemProvider->canUndo())
		{
			commandSystemProvider->undo();
		}
		
	}
}


/// Slots
/// Handle the progress cancel
void ProgressManager::onProgressCancelled(bool cancelled)
{
	if ( cancelled )
	{
		view_->hide();

		isViewVisible_ = false;

		cancelCurrentCommand();
	}
}


/// Slots
/// Hide the view when possible
void ProgressManager::timedUpdate()
{
	// Wait until we go back to main thread where the view's been created
	if ( std::this_thread::get_id() == threadId_ && !isViewVisible_ )
	{
		// Hide the view when it is active or visible without focus
		if ( view_->isActive() || view_->isVisible() )
		{
			view_->hide();
		}
	}
}
} // end namespace wgt
