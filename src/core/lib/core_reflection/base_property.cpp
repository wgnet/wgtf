#include "base_property.hpp"

#include "core_common/assert.hpp"
#include "wg_types/hash_utilities.hpp"
#include "metadata/meta_base.hpp"
#include "core_logging/logging.hpp"
#include "private/property_path.hpp"
#include <stddef.h>

namespace wgt
{
BaseProperty::BaseProperty(const char* name, const TypeId& type)
    : name_(name), type_(type), hash_(HashUtilities::compute(name_))
{
}

std::shared_ptr< IPropertyPath> BaseProperty::generatePropertyName(
	const std::shared_ptr< const IPropertyPath > & parent) const
{
	if (isCollection())
	{
		return std::make_shared< CollectionPath >(parent, name_);
	}
	return std::make_shared< PropertyPath >( parent, name_ );
}

const TypeId& BaseProperty::getType() const
{
	return type_;
}

const char* BaseProperty::getName() const
{
	return name_;
}

uint64_t BaseProperty::getNameHash() const
{
	return hash_;
}

const MetaData & BaseProperty::getMetaData() const
{
	static MetaData s_Data;
	return s_Data;
}

bool BaseProperty::readOnly(const ObjectHandle&) const
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

bool BaseProperty::isCollection() const /* override */
{
	NGT_ERROR_MSG("Inherited class must provide correct implementation.");
	return false;
}

bool BaseProperty::set(const ObjectHandle& handle, const Variant& value,
                       const IDefinitionManager& definitionManager) const /* override */
{
	TF_ASSERT(!this->readOnly(handle));
	return false;
}

Variant BaseProperty::get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const /* override */
{
	TF_ASSERT(!this->isValue());
	return 0;
}

Variant BaseProperty::invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
                             const ReflectedMethodParameters& parameters) /* override */
{
	TF_ASSERT(isMethod());
	return 0;
}

size_t BaseProperty::parameterCount() const /* override */
{
	TF_ASSERT(isMethod());
	return 0;
}

void BaseProperty::setType(const TypeId& type)
{
	type_ = type;
}

void BaseProperty::setName(const char* name)
{
	name_ = name;
	hash_ = HashUtilities::compute(name_);
}
} // end namespace wgt
