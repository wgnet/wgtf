#include "batch_command.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "command_manager.hpp"

namespace wgt
{

//--------------------------------------------------------------------------
BatchCommand::BatchCommand(CommandManager* pCommandManager) : pCommandManager_(pCommandManager)
{
}

//--------------------------------------------------------------------------
const char* BatchCommand::getId() const
{
	static const char* s_id = typeid(BatchCommand).name();
	return s_id;
}

//--------------------------------------------------------------------------
Variant BatchCommand::execute(const ObjectHandle& arguments) const
{
	TF_ASSERT(pCommandManager_ != nullptr);
	auto params = arguments.getBase<BatchCommandParameters>();
	TF_ASSERT(params != nullptr);
	switch (params->stage_)
	{
	case BatchCommandStage::Begin:
		pCommandManager_->notifyBeginMultiCommand();
		break;
	case BatchCommandStage::End:
		break;
	case BatchCommandStage::Abort:
		return CommandErrorCode::ABORTED;
	default:
		TF_ASSERT(false);
		break;
	}
	return CommandErrorCode::BATCH_NO_ERROR;
}

//--------------------------------------------------------------------------
CommandThreadAffinity BatchCommand::threadAffinity() const
{
	return CommandThreadAffinity::ANY_THREAD;
}

ManagedObjectPtr BatchCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<BatchCommandParameters>(arguments);
}

CommandDescription BatchCommand::getCommandDescription(const ObjectHandle& arguments) const
{
	auto params = arguments.getBase<BatchCommandParameters>();
	if (!params || params->description_.empty())
	{
		return nullptr;
	}

	auto object = GenericObject::create();
	object->set("Name", params->description_);
	object->set("Type", "Custom");
	return std::move(object);

}
} // end namespace wgt
