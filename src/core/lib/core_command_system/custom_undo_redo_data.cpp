#include "custom_undo_redo_data.hpp"
#include "command_instance.hpp"
#include "command.hpp"

namespace wgt
{
CustomUndoRedoData::CustomUndoRedoData( CommandInstance & commandInstance )
	: commandInstance_( commandInstance )
{

}


void CustomUndoRedoData::undo()
{
	auto command = commandInstance_.getCommand();
	auto commandArgs = commandInstance_.getArguments();
	command->undo( commandArgs );
}


void CustomUndoRedoData::redo()
{
	auto command = commandInstance_.getCommand();
	auto commandArgs = commandInstance_.getArguments();
	command->redo( commandArgs );
}
} // end namespace wgt
