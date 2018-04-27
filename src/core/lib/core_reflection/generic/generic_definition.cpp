#include "generic_definition.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/utilities/definition_helpers.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/base_property_with_metadata.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_object/i_object_manager.hpp"
#include "generic_property.hpp"

namespace wgt
{
const char* GenericDefinition::getName() const
{
	return name_.c_str();
}

//------------------------------------------------------------------------------
GenericDefinition::GenericDefinition(const char* name)
    : name_(name == nullptr ? RefObjectId::generate().toString() : name)
{
}

//------------------------------------------------------------------------------
ObjectHandleStoragePtr GenericDefinition::createObjectStorage(const IClassDefinition& definition) const
{
    auto objectManager = definition.getDefinitionManager()->getObjectManager();
    std::unique_ptr<GenericObject> object(new GenericObject());
    object->setDefinition(const_cast<IClassDefinition*>(&definition));
    return objectManager->createObjectStorage(std::move(object));
}

//------------------------------------------------------------------------------
ManagedObjectPtr GenericDefinition::createManaged(const IClassDefinition& definition, RefObjectId id) const
{
	auto objectManager = definition.getDefinitionManager()->getObjectManager();
	auto storage = createObjectStorage(definition);
	return objectManager->createGenericObject(storage, id);
}

//------------------------------------------------------------------------------
PropertyIteratorImplPtr GenericDefinition::getPropertyIterator() const
{
	return properties_.getIterator();
}

//------------------------------------------------------------------------------
IBasePropertyPtr GenericDefinition::addProperty(const char* name, const TypeId& typeId, MetaData metaData,
                                                bool isCollection)
{
	IBasePropertyPtr property = std::make_shared<GenericProperty>(name, typeId, isCollection);
	if (metaData != nullptr)
	{
        property = std::make_shared<BasePropertyWithMetaData>(property, std::move(metaData));
	}
	prePropertyAdded(name);
	properties_.addProperty(property);
	postPropertyAdded(name);
	return property;
}

//------------------------------------------------------------------------------
void GenericDefinition::removeProperty(const char* name)
{
	prePropertyRemoved(name);
	properties_.removeProperty(name);
	postPropertyRemoved(name);
}
} // end namespace wgt
