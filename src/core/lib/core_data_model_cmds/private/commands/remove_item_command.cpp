#include "remove_item_command.hpp"
#include "remove_item_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
namespace RemoveItemCommand_Detail
{
bool isValid(const RemoveItemCommandArgument* pCommandArgs)
{
	if (pCommandArgs == nullptr)
	{
		return false;
	}

	if (pCommandArgs->pModel_ == nullptr)
	{
		return false;
	}

	return true;
}

} // end namespace RemoveItemCommand_Detail

RemoveItemCommand::RemoveItemCommand()
{
}

RemoveItemCommand::~RemoveItemCommand()
{
}

bool RemoveItemCommand::customUndo() const /* override */
{
	return true;
}

bool RemoveItemCommand::canUndo(const ObjectHandle& arguments) const /* override */
{
	return this->validateArguments(arguments);
}

bool RemoveItemCommand::undo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<RemoveItemCommandArgument>();
	if (!RemoveItemCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);
	const auto& value = (pCommandArgs->value_);

	return model.insertItem(key, value);
}

bool RemoveItemCommand::redo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<RemoveItemCommandArgument>();
	if (!RemoveItemCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);

	return model.removeItem(key);
}

CommandDescription RemoveItemCommand::getCommandDescription(const ObjectHandle& arguments) const /* override */
{
    auto object = GenericObject::create();

	if (!arguments.isValid())
	{
		object->set("Name", "Invalid");
		object->set("Type", "Remove");
	}
    else
    {
        auto pCommandArgs = arguments.getBase<RemoveItemCommandArgument>();
        if (!RemoveItemCommand_Detail::isValid(pCommandArgs))
        {
            object->set("Name", "Invalid");
            object->set("Type", "Remove");
        }
        else
        {
            object->set("Id", pCommandArgs->key_);
            object->set("Name", "Remove");
            object->set("Type", "Remove");
            object->set("PreValue", pCommandArgs->value_);
        }
    }

    return std::move(object);
}

const char* RemoveItemCommand::getId() const /* override */
{
	static const char* s_Id = wgt::getClassIdentifier<RemoveItemCommand>();
	return s_Id;
}

bool RemoveItemCommand::validateArguments(const ObjectHandle& arguments) const /* override */
{
	const auto pCommandArgs = arguments.getBase<RemoveItemCommandArgument>();
	return RemoveItemCommand_Detail::isValid(pCommandArgs);
}

Variant RemoveItemCommand::execute(const ObjectHandle& arguments) const /* override */
{
	auto pCommandArgs = arguments.getBase<RemoveItemCommandArgument>();
	if (!RemoveItemCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);
	const auto pItem = model.find(key);
	pCommandArgs->value_ = pItem->getData(0 /* row */, 0 /* column */, ValueRole::roleId_);

	const auto result = model.removeItem(key);

	const auto errorCode = result ? CommandErrorCode::COMMAND_NO_ERROR : CommandErrorCode::FAILED;
	return errorCode;
}

CommandThreadAffinity RemoveItemCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}

ManagedObjectPtr RemoveItemCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<RemoveItemCommandArgument>(arguments);
}
} // end namespace wgt
