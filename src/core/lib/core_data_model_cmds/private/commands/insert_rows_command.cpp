#include "insert_rows_command.hpp"
#include "insert_rows_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
namespace InsertRowsCommand_Detail
{
bool isValid(const InsertRowsCommandArgument* pCommandArgs)
{
	if (pCommandArgs == nullptr)
	{
		return false;
	}

	if (pCommandArgs->pModel_ == nullptr)
	{
		return false;
	}

	if (pCommandArgs->startPos_ < 0)
	{
		return false;
	}

	if (pCommandArgs->count_ < 1)
	{
		return false;
	}

	return true;
}

} // end namespace InsertRowsCommand_Detail

InsertRowsCommand::InsertRowsCommand(IComponentContext& context) : definitionManager_(context)
{
}

bool InsertRowsCommand::customUndo() const /* override */
{
	return true;
}

bool InsertRowsCommand::canUndo(const ObjectHandle& arguments) const /* override */
{
	return this->validateArguments(arguments);
}

bool InsertRowsCommand::undo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<InsertRowsCommandArgument>();
	if (!InsertRowsCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& startPos = (pCommandArgs->startPos_);
	const auto& type = (pCommandArgs->type_);
	const auto& count = (pCommandArgs->count_);
	const auto& pParent = (pCommandArgs->pParent_);

	if (type == InsertRowsCommandArgument::Type::ROW)
	{
		return model.removeRows(startPos, count, pParent);
	}
	return model.removeColumns(startPos, count, pParent);
}

bool InsertRowsCommand::redo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<InsertRowsCommandArgument>();
	if (!InsertRowsCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& startPos = (pCommandArgs->startPos_);
	const auto& type = (pCommandArgs->type_);
	const auto& count = (pCommandArgs->count_);
	const auto& pParent = (pCommandArgs->pParent_);

	if (type == InsertRowsCommandArgument::Type::ROW)
	{
		return model.insertRows(startPos, count, pParent);
	}
	return model.insertColumns(startPos, count, pParent);
}

ObjectHandle InsertRowsCommand::getCommandDescription(const ObjectHandle& arguments) const /* override */
{
	auto handle = GenericObject::create(*definitionManager_);
	assert(handle.get() != nullptr);
	auto& genericObject = (*handle);

	if (!arguments.isValid())
	{
		genericObject.set("Name", "Invalid");
		genericObject.set("Type", "Insert/Remove");
		return ObjectHandle(std::move(handle));
	}

	auto pCommandArgs = arguments.getBase<InsertRowsCommandArgument>();
	if (!InsertRowsCommand_Detail::isValid(pCommandArgs))
	{
		genericObject.set("Name", "Invalid");
		genericObject.set("Type", "Insert/Remove");
		return ObjectHandle(std::move(handle));
	}

	genericObject.set("Name", "Insert");
	genericObject.set("Type", "Insert");

	return ObjectHandle(std::move(handle));
}

const char* InsertRowsCommand::getId() const /* override */
{
	static const char* s_Id = wgt::getClassIdentifier<InsertRowsCommand>();
	return s_Id;
}

bool InsertRowsCommand::validateArguments(const ObjectHandle& arguments) const /* override */
{
	const auto pCommandArgs = arguments.getBase<InsertRowsCommandArgument>();
	return InsertRowsCommand_Detail::isValid(pCommandArgs);
}

ObjectHandle InsertRowsCommand::execute(const ObjectHandle& arguments) const /* override */
{
	auto pCommandArgs = arguments.getBase<InsertRowsCommandArgument>();
	if (!InsertRowsCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& startPos = (pCommandArgs->startPos_);
	const auto& type = (pCommandArgs->type_);
	const auto& count = (pCommandArgs->count_);
	const auto& pParent = (pCommandArgs->pParent_);

	auto result = false;
	if (type == InsertRowsCommandArgument::Type::ROW)
	{
		result = model.insertRows(startPos, count, pParent);
	}
	else
	{
		result = model.insertColumns(startPos, count, pParent);
	}

	const auto errorCode = result ? CommandErrorCode::COMMAND_NO_ERROR : CommandErrorCode::FAILED;
	return errorCode;
}

CommandThreadAffinity InsertRowsCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}

} // end namespace wgt
