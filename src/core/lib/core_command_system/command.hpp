#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <list>

#include "i_command_event_listener.hpp"
#include "command_instance.hpp"
namespace wgt
{
class IDataStream;
class ICommandManager;

enum class CommandErrorCode : uint8_t
{
	COMMAND_NO_ERROR = 0,
	ABORTED,
	FAILED,
	INVALID_VALUE,
	INVALID_ARGUMENTS,
	INVALID_OPERATIONS,
	NOT_SUPPORTED
};

/**
 *	@param errorCode code from command to be checked.
 *	@return true if the command has completed successfully.
 */
bool isCommandSuccess( CommandErrorCode errorCode );

enum class CommandThreadAffinity : uint8_t
{
	NO_THREAD = 0,
	UI_THREAD = 1,
	COMMAND_THREAD = 1 << 1,
	ANY_THREAD = UI_THREAD | COMMAND_THREAD
};

/**
 *	Command specifies the type of actions to be performed.
 */

class Command
{
public:
	virtual ~Command();
	virtual const char * getId() const = 0;
	virtual ObjectHandle execute(const ObjectHandle & arguments) const = 0;


	virtual bool validateArguments(const ObjectHandle & arguments) const 
	{
		//NOTE(aidan): 
		//If there are any conditions that argument objects must follow, override this function. It should
		//just return if the command could execute correctly with the given arguments
		return true; 
	}

	virtual CommandThreadAffinity threadAffinity() const { return CommandThreadAffinity::COMMAND_THREAD; }

	virtual bool customUndo() const { return false; }
	virtual bool canUndo( const ObjectHandle & arguments ) const { return true; }
	virtual bool undo( const ObjectHandle & arguments ) const { return false; }
	virtual bool redo( const ObjectHandle & arguments ) const { return false; }
	virtual ObjectHandle getCommandDescription(const ObjectHandle & arguments) const { return ObjectHandle(); }

	virtual void setCommandSystemProvider( ICommandManager * commandSystemProvider );
	virtual void registerCommandStatusListener( ICommandEventListener * listener );

	ICommandManager * getCommandSystemProvider() const;

	virtual void fireCommandStatusChanged( const CommandInstance & command ) const;
	virtual void fireProgressMade( const CommandInstance & command ) const;
	virtual void fireCommandExecuted( const CommandInstance & command, CommandOperation operation) const;

private:
	typedef std::list< ICommandEventListener * > EventListenerCollection;
	EventListenerCollection eventListenerCollection_;
	ICommandManager * commandSystemProvider_;
};
} // end namespace wgt
#endif //COMMAND_HPP
