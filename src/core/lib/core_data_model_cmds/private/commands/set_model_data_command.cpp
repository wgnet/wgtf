#include "set_model_data_command.hpp"
#include "set_model_data_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
namespace SetModelDataCommand_Detail
{
bool isValid(const SetModelDataCommandArgument* pCommandArgs)
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
} // end namespace SetModelDataCommand_Detail

SetModelDataCommand::SetModelDataCommand()
{
}

SetModelDataCommand::~SetModelDataCommand()
{
}

bool SetModelDataCommand::customUndo() const /* override */
{
	return true;
}

bool SetModelDataCommand::canUndo(const ObjectHandle& arguments) const /* override */
{
	return this->validateArguments(arguments);
}

bool SetModelDataCommand::undo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<SetModelDataCommandArgument>();
	if (!SetModelDataCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& row = (pCommandArgs->row_);
	const auto& column = (pCommandArgs->column_);
	const auto& roleId = (pCommandArgs->roleId_);
	const auto& oldValue = (pCommandArgs->oldValue_);

	return model.setData(row, column, roleId, oldValue);
}

bool SetModelDataCommand::redo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<SetModelDataCommandArgument>();
	if (!SetModelDataCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& row = (pCommandArgs->row_);
	const auto& column = (pCommandArgs->column_);
	const auto& roleId = (pCommandArgs->roleId_);
	const auto& newValue = (pCommandArgs->newValue_);

	return model.setData(row, column, roleId, newValue);
}

CommandDescription SetModelDataCommand::getCommandDescription(const ObjectHandle& arguments) const /* override */
{
    auto object = GenericObject::create();

	if (!arguments.isValid())
	{
		object->set("Name", "Invalid");
		object->set("Type", "Data");
	}
    else
    {
        auto pCommandArgs = arguments.getBase<SetModelDataCommandArgument>();
        if (!SetModelDataCommand_Detail::isValid(pCommandArgs))
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

const char* SetModelDataCommand::getId() const /* override */
{
	static const char* s_Id = wgt::getClassIdentifier<SetModelDataCommand>();
	return s_Id;
}

bool SetModelDataCommand::validateArguments(const ObjectHandle& arguments) const /* override */
{
	const auto pCommandArgs = arguments.getBase<SetModelDataCommandArgument>();
	return SetModelDataCommand_Detail::isValid(pCommandArgs);
}

Variant SetModelDataCommand::execute(const ObjectHandle& arguments) const /* override */
{
	auto pCommandArgs = arguments.getBase<SetModelDataCommandArgument>();
	if (!SetModelDataCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& row = (pCommandArgs->row_);
	const auto& column = (pCommandArgs->column_);
	const auto& roleId = (pCommandArgs->roleId_);
	const auto& newValue = (pCommandArgs->newValue_);

	const auto result = model.setData(row, column, roleId, newValue);
	const auto errorCode = result ? CommandErrorCode::COMMAND_NO_ERROR : CommandErrorCode::FAILED;

	return errorCode;
}

CommandThreadAffinity SetModelDataCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}

ManagedObjectPtr SetModelDataCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<SetModelDataCommandArgument>(arguments);
}
} // end namespace wgt
