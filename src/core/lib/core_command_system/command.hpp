#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <list>

#include "i_command_event_listener.hpp"
#include "command_instance.hpp"
#include "core_variant/variant.hpp"
#include "core_object/i_managed_object.hpp"

namespace wgt
{
class IDataStream;
class ICommandManager;
template <typename T> class ManagedObject;
typedef ManagedObject<class GenericObject> CommandDescription;

/**
 *	@param errorCode code from command to be checked.
 *	@return true if the command has completed successfully.
 */
bool isCommandSuccess(CommandErrorCode errorCode);

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
	virtual const char* getId() const = 0;

	/**
	 *	Run the command.
	 *	@param arguments command data.
	 *	@return the CommandErrorCode or the return value.
	 *		Not returning a CommandErrorCode assumes the code is COMMAND_NO_ERROR.
	 */
	virtual Variant execute(const ObjectHandle& arguments) const = 0;

	virtual bool validateArguments(const ObjectHandle& arguments) const
	{
		// NOTE(aidan):
		// If there are any conditions that argument objects must follow, override this function. It should
		// just return if the command could execute correctly with the given arguments
		return true;
	}

	virtual CommandThreadAffinity threadAffinity() const
	{
		return CommandThreadAffinity::COMMAND_THREAD;
	}

	virtual const char* getName() const
	{
		return getId();
	}

	virtual bool customUndo() const
	{
		return false;
	}
	virtual bool canUndo(const ObjectHandle& arguments) const
	{
		return true;
	}
	virtual bool undo(const ObjectHandle& arguments) const
	{
		return false;
	}
	virtual bool redo(const ObjectHandle& arguments) const
	{
		return false;
	}
	virtual CommandDescription getCommandDescription(const ObjectHandle& arguments) const
	{
		return nullptr;
	}

	virtual const Collection& getSubCommands() const
	{
		return subCommands_;
	}

	virtual void setCommandSystemProvider(ICommandManager* commandSystemProvider);
	virtual void registerCommandStatusListener(ICommandEventListener* listener);

	ICommandManager* getCommandSystemProvider() const;

	virtual void fireCommandStatusChanged(const CommandInstance& command) const;
	virtual void fireProgressMade(const CommandInstance& command) const;
	virtual void fireCommandExecuted(const CommandInstance& command, CommandOperation operation) const;

	// Default implementation to call from a Command's copyArguments implementation.
	template<class Type>
	static ManagedObjectPtr copyArguments(const ObjectHandle& arguments)
	{
		if (Type* data = arguments.getBase<Type>())
		{
			// This assumes there is a copy constructor.
			return ManagedObject<Type>::make_iunique(*data);
		}

		return nullptr;
	}

	// This needs to be overridden to be able to make copies of command arguments for use in macros.
	virtual ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const = 0;

protected:
	Collection subCommands_;

private:
	typedef std::list<ICommandEventListener*> EventListenerCollection;
	EventListenerCollection eventListenerCollection_;
	ICommandManager* commandSystemProvider_;
};
} // end namespace wgt
#endif // COMMAND_HPP
