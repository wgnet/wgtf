#include "undo_redo_command.hpp"
#include "core_reflection/object_handle.hpp"
#include "command_manager.hpp"
#include "command_instance.hpp"
#include "command_manager.hpp"

namespace wgt
{
//--------------------------------------------------------------------------
UndoRedoCommand::UndoRedoCommand(CommandManager* pCommandManager) : pCommandManager_(pCommandManager)
{
}

//--------------------------------------------------------------------------
const char* UndoRedoCommand::getId() const
{
	static const char* s_id = typeid(UndoRedoCommand).name();
	return s_id;
}

//--------------------------------------------------------------------------
ObjectHandle UndoRedoCommand::execute(const ObjectHandle& arguments) const
{
	assert(pCommandManager_ != nullptr);
	auto pValue = arguments.getBase<int>();
	assert(pValue != nullptr);
	if (pValue == nullptr)
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}
	if (!pCommandManager_->undoRedo(*pValue))
	{
		return CommandErrorCode::INVALID_VALUE;
	}

	return CommandErrorCode::COMMAND_NO_ERROR;
}

//--------------------------------------------------------------------------
CommandThreadAffinity UndoRedoCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}
} // end namespace wgt
