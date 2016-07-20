#include "command.hpp"
#include "i_command_manager.hpp"

namespace wgt
{

bool isCommandSuccess( CommandErrorCode errorCode )
{
	return (errorCode == CommandErrorCode::COMMAND_NO_ERROR);
}

//==============================================================================
Command::~Command()
{
	eventListenerCollection_.clear();
	commandSystemProvider_ = nullptr;
}

//==============================================================================
void Command::registerCommandStatusListener(
	ICommandEventListener * listener )
{
	eventListenerCollection_.push_back( listener );
}


//==============================================================================
void Command::setCommandSystemProvider(
	ICommandManager * commandSystemProvider )
{
	commandSystemProvider_ = commandSystemProvider;
}


//==============================================================================
ICommandManager * Command::getCommandSystemProvider() const
{
	return commandSystemProvider_;
}


//==============================================================================
void Command::fireCommandStatusChanged(
	const CommandInstance & command ) const
{
	EventListenerCollection::const_iterator it =
		eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd =
		eventListenerCollection_.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->statusChanged( command );
	}
}


//==============================================================================
void Command::fireProgressMade( const CommandInstance & command ) const
{
	EventListenerCollection::const_iterator it =
		eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd =
		eventListenerCollection_.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->progressMade( command );
	}
}

void Command::fireCommandExecuted(const CommandInstance & command, CommandOperation operation) const
{
	EventListenerCollection::const_iterator it =
		eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd =
		eventListenerCollection_.end();
	for (; it != itEnd; ++it)
	{
		(*it)->commandExecuted(command, operation);
	}
}

//==============================================================================
/*virtual */ObjectHandle Command::execute(
	const ObjectHandle & arguments ) const
{
	return nullptr;
}
} // end namespace wgt
