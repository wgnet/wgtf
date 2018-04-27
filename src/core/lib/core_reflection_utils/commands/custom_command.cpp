#include "custom_command.hpp"

#include "core_variant/variant.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/base_property_with_metadata.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{

const char* CustomCommand::getId() const
{
	static const char* s_Id = getClassIdentifier<CustomCommand>();
	return s_Id;
}

const char* CustomCommand::getName() const
{
	static const char* s_name = "Custom Command";
	return s_name;
}

bool CustomCommand::validateArguments(const ObjectHandle& arguments) const
{
	auto commandParameters = arguments.getBase<CustomCommandParameters>();
	if (commandParameters == nullptr)
	{
		return false;
	}

	return commandParameters->execute_ && commandParameters->undo_;
}

Variant CustomCommand::execute(const ObjectHandle& arguments) const
{
	auto commandParameters = arguments.getBase<CustomCommandParameters>();
	if (!commandParameters->execute_ || !commandParameters->undo_)
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	return commandParameters->execute_() ? CommandErrorCode::COMMAND_NO_ERROR : CommandErrorCode::FAILED;
}

CommandThreadAffinity CustomCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}

bool CustomCommand::undo(const ObjectHandle& arguments) const
{
	auto commandParameters = arguments.getBase<CustomCommandParameters>();
	return commandParameters->undo_();
}

bool CustomCommand::redo(const ObjectHandle& arguments) const
{
	auto commandParameters = arguments.getBase<CustomCommandParameters>();
	return commandParameters->execute_();
}

CommandDescription CustomCommand::getCommandDescription(const ObjectHandle& arguments) const
{
	auto commandParameters = arguments.getBase<CustomCommandParameters>();
	auto object = GenericObject::create();
    object->set("Name", commandParameters->description_);
    object->set("Type", "Custom");
    return std::move(object);
}

ManagedObjectPtr CustomCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<CustomCommandParameters>(arguments);
}
} // end namespace wgt
