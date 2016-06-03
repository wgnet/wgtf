#ifndef PROGRESS_MANAGER_HPP
#define PROGRESS_MANAGER_HPP

#include "core_command_system/i_command_event_listener.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include <QObject>
#include <QString>
#include <vector>
#include <thread>


class QQuickView;
class QTimer;
class QWindow;

namespace wgt
{
class IComponentContext;
class ILoggingSystem;

typedef std::vector< QString > CommandIdList;

class ProgressManager
	: public QObject
	, public ICommandEventListener
{
	Q_OBJECT

public slots:
	/// Handle the progress cancel
	void onProgressCancelled(bool cancelled);

	/// Hide the view when possible
	void timedUpdate();

public:
	ProgressManager();
	~ProgressManager();

	/// Cache the context manager and register command status listener
	void init( IComponentContext & contextManager );

	/// Deregister command status listener
	void fini();

	/// ICommandEventListener implementation.
	/// Let the QML know about the status change.
	void statusChanged( const CommandInstance & commandInstance ) const override;

	/// ICommandEventListener implementation.
	/// Let the QML know about the progress change.
	void progressMade( const CommandInstance & commandInstance ) const override;

	/// ICommandEventListener implementation.
	/// Handle multi commands begin / complete events
	void multiCommandStatusChanged( MultiCommandStatus multiCommandStatus ) const override;

	/// ICommandEventListener implementation.
	/// Display status (progress bar) on the command being queued
	void handleCommandQueued( const char * commandId ) const override;

	/// ICommandEventListener implementation.
	/// Present the non-blocking process to the user ( with the alert manager )
	void onNonBlockingProcessExecution( const char * commandId ) const override;

	/// Create a QQuickView
	void createQQuickView( const char * commandId = nullptr ) const;

	/// Clean up when a command is completed
	void progressCompleted( const char * commandId = nullptr ) const;

	/// Increment the progress dialog value when the command made a progress
	void perform() const;

	/// Create auto close message box
	void createAutoCloseMessageBox( const QString & title, const QString & message ) const;

	/// Set "ProgressValue" QML property value with current progressValue_
	void setProgressValueProperty() const;

	/// Remove a command from our list
	void removeCommand( const char * commandId = nullptr ) const;

	/// Handle events from the QQuickView
	bool eventFilter( QObject * object, QEvent * event ) override;

	/// Cancel the current command
	void cancelCurrentCommand();

	QObject * getRootObject() const { return rootObject_; }

private:
	mutable QObject * rootObject_;
	mutable int progressValue_;
	mutable bool isMultiCommandProgress_;
	mutable QQuickView * view_;
	mutable IComponentContext * contextManager_;
	mutable CommandIdList commandIdList_;
	mutable CommandIdList::iterator curCommandId_;
	mutable bool isViewVisible_;
	mutable std::thread::id threadId_;

	QTimer * timer_;
	ILoggingSystem * loggingSystem_;
	QWindow * prevFocusedWindow_;

	bool isCurrentCommandActive() { return (  commandIdList_.size() > 0 && commandIdList_.end() != curCommandId_  ); }

};
} // end namespace wgt
#endif // PROGRESS_MANAGER_HPP
