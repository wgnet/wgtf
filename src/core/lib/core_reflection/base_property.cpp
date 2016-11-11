#include "base_property.hpp"
#include "metadata/meta_base.hpp"
#include <stddef.h>

namespace wgt
{
//==============================================================================
BaseProperty::BaseProperty(const char* name, const TypeId& type)
    : name_(name), type_(type), hash_(HashUtilities::compute(name_))
{
}

//==============================================================================
const TypeId& BaseProperty::getType() const
{
	return type_;
}

//==============================================================================
const char* BaseProperty::getName() const
{
	return name_;
}

//==============================================================================
uint64_t BaseProperty::getNameHash() const
{
	return hash_;
}

//==============================================================================
MetaHandle BaseProperty::getMetaData() const
{
	return nullptr;
}

bool BaseProperty::readOnly() const
{
	return !this->isValue();
}

bool BaseProperty::isMethod() const /* override */
{
	return false;
}

bool BaseProperty::isValue() const /* override */
{
	return false;
}

bool BaseProperty::set(const ObjectHandle& handle, const Variant& value,
                       const IDefinitionManager& definitionManager) const /* override */
{
	assert(!this->readOnly());
	return false;
}

Variant BaseProperty::get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const /* override */
{
	assert(!this->isValue());
	return 0;
}

Variant BaseProperty::invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
                             const ReflectedMethodParameters& parameters) /* override */
{
	assert(isMethod());
	return 0;
}

size_t BaseProperty::parameterCount() const /* override */
{
	assert(isMethod());
	return 0;
}

//==============================================================================
void BaseProperty::setType(const TypeId& type)
{
	type_ = type;
}

//==============================================================================
void BaseProperty::setName(const char* name)
{
	name_ = name;
	hash_ = HashUtilities::compute(name_);
}
} // end namespace wgt
