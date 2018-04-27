#include "set_item_data_command.hpp"

#include "set_item_data_command_arg.hpp"

#include "core_common/assert.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
namespace SetItemDataCommand_Detail
{
bool isValid(const SetItemDataCommandArgument* pCommandArgs)
{
	if (pCommandArgs == nullptr)
	{
		return false;
	}

	if (pCommandArgs->pModel_ == nullptr)
	{
		return false;
	}

	if (!pCommandArgs->index_.isValid())
	{
		return false;
	}

	return true;
}
} // end namespace SetItemDataCommand_Detail

SetItemDataCommand::~SetItemDataCommand()
{
}

bool SetItemDataCommand::customUndo() const /* override */
{
	return true;
}

bool SetItemDataCommand::canUndo(const ObjectHandle& arguments) const /* override */
{
	return this->validateArguments(arguments);
}

bool SetItemDataCommand::undo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<SetItemDataCommandArgument>();
	if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& index = (pCommandArgs->index_);
	const auto& roleId = (pCommandArgs->roleId_);
	const auto& oldValue = (pCommandArgs->oldValue_);

	auto pItem = model.item(index);
	TF_ASSERT(pItem != nullptr);
	return pItem->setData(index.row_, index.column_, roleId, oldValue);
}

bool SetItemDataCommand::redo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<SetItemDataCommandArgument>();
	if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& index = (pCommandArgs->index_);
	const auto& roleId = (pCommandArgs->roleId_);
	const auto& newValue = (pCommandArgs->newValue_);

	auto pItem = model.item(index);
	TF_ASSERT(pItem != nullptr);
	return pItem->setData(index.row_, index.column_, roleId, newValue);
}

CommandDescription SetItemDataCommand::getCommandDescription(const ObjectHandle& arguments) const /* override */
{
    auto object = GenericObject::create();

	if (!arguments.isValid())
	{
		object->set("Name", "Invalid");
		object->set("Type", "Data");
	}
    else
    {
        auto pCommandArgs = arguments.getBase<SetItemDataCommandArgument>();
        if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
        {
            object->set("Name", "Invalid");
            object->set("Type", "Data");
        }
        else
        {
            object->set("Name", "Data");
            object->set("Type", "Data");

            const auto& oldValue = (pCommandArgs->oldValue_);
            const auto& newValue = (pCommandArgs->newValue_);
            object->set("PreValue", oldValue);
            object->set("PostValue", newValue);
        }
    }

    return std::move(object);
}

const char* SetItemDataCommand::getId() const /* override */
{
	static const char* s_Id = wgt::getClassIdentifier<SetItemDataCommand>();
	return s_Id;
}

bool SetItemDataCommand::validateArguments(const ObjectHandle& arguments) const /* override */
{
	const auto pCommandArgs = arguments.getBase<SetItemDataCommandArgument>();
	return SetItemDataCommand_Detail::isValid(pCommandArgs);
}

Variant SetItemDataCommand::execute(const ObjectHandle& arguments) const /* override */
{
	auto pCommandArgs = arguments.getBase<SetItemDataCommandArgument>();
	if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& index = (pCommandArgs->index_);
	const auto& roleId = (pCommandArgs->roleId_);
	const auto& newValue = (pCommandArgs->newValue_);

	auto pItem = model.item(index);
	TF_ASSERT(pItem != nullptr);
	const auto result = pItem->setData(index.row_, index.column_, roleId, newValue);
	const auto errorCode = result ? CommandErrorCode::COMMAND_NO_ERROR : CommandErrorCode::FAILED;

	return errorCode;
}

CommandThreadAffinity SetItemDataCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}

ManagedObjectPtr SetItemDataCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<SetItemDataCommandArgument>(arguments);
}
} // end namespace wgt
