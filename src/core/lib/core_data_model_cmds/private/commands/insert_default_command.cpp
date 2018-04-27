#include "insert_default_command.hpp"
#include "insert_default_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
namespace InsertDefaultCommand_Detail
{
bool isValid(const InsertDefaultCommandArgument* pCommandArgs)
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

} // end namespace InsertDefaultCommand_Detail

InsertDefaultCommand::~InsertDefaultCommand()
{
}

bool InsertDefaultCommand::customUndo() const /* override */
{
	return true;
}

bool InsertDefaultCommand::canUndo(const ObjectHandle& arguments) const /* override */
{
	return this->validateArguments(arguments);
}

bool InsertDefaultCommand::undo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<InsertDefaultCommandArgument>();
	if (!InsertDefaultCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);

	return model.removeItem(key);
}

bool InsertDefaultCommand::redo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<InsertDefaultCommandArgument>();
	if (!InsertDefaultCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);

	return model.insertItem(key);
}

CommandDescription InsertDefaultCommand::getCommandDescription(const ObjectHandle& arguments) const /* override */
{
    auto object = GenericObject::create();

	if (!arguments.isValid())
	{
        object->set("Name", "Invalid");
		object->set("Type", "Insert");
	}
    else
    {
        auto pCommandArgs = arguments.getBase<InsertDefaultCommandArgument>();
        if (!InsertDefaultCommand_Detail::isValid(pCommandArgs))
        {
            object->set("Name", "Invalid");
            object->set("Type", "Insert");
        }
        else
        {
            object->set("Id", pCommandArgs->key_);
            object->set("Name", "Insert");
            object->set("Type", "Insert");
        }
    }

    return std::move(object);
}

const char* InsertDefaultCommand::getId() const /* override */
{
	static const char* s_Id = wgt::getClassIdentifier<InsertDefaultCommand>();
	return s_Id;
}

bool InsertDefaultCommand::validateArguments(const ObjectHandle& arguments) const /* override */
{
	const auto pCommandArgs = arguments.getBase<InsertDefaultCommandArgument>();
	return InsertDefaultCommand_Detail::isValid(pCommandArgs);
}

Variant InsertDefaultCommand::execute(const ObjectHandle& arguments) const /* override */
{
	auto pCommandArgs = arguments.getBase<InsertDefaultCommandArgument>();
	if (!InsertDefaultCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto& model = (*static_cast<CollectionModel*>(pCommandArgs->pModel_));
	const auto& key = (pCommandArgs->key_);

	const auto result = model.insertItem(key);

	const auto errorCode = result ? CommandErrorCode::COMMAND_NO_ERROR : CommandErrorCode::FAILED;
	return errorCode;
}

CommandThreadAffinity InsertDefaultCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}

ManagedObjectPtr InsertDefaultCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<InsertDefaultCommandArgument>(arguments);
}
} // end namespace wgt
