#include "generic_property.hpp"
#include "generic_object.hpp"
#include "core_variant/variant.hpp"


namespace wgt
{
const char * GenericProperty::getName() const
{
	return propertyName_.c_str();
}


bool GenericProperty::isValue() const /* override */
{
	return true;
}


bool GenericProperty::set( const ObjectHandle & pBase, const Variant & value, const IDefinitionManager & definitionManager ) const 
{
	assert( !this->readOnly() );
	auto pObject = reflectedCast< GenericObject >( pBase.data(), pBase.type(), definitionManager );
	pObject->properties_[this] = value;
	return true;
}

Variant GenericProperty::get( const ObjectHandle & pBase, const IDefinitionManager & definitionManager ) const 
{
	assert( this->isValue() );
	auto pObject = reflectedCast< GenericObject >( pBase.data(), pBase.type(), definitionManager );
	return pObject->properties_[this];
}
} // end namespace wgt
