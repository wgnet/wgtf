#ifndef COMMAND_INSTANCE_HPP
#define COMMAND_INSTANCE_HPP

#include "reflection_undo_redo_data.hpp"
#include "custom_undo_redo_data.hpp"

#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_object/managed_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

#include <mutex>
#include "core_common/wg_condition_variable.hpp"

namespace wgt
{
namespace
{
class CommandManagerImpl;
}
class Command;
class ICommandManager;
class IDefinitionManager;
class BinaryBlock;
template <typename T> class ManagedObject;
typedef ManagedObject<class GenericObject> CommandDescription;

enum CommandErrorCode
{
	COMMAND_NO_ERROR = 0,
	BATCH_NO_ERROR,
	ABORTED,
	FAILED,
	INVALID_VALUE,
	INVALID_ARGUMENTS,
	INVALID_OPERATIONS,
	NOT_SUPPORTED
};

enum ExecutionStatus
{
	Queued,
	Running,
	Complete
};

class CommandStatusHandler
{
public:
	virtual void setStatus(ExecutionStatus status) = 0;
};

typedef std::unique_ptr<class UndoRedoData> UndoRedoDataPtr;
typedef std::shared_ptr<class CommandInstance> CommandInstancePtr;

// TODO: Pull out interface to remove linkage
/**
 *	CommandInstance stores per-instance data for a type of Command.
 */
class CommandInstance : public CommandStatusHandler
{
public:
	friend CommandManagerImpl;
	friend ReflectionUndoRedoData;
	friend CustomUndoRedoData;

	typedef XMLSerializer UndoRedoSerializer;

	CommandInstance();
	virtual ~CommandInstance();

	void cancel();

	void execute();
	bool isComplete() const;

	ExecutionStatus getExecutionStatus() const;
	const ObjectHandle& getArguments() const
	{
		return arguments_;
	}
	const Variant& getReturnValue() const
	{
		return returnValue_;
	}

	CommandErrorCode getErrorCode() const;

	/**
	 *	Check if this command contains child commands.
	 *	@note an empty BatchCommand will return false for hasChildren().
	 *	@return if the command contains any children.
	 */
	bool hasChildren() const;

	Collection getChildren() const;

	bool undo();
	bool redo();

	const char* getCommandId() const;
	Command* getCommand();
	const Command* getCommand() const;
	void setContextObject(const ObjectHandle& contextObject);

	ICommandManager* getCommandSystemProvider()
	{
		return pCmdSysProvider_;
	}

	ObjectHandle getCommandDescription() const;

	void consolidateUndoRedoData(CommandInstance* parentInstance);

	/**
	*	Compress command children.
	*	Sequential commands that set the same property on the same object will be compressed to one command.
	*/
	void consolidateChildren();

private:
	// Disable copy and move
	CommandInstance(const CommandInstance&);
	CommandInstance(CommandInstance&&);
	CommandInstance& operator=(const CommandInstance&);
	CommandInstance& operator=(CommandInstance&&);

	void waitForCompletion();

	void setStatus(ExecutionStatus status);
    void setArguments(const std::nullptr_t&);
	void setArguments(const ObjectHandle& arguments);
    void setArguments(ManagedObjectPtr arguments);
	void setCommandId(const char* commandName);

	void setCommandSystemProvider(ICommandManager* pCmdSysProvider);
	void setDefinitionManager(IDefinitionManager& defManager);
    ObjectHandle setCommandDescription(CommandDescription description) const;

	std::mutex mutex_;
	IDefinitionManager* defManager_;
	std::atomic<ExecutionStatus> status_;
	wg_condition_variable completeStatus_; // assumed predicate: status_ == Complete
	ObjectHandle arguments_;
    ManagedObjectPtr argumentsStorage_; // if owning the arguments
	Variant returnValue_;
	std::vector<CommandInstancePtr> children_;
	ICommandManager* pCmdSysProvider_;
	std::string commandId_;
	ObjectHandle contextObject_;
	CommandErrorCode errorCode_;
	std::vector<UndoRedoDataPtr> undoRedoData_;
    mutable ManagedObject<GenericObject> description_;
};
} // end namespace wgt
#endif // COMMAND_INSTANCE_HPP
