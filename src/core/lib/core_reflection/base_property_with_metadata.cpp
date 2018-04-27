#include "base_property_with_metadata.hpp"

#include "core_common/assert.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
BasePropertyWithMetaData::BasePropertyWithMetaData(const IBasePropertyPtr& property, MetaData metaData)
    : property_(property), metaData_(std::move(metaData))
{
	TF_ASSERT(property_ != nullptr);
}

BasePropertyWithMetaData::~BasePropertyWithMetaData()
{
}

std::shared_ptr< IPropertyPath> BasePropertyWithMetaData::generatePropertyName(
	const std::shared_ptr< const IPropertyPath > & parent) const
{
	return property_->generatePropertyName( parent );
}

const TypeId& BasePropertyWithMetaData::getType() const
{
	return property_->getType();
}

const char* BasePropertyWithMetaData::getName() const
{
	return property_->getName();
}

uint64_t BasePropertyWithMetaData::getNameHash() const
{
	return property_->getNameHash();
}

const MetaData & BasePropertyWithMetaData::getMetaData() const
{
	// TODO NGT-1582
	// return property_->getMetaData() + metaData_;
	return metaData_;
}

bool BasePropertyWithMetaData::readOnly(const ObjectHandle& handle) const
{
	return property_->readOnly(handle);
}

bool BasePropertyWithMetaData::isMethod() const
{
	return property_->isMethod();
}

bool BasePropertyWithMetaData::isValue() const
{
	return property_->isValue();
}

bool BasePropertyWithMetaData::isCollection() const
{
	return property_->isCollection();
}

bool BasePropertyWithMetaData::isByReference() const
{
	return property_->isByReference();
}

bool BasePropertyWithMetaData::set(const ObjectHandle& handle, const Variant& value,
                                   const IDefinitionManager& definitionManager) const
{
	return property_->set(handle, value, definitionManager);
}

Variant BasePropertyWithMetaData::get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const
{
	return property_->get(handle, definitionManager);
}

Variant BasePropertyWithMetaData::invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
                                         const ReflectedMethodParameters& parameters)
{
	return property_->invoke(object, definitionManager, parameters);
}

size_t BasePropertyWithMetaData::parameterCount() const
{
	return property_->parameterCount();
}

IBasePropertyPtr BasePropertyWithMetaData::baseProperty() const
{
	return property_;
}
} // end namespace wgt
