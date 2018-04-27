#ifndef I_COMMAND_MANAGER_HPP
#define I_COMMAND_MANAGER_HPP

#include "command_instance.hpp"
#include "i_command_event_listener.hpp"
#include "core_common/signal.hpp"
#include "core_variant/collection.hpp"
#include "core_object/i_managed_object.hpp"
#include <thread>

namespace wgt
{
class IValueChangeNotifier;
class ISerializer;
class Command;
class ISelectionContext;
class ObjectHandle;

class ISelectionContext
{
public:
	virtual const ObjectHandle& getContextObject() const = 0;
	virtual void setContextObject(const ObjectHandle& contextObject) = 0;
};

class ICommandManager
{
	typedef Signal<void(const Collection&)> SignalReset;
	typedef Signal<void(int)> SignalIndexChanged;

public:
    virtual ~ICommandManager() = default;

	virtual void fini() = 0;
	virtual void registerCommand(Command* command) = 0;
	virtual void deregisterCommand(const char* commandId) = 0;
	virtual Command* findCommand(const char* commandId) const = 0;

    virtual CommandInstancePtr queueCommand(const char* commandId) = 0;
	virtual CommandInstancePtr queueCommand(const char* commandId, const ObjectHandle& arguments) = 0;
    virtual CommandInstancePtr queueCommand(const char* commandId, ManagedObjectPtr arguments) = 0;

	virtual void waitForInstance(const CommandInstancePtr& instance) = 0;
	virtual void registerCommandStatusListener(ICommandEventListener* listener) = 0;
	virtual void deregisterCommandStatusListener(ICommandEventListener* listener) = 0;
	virtual void fireCommandStatusChanged(const CommandInstance& command) const = 0;
	virtual void fireProgressMade(const CommandInstance& command) const = 0;
	virtual void fireCommandExecuted(const CommandInstance& command, CommandOperation operation) const = 0;

	virtual void undo() = 0;
	virtual void redo() = 0;

	virtual bool canUndo() const = 0;
	virtual bool canRedo() const = 0;

	typedef std::function<bool(const CommandInstancePtr&)> TRemoveFunctor;
	virtual void removeCommands(const TRemoveFunctor& functor) = 0;

	virtual const Collection& getHistory() const = 0;
	virtual const int commandIndex() const = 0;
	virtual void moveCommandIndex(int newIndex) = 0;
	virtual Collection& getMacros() const = 0;
	virtual bool createMacro(const Collection& commandInstanceList, const char* id = "") = 0;
	virtual bool deleteMacroByName(const char* id) = 0;

	virtual void beginBatchCommand() = 0;
	virtual void endBatchCommand(const char* description = "") = 0;
	virtual void abortBatchCommand() = 0;

	/// Notifies for Progress Manager
	virtual void notifyBeginMultiCommand() = 0;
	virtual void notifyCompleteMultiCommand() = 0;
	virtual void notifyCancelMultiCommand() = 0;
	virtual void notifyHandleCommandQueued(const char* commandId) = 0;
	virtual void notifyNonBlockingProcessExecution(const char* commandId) = 0;

	virtual ISelectionContext& selectionContext() = 0;

	virtual std::thread::id ownerThreadId() = 0;
	virtual bool executingCommandGroup() = 0;

	SignalReset signalHistoryPreReset;
	SignalReset signalHistoryPostReset;
	SignalIndexChanged signalPreCommandIndexChanged;
	SignalIndexChanged signalPostCommandIndexChanged;
};
} // end namespace wgt
#endif // I_COMMAND_MANAGER_HPP
