#ifndef PROGRESS_MANAGER_HPP
#define PROGRESS_MANAGER_HPP

#include "core_command_system/i_command_event_listener.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

#include <vector>

namespace wgt
{
class IComponentContext;
class ILoggingSystem;

typedef std::vector<std::string> CommandIdList;

class ProgressManager : public ICommandEventListener, public Depends<ILoggingSystem, ICommandManager, IUIFramework>
{
public:
	ProgressManager(IComponentContext& contextManager);
	~ProgressManager();

	/// Register command status listener
	void init();

	/// Deregister command status listener
	void fini();

private:
	/// ICommandEventListener implementation.
	/// Let the QML know about the status change.
	void statusChanged(const CommandInstance& commandInstance) const override;

	/// ICommandEventListener implementation.
	/// Let the QML know about the progress change.
	void progressMade(const CommandInstance& commandInstance) const override;

	/// ICommandEventListener implementation.
	/// Handle multi commands begin / complete events
	void multiCommandStatusChanged(MultiCommandStatus multiCommandStatus) const override;

	/// ICommandEventListener implementation.
	/// Display status (progress bar) on the command being queued
	void handleCommandQueued(const char* commandId) const override;

	/// ICommandEventListener implementation.
	/// Present the non-blocking process to the user ( with the alert manager )
	void onNonBlockingProcessExecution(const char* commandId) const override;

	/// Create a QQuickView
	void createProgressDialog(const char* commandId = nullptr) const;

	/// Clean up when a command is completed
	void progressCompleted(const char* commandId = nullptr) const;

	/// Increment the progress dialog value when the command made a progress
	void perform() const;

	/// Set "ProgressValue" QML property value with current progressValue_
	void setProgressValueProperty() const;

	/// Remove a command from our list
	void removeCommand(const char* commandId = nullptr) const;

	/// Cancel the current command
	void cancelCurrentCommand() const;

	bool isCurrentCommandActive() const
	{
		return (commandIdList_.size() > 0 && commandIdList_.end() != curCommandId_);
	}
	mutable int progressValue_;
	mutable bool isMultiCommandProgress_;
	mutable CommandIdList commandIdList_;
	mutable CommandIdList::iterator curCommandId_;
	mutable bool isViewVisible_;
	mutable IProgressDialogPtr dlg_;
};
} // end namespace wgt
#endif // PROGRESS_MANAGER_HPP
