#include "custom_undo_redo_data.hpp"
#include "command_instance.hpp"
#include "command.hpp"

namespace wgt
{
CustomUndoRedoData::CustomUndoRedoData(CommandInstance& commandInstance) : commandInstance_(commandInstance)
{
}

bool CustomUndoRedoData::undo()
{
	auto command = commandInstance_.getCommand();
	auto commandArgs = commandInstance_.getArguments();
	return command->undo(commandArgs);
}

bool CustomUndoRedoData::redo()
{
	auto command = commandInstance_.getCommand();
	auto commandArgs = commandInstance_.getArguments();
	return command->redo(commandArgs);
}

CommandDescription CustomUndoRedoData::getCommandDescription() const /* override */
{
	const auto pCommand = commandInstance_.getCommand();
	const auto commandArgs = commandInstance_.getArguments();
	return pCommand->getCommandDescription(commandArgs);
}

const CommandInstance& CustomUndoRedoData::getCommandInstance() const
{
	return commandInstance_;
}
} // end namespace wgt
