#include "insert_item_command.hpp"
#include "insert_item_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
namespace InsertItemCommand_Detail
{
bool isValid(const InsertItemCommandArgument* pCommandArgs)
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

} // end namespace InsertItemCommand_Detail

InsertItemCommand::~InsertItemCommand()
{
}

bool InsertItemCommand::customUndo() const /* override */
{
	return true;
}

bool InsertItemCommand::canUndo(const ObjectHandle& arguments) const /* override */
{
	return this->validateArguments(arguments);
}

bool InsertItemCommand::undo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<InsertItemCommandArgument>();
	if (!InsertItemCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);

	return model.removeItem(key);
}

bool InsertItemCommand::redo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<InsertItemCommandArgument>();
	if (!InsertItemCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);
	const auto& value = (pCommandArgs->value_);

	return model.insertItem(key, value);
}

CommandDescription InsertItemCommand::getCommandDescription(const ObjectHandle& arguments) const /* override */
{
    auto object = GenericObject::create();

	if (!arguments.isValid())
	{
		object->set("Name", "Invalid");
		object->set("Type", "Insert");
	}
    else
    {
        auto pCommandArgs = arguments.getBase<InsertItemCommandArgument>();
        if (!InsertItemCommand_Detail::isValid(pCommandArgs))
        {
            object->set("Name", "Invalid");
            object->set("Type", "Insert");
        }
        else
        {
            object->set("Id", pCommandArgs->key_);
            object->set("Name", "Insert");
            object->set("Type", "Insert");
            object->set("PostValue", pCommandArgs->value_);
        }
    }

    return std::move(object);
}

const char* InsertItemCommand::getId() const /* override */
{
	static const char* s_Id = wgt::getClassIdentifier<InsertItemCommand>();
	return s_Id;
}

bool InsertItemCommand::validateArguments(const ObjectHandle& arguments) const /* override */
{
	const auto pCommandArgs = arguments.getBase<InsertItemCommandArgument>();
	return InsertItemCommand_Detail::isValid(pCommandArgs);
}

Variant InsertItemCommand::execute(const ObjectHandle& arguments) const /* override */
{
	auto pCommandArgs = arguments.getBase<InsertItemCommandArgument>();
	if (!InsertItemCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);
	const auto& value = (pCommandArgs->value_);

	const auto result = model.insertItem(key, value);

	const auto errorCode = result ? CommandErrorCode::COMMAND_NO_ERROR : CommandErrorCode::FAILED;
	return errorCode;
}

CommandThreadAffinity InsertItemCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}

ManagedObjectPtr InsertItemCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<InsertItemCommandArgument>(arguments);
}
} // end namespace wgt
