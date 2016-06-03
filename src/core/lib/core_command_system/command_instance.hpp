#ifndef COMMAND_INSTANCE_HPP
#define COMMAND_INSTANCE_HPP

#include "reflection_undo_redo_data.hpp"
#include "custom_undo_redo_data.hpp"

#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"

#include "core_reflection/reflected_object.hpp"
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
enum class CommandErrorCode : uint8_t;

enum ExecutionStatus
{
	Queued,
	Running,
	Complete
};

class CommandStatusHandler
{
public:
	virtual void setStatus( ExecutionStatus status ) = 0;
};

class CommandInstance;
typedef ObjectHandleT< CommandInstance > CommandInstancePtr;

class UndoRedoData;
typedef std::unique_ptr< UndoRedoData > UndoRedoDataPtr;

//TODO: Pull out interface to remove linkage
/**
 *	CommandInstance stores per-instance data for a type of Command.
 */
class CommandInstance
	: public CommandStatusHandler
{
	DECLARE_REFLECTED

public:
	friend CommandManagerImpl;
	friend ReflectionUndoRedoData;
	friend CustomUndoRedoData;

	typedef XMLSerializer UndoRedoSerializer;

	CommandInstance();
	CommandInstance( const CommandInstance& );
	virtual ~CommandInstance();

	void cancel();

	void execute();
	bool isComplete() const;

	ExecutionStatus getExecutionStatus() const;
	ObjectHandle getArguments() const { return arguments_; }
	ObjectHandle getReturnValue() const { return returnValue_; }

	CommandErrorCode getErrorCode() const;

	bool isMultiCommand() const;

	void undo();
	void redo();

	const char * getCommandId() const;
	void setContextObject( const ObjectHandle & contextObject );

	ICommandManager * getCommandSystemProvider() { return pCmdSysProvider_; }

    ObjectHandle getCommandDescription() const;

	void consolidateUndoRedoData( CommandInstance * parentInstance );

private:
	void waitForCompletion();

	Command * getCommand();
	const Command * getCommand() const;

	void setStatus( ExecutionStatus status );
	void setArguments( const ObjectHandle & arguments );
	void setCommandId( const char * commandName );
	
	void setCommandSystemProvider( ICommandManager * pCmdSysProvider );
	void setDefinitionManager( IDefinitionManager & defManager );

	std::mutex					mutex_;
	IDefinitionManager *		defManager_;
	std::atomic< ExecutionStatus > status_;
	wg_condition_variable		completeStatus_; // assumed predicate: status_ == Complete
	ObjectHandle				arguments_;
	ObjectHandle				returnValue_;
	std::vector< CommandInstancePtr > children_;
	ICommandManager *			pCmdSysProvider_;
	std::string					commandId_;
	ObjectHandle				contextObject_;
	CommandErrorCode			errorCode_;
	std::vector< UndoRedoDataPtr > undoRedoData_;
};
} // end namespace wgt
#endif //COMMAND_INSTANCE_HPP
