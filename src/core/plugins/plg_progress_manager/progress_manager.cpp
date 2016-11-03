#include "progress_manager.hpp"

#include "core_command_system/command_instance.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/i_command_event_listener.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"
#include "core_logging_system/log_level.hpp"


namespace wgt
{
ProgressManager::ProgressManager( IComponentContext & contextManager )
	: Depends( contextManager )
	, progressValue_( 0 )
	, isMultiCommandProgress_( false )
	, isViewVisible_( false )
{
}

ProgressManager::~ProgressManager()
{
}

/// Cache the context manager and register command status listener
void ProgressManager::init()
{
	ICommandManager * commandSystemProvider = get< ICommandManager >();
	assert ( nullptr != commandSystemProvider );

	commandSystemProvider->registerCommandStatusListener( this );
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
	curCommandId_ = std::find(commandIdList_.begin(), commandIdList_.end(), commandInstance.getCommandId());
	// We only care the commandIds in our list, see if this is in our list commandId
	if (commandIdList_.end() == curCommandId_)
	{
		return;
	}
	ExecutionStatus curCommandStatus = commandInstance.getExecutionStatus();
	switch( curCommandStatus )
	{
	case Queued:
		{
		    createProgressDialog(commandInstance.getCommandId());
		    break;
		}

	case Complete:
		{
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
	curCommandId_ = std::find(commandIdList_.begin(), commandIdList_.end(), commandInstance.getCommandId());
	// We only care the commandIds in our list, see if this is in our list commandId
	if (commandIdList_.end() == curCommandId_)
	{
		return;
	}
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
		    progressCompleted(commandInstance.getCommandId());
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
		    createProgressDialog();
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
void ProgressManager::createProgressDialog(const char* commandId) const
{
	progressValue_ = 0;

	curCommandId_ = std::find( commandIdList_.begin(), commandIdList_.end(), commandId );
	// We only care the commandIds in our list, see if this is in our list commandId 
	if ( commandIdList_.end() == curCommandId_ )
	{
		return;
	}
	auto qtFramework = get<IUIFramework>();
	if (qtFramework == nullptr)
	{
		return;
	}
	if (dlg_ == nullptr)
	{
		dlg_ = qtFramework->createProgressDialog("Command In Progress", commandId, "Cancel", 5000);
	}

	isViewVisible_ = true;
	dlg_->setValue(progressValue_);
	dlg_->setLabelText(commandId);
	dlg_->show();
}


/// Clean up when a command is completed
void ProgressManager::progressCompleted( const char * commandId ) const
{
	if (dlg_ == nullptr)
	{
		return;
	}
	if (dlg_->wasCanceled())
	{
		cancelCurrentCommand();
		isViewVisible_ = false;
		dlg_.reset();
	}
	else if (true == isViewVisible_ && false == isMultiCommandProgress_)
	{
		isViewVisible_ = false;

		progressValue_ = dlg_->maximum();

		setProgressValueProperty();
		dlg_.reset();
	}
	// Remove this commandId
	removeCommand(commandId);
}


/// Increment the progress dialog value when the command made a progress
void ProgressManager::perform() const
{
	if ( isViewVisible_ )
	{
		progressValue_++;
		if (progressValue_ == dlg_->maximum())
		{
			return;
		}
		setProgressValueProperty();
	}
}


/// Set "ProgressValue" QML property value with current progressValue_
void ProgressManager::setProgressValueProperty() const
{
	if (dlg_ && !dlg_->wasCanceled())
	{
		dlg_->setValue(progressValue_);
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


/// Cancel the current command
void ProgressManager::cancelCurrentCommand() const
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
} // end namespace wgt
