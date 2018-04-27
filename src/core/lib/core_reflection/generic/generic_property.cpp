#include "generic_property.hpp"

#include "generic_object.hpp"
#include "core_common/assert.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{

//==============================================================================
GenericProperty::GenericProperty(
	const char* name, const TypeId& typeName, bool isCollection)
	: BaseProperty(name, typeName)
	, propertyName_(name)
	, typeName_(typeName.getName())
	, isCollection_(isCollection)
{
	setType(typeName_.c_str());
	setName(propertyName_.c_str());
}


//------------------------------------------------------------------------------
bool GenericProperty::isValue() const /* override */
{
	return true;
}


//------------------------------------------------------------------------------
bool GenericProperty::set(const ObjectHandle& pBase, const Variant& value,
                          const IDefinitionManager& definitionManager) const
{
	TF_ASSERT(!this->readOnly(pBase));
	auto pObject = 
		reflectedCast<GenericObject>(
			pBase.data(), pBase.type(), definitionManager);
	pObject->properties_[this] = value;
	return true;
}


//------------------------------------------------------------------------------
Variant GenericProperty::get(
	const ObjectHandle& pBase, const IDefinitionManager& definitionManager) const
{
	TF_ASSERT(this->isValue());
	auto pObject = reflectedCast<GenericObject>(pBase.data(), pBase.type(), definitionManager);
	return pObject->properties_[this];
}


//------------------------------------------------------------------------------
bool GenericProperty::isCollection() const 
{
	return isCollection_;
}


//------------------------------------------------------------------------------
bool GenericProperty::isByReference() const 
{
	return getType().isPointer();
}

} // end namespace wgt
