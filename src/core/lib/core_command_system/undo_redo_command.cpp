#include "undo_redo_command.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/object_handle.hpp"
#include "command_manager.hpp"
#include "command_instance.hpp"
#include "command_manager.hpp"
#include "core_common/assert.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
//--------------------------------------------------------------------------
UndoRedoCommandArgument::UndoRedoCommandArgument(int index) : index_(index)
{
}

//--------------------------------------------------------------------------
int UndoRedoCommandArgument::getIndex() const
{
	return index_;
}

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
Variant UndoRedoCommand::execute(const ObjectHandle& arguments) const
{
	TF_ASSERT(pCommandManager_ != nullptr);
	auto pArg = arguments.getBase<UndoRedoCommandArgument>();
	TF_ASSERT(pArg != nullptr);
	if (pArg == nullptr)
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	int index = pArg->getIndex();
	if (!pCommandManager_->undoRedo(index))
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

ManagedObjectPtr UndoRedoCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<UndoRedoCommandArgument>(arguments);
}
} // end namespace wgt
