#ifndef UNDO_REDO_COMMAND_HPP
#define UNDO_REDO_COMMAND_HPP

#include "command.hpp"

namespace wgt
{
class ObjectHandle;
class IDataStream;
class CommandManager;
namespace
{
	class CommandManagerImpl;
}

class UndoRedoCommand 
	: public Command
{
public:
	const char * getId() const override;
	ObjectHandle execute( const ObjectHandle & arguments ) const override;
	CommandThreadAffinity threadAffinity() const override;

private:
	friend CommandManagerImpl;
	UndoRedoCommand( CommandManager * pCommandManager );
	CommandManager * pCommandManager_;
};
} // end namespace wgt
#endif//UNDO_REDO_COMMAND_HPP
