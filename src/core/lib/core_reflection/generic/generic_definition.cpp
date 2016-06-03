#include "generic_definition.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/utilities/definition_helpers.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/base_property_with_metadata.hpp"
#include "generic_property.hpp"

namespace wgt
{
const char * GenericDefinition::getName() const
{
	return name_.c_str();
}

//------------------------------------------------------------------------------
GenericDefinition::GenericDefinition( const char* name )
	:name_( name == nullptr
				? RefObjectId::generate().toString()
				: name )
{
}


//------------------------------------------------------------------------------
ObjectHandle GenericDefinition::create( const IClassDefinition & definition ) const
{
	auto pInst = std::unique_ptr< GenericObject >( new GenericObject() );
	pInst->setDefinition( const_cast< IClassDefinition * >( &definition ) );
	return ObjectHandle( std::move( pInst ), &definition );
}


//------------------------------------------------------------------------------
PropertyIteratorImplPtr GenericDefinition::getPropertyIterator() const
{
	return properties_.getIterator();
}


//------------------------------------------------------------------------------
IBasePropertyPtr GenericDefinition::addProperty( const char * name, const TypeId & typeId, MetaHandle metaData )
{
	IBasePropertyPtr property = std::make_shared< GenericProperty >( name, typeId );
	if (metaData != nullptr)
	{
		property = std::make_shared< BasePropertyWithMetaData >( property, metaData );
	}
	properties_.addProperty( property );
	return property;
}
} // end namespace wgt
