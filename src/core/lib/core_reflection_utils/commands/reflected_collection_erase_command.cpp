#include "reflected_collection_erase_command.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
ReflectedCollectionEraseCommand::ReflectedCollectionEraseCommand(IDefinitionManager& definitionManager)
    : definitionManager_(definitionManager)
{
}

ReflectedCollectionEraseCommand::~ReflectedCollectionEraseCommand()
{
}

const char* ReflectedCollectionEraseCommand::getId() const
{
	static const char* s_Id = getClassIdentifier<ReflectedCollectionEraseCommand>();
	return s_Id;
}

const char* ReflectedCollectionEraseCommand::getName() const
{
	static const char* s_name = "Remove an item from a reflected collection";
	return s_name;
}

Variant ReflectedCollectionEraseCommand::execute(const ObjectHandle& arguments) const
{
	auto commandArgs = arguments.getBase<ReflectedCollectionEraseCommandParameters>();

	auto objManager = definitionManager_.getObjectManager();
	TF_ASSERT(objManager != nullptr);
	auto object = objManager->getObject(commandArgs->id_);
	if (!object.isValid())
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto property = definitionManager_.getDefinition(object)->bindProperty(commandArgs->path_.c_str(), object);
	if (property.isValid() == false)
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	Collection collection;
	auto value = property.getValue();
	if (!value.tryCast(collection))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto it = collection.find(commandArgs->key_);
	if (it == collection.end())
	{
		return CommandErrorCode::INVALID_VALUE;
	}

	commandArgs->value_ = *it;

	commandArgs->erased_ = property.erase(commandArgs->key_);
	if (!commandArgs->erased_)
	{
		return CommandErrorCode::INVALID_VALUE;
	}

	return CommandErrorCode::COMMAND_NO_ERROR;
}

bool ReflectedCollectionEraseCommand::customUndo() const
{
	return true;
}

bool ReflectedCollectionEraseCommand::undo(const ObjectHandle& arguments) const
{
	ReflectedCollectionEraseCommandParameters* commandArgs =
	arguments.getBase<ReflectedCollectionEraseCommandParameters>();
	if (!commandArgs->erased_)
	{
		return true;
	}

	auto objManager = definitionManager_.getObjectManager();
	TF_ASSERT(objManager != nullptr);
	auto object = objManager->getObject(commandArgs->id_);
	if (!object.isValid())
	{
		return false;
	}

	auto property = definitionManager_.getDefinition(object)->bindProperty(commandArgs->path_.c_str(), object);
	if (property.isValid() == false)
	{
		return false;
	}

	return property.insert(commandArgs->key_, commandArgs->value_);
}

bool ReflectedCollectionEraseCommand::redo(const ObjectHandle& arguments) const
{
	execute(arguments);
	return true;
}

CommandDescription ReflectedCollectionEraseCommand::getCommandDescription(const ObjectHandle&) const
{
    auto object = GenericObject::create();
    object->set("Name", "Erase");
    object->set("Type", "Unknown");
    return std::move(object);
}

CommandThreadAffinity ReflectedCollectionEraseCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}

ManagedObjectPtr ReflectedCollectionEraseCommand::copyArguments(const ObjectHandle& arguments) const
{
	return Command::copyArguments<ReflectedCollectionEraseCommandParameters>(arguments);
}
} // end namespace wgt
