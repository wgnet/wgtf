#include "reflected_collection_insert_command.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/generic/generic_object.hpp"

namespace wgt
{
ReflectedCollectionInsertCommand::ReflectedCollectionInsertCommand( IDefinitionManager & definitionManager )
	: definitionManager_( definitionManager )
{

}

ReflectedCollectionInsertCommand::~ReflectedCollectionInsertCommand()
{

}

const char * ReflectedCollectionInsertCommand::getId() const
{
	static const char * s_Id = getClassIdentifier<ReflectedCollectionInsertCommand>();
	return s_Id;
}

ObjectHandle ReflectedCollectionInsertCommand::execute(const ObjectHandle & arguments) const
{
	ReflectedCollectionInsertCommandParameters * commandArgs =
		arguments.getBase< ReflectedCollectionInsertCommandParameters >();

	auto objManager = definitionManager_.getObjectManager();
	assert( objManager != nullptr );
	auto object = objManager->getObject( commandArgs->id_ );
	if (!object.isValid())
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto property = object.getDefinition( definitionManager_ )->bindProperty( 
		commandArgs->path_.c_str(), object );
	if (property.isValid() == false)
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	commandArgs->inserted_ = property.insert( commandArgs->key_, commandArgs->value_ );
	if (!commandArgs->inserted_)
	{
		return CommandErrorCode::INVALID_VALUE;
	}

	return nullptr;
}

bool ReflectedCollectionInsertCommand::customUndo() const
{
	return true;
}

bool ReflectedCollectionInsertCommand::undo( const ObjectHandle & arguments ) const
{
	auto commandArgs = arguments.getBase< ReflectedCollectionInsertCommandParameters >();
	if (!commandArgs->inserted_)
	{
		return true;
	}

	auto objManager = definitionManager_.getObjectManager();
	assert( objManager != nullptr );
	auto object = objManager->getObject( commandArgs->id_ );
	if (!object.isValid())
	{
		return false;
	}

	auto property = object.getDefinition( definitionManager_ )->bindProperty( 
		commandArgs->path_.c_str(), object );
	if (property.isValid() == false)
	{
		return false;
	}

	return property.erase( commandArgs->key_ );
}

bool ReflectedCollectionInsertCommand::redo( const ObjectHandle & arguments ) const
{
	execute( arguments );
	return true;
}

ObjectHandle ReflectedCollectionInsertCommand::getCommandDescription(const ObjectHandle & arguments) const
{
	auto object = GenericObject::create( definitionManager_ );
	assert( object != nullptr );
	object->set("Name", "Insert");
	object->set("Type", "Unknown");
	return object;
}

CommandThreadAffinity ReflectedCollectionInsertCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}
} // end namespace wgt
