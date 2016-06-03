#include "generic_object.hpp"

#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/generic/generic_property.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_variant/variant.hpp"


namespace wgt
{
GenericObjectPtr GenericObject::create(
	IDefinitionManager & definitionManager, 
	const RefObjectId & id, 
	const char* classDefinitionName )
{
	auto defDetails = definitionManager.createGenericDefinition( classDefinitionName );
	auto definition = definitionManager.registerDefinition( std::move(defDetails) );
	return safeCast< GenericObject >( definition->createManagedObject( id ) );
}


ObjectHandle GenericObject::getDerivedType() const
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	return ObjectHandleT< GenericObject >( this );
}


ObjectHandle GenericObject::getDerivedType()
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	return ObjectHandleT< GenericObject >( this );
}
} // end namespace wgt
