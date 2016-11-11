#include "reflected_collection_erase_command.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/generic/generic_object.hpp"

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

ObjectHandle ReflectedCollectionEraseCommand::execute(const ObjectHandle& arguments) const
{
	auto commandArgs = arguments.getBase<ReflectedCollectionEraseCommandParameters>();

	auto objManager = definitionManager_.getObjectManager();
	assert(objManager != nullptr);
	auto object = objManager->getObject(commandArgs->id_);
	if (!object.isValid())
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto property = object.getDefinition(definitionManager_)->bindProperty(commandArgs->path_.c_str(), object);
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

	return nullptr;
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
	assert(objManager != nullptr);
	auto object = objManager->getObject(commandArgs->id_);
	if (!object.isValid())
	{
		return false;
	}

	auto property = object.getDefinition(definitionManager_)->bindProperty(commandArgs->path_.c_str(), object);
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

ObjectHandle ReflectedCollectionEraseCommand::getCommandDescription(const ObjectHandle& arguments) const
{
	auto object = GenericObject::create(definitionManager_);
	assert(object != nullptr);
	object->set("Name", "Erase");
	object->set("Type", "Unknown");
	return object;
}

CommandThreadAffinity ReflectedCollectionEraseCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}
} // end namespace wgt
