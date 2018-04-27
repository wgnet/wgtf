#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "command_instance.hpp"
#include "i_command_manager.hpp"

#include <functional>
#include <vector>

namespace wgt
{
class IApplication;
class BatchCommand;
class UndoRedoCommand;
class IEnvManager;
class IFileSystem;
class IReflectionController;
class LockedState;

class SelectionContext : public ISelectionContext
{
	virtual const ObjectHandle& getContextObject() const override
	{
		return contextObject_;
	}

	virtual void setContextObject(const ObjectHandle& contextObject) override
	{
		contextObject_ = contextObject;
	}

private:
	ObjectHandle contextObject_;
};

class CommandManager : public Implements<ICommandManager>
{
	typedef std::vector<ICommandEventListener*> EventListenerCollection;

public:
	CommandManager(IEnvManager& envManager);
	virtual ~CommandManager();

	void init(IApplication& application, IDefinitionManager& defManager );

	void fini() override;

	// From ICommandManager begin
	void registerCommand(Command* command) override;
	void deregisterCommand(const char* commandId) override;
	Command* findCommand(const char* commandId) const override;
	void waitForInstance(const CommandInstancePtr& instance) override;

    virtual CommandInstancePtr queueCommand(const char* commandId) override;
    virtual CommandInstancePtr queueCommand(const char* commandId, const ObjectHandle& arguments) override;
    virtual CommandInstancePtr queueCommand(const char* commandId, ManagedObjectPtr arguments) override;

	void registerCommandStatusListener(ICommandEventListener* listener) override;
	void deregisterCommandStatusListener(ICommandEventListener* listener) override;
	void fireCommandStatusChanged(const CommandInstance& command) const override;
	void fireProgressMade(const CommandInstance& command) const override;
	void fireCommandExecuted(const CommandInstance& command, CommandOperation operation) const override;
	void undo() override;
	void redo() override;
	bool canUndo() const override;
	bool canRedo() const override;

	void removeCommands(const TRemoveFunctor& functor) override;

	const Collection& getHistory() const override;
	const int commandIndex() const override;
	void moveCommandIndex(int newIndex) override;
	Collection& getMacros() const override;
	bool createMacro(const Collection& commandInstanceList, const char* id = "") override;
	bool deleteMacroByName(const char* id) override;

	void beginBatchCommand() override;
	void endBatchCommand(const char* description = "") override;
	void abortBatchCommand() override;
	void notifyBeginMultiCommand() override;
	void notifyCompleteMultiCommand() override;
	void notifyCancelMultiCommand() override;
	void notifyHandleCommandQueued(const char* commandId) override;
	void notifyNonBlockingProcessExecution(const char* commandId) override;
	ISelectionContext& selectionContext() override;
	virtual std::thread::id ownerThreadId() override;
	virtual bool executingCommandGroup() override;
	// From ICommandManager end

	IDefinitionManager& getDefManager() const;

private:
	friend UndoRedoCommand;
	void addToHistory(const CommandInstancePtr& instance);
	bool undoRedo(int& desiredIndex);
	void multiCommandStatusChanged(ICommandEventListener::MultiCommandStatus status);
	std::unique_ptr<class CommandManagerImpl> pImpl_;
	IDefinitionManager * defManager_;
	IFileSystem* fileSystem_;
	IReflectionController* controller_;
	SelectionContext selectionContext_;
	EventListenerCollection eventListenerCollection_;
};

class CommandManagerEventListener : public ICommandEventListener
{
public:
	void setCommandSystemProvider(ICommandManager* commandSystemProvider)
	{
		commandSystemProvider_ = commandSystemProvider;
	}

private:
	ICommandManager* commandSystemProvider_;

	void statusChanged(const CommandInstance& commandInstance) const override
	{
		commandSystemProvider_->fireCommandStatusChanged(commandInstance);
	}

	void progressMade(const CommandInstance& commandInstance) const override
	{
		commandSystemProvider_->fireProgressMade(commandInstance);
	}

	void commandExecuted(const CommandInstance& commandInstance, CommandOperation operation) override
	{
		commandSystemProvider_->fireCommandExecuted(commandInstance, operation);
	}
};
} // end namespace wgt
#endif // COMMAND_MANAGER_HPP
