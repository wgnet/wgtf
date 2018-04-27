#include "base_generic_object.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/generic/generic_property.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/property_accessor.hpp"

namespace wgt
{
BaseGenericObject::BaseGenericObject() : definition_(nullptr)
{
}

BaseGenericObject::~BaseGenericObject()
{
}

IClassDefinition* BaseGenericObject::getDefinition() const
{
	return definition_;
}

void BaseGenericObject::setDefinition(IClassDefinition* definition)
{
	definition_ = definition;
}

bool BaseGenericObject::add(const char* name, const Variant& value, MetaData metadata, bool enableNotification)
{
	return !getDefinition()->bindProperty(name, getDerivedType()).isValid() &&
		addProperty(name, value, std::move(metadata), enableNotification);
}

bool BaseGenericObject::set(const char* name, const Variant& value, bool enableNotification /*= true*/)
{
	return this->setProperty(name, value, enableNotification);
}

Variant BaseGenericObject::invoke(const char* name, const ReflectedMethodParameters& parameters)
{
	return this->invokeProperty(name, parameters);
}

Variant BaseGenericObject::invokeProperty(const char* name, const ReflectedMethodParameters& parameters)
{
	const IClassDefinition& definition = *this->getDefinition();
	ObjectHandle provider = this->getDerivedType();
	PropertyAccessor accessor = definition.bindProperty(name, provider);
	if (!accessor.isValid())
	{
		TF_ASSERT(false && "Property could not be found");
		return Variant();
	}
	return accessor.invoke(parameters);
}

PropertyAccessor BaseGenericObject::findProperty(const char* name) const
{
	const IClassDefinition& definition = *this->getDefinition();
	ObjectHandle provider = this->getDerivedType();
	return definition.bindProperty(name, provider);
}

Variant BaseGenericObject::getProperty(const char* name) const
{
	auto accessor = findProperty(name);
	if (!accessor.isValid())
	{
		TF_ASSERT(false && "Property could not be found");
		return Variant();
	}
	return accessor.getValue();
}

bool BaseGenericObject::addProperty(const char* name, const Variant& value, MetaData metadata, bool enableNotification)
{
	const IClassDefinition& definition = *this->getDefinition();
	ObjectHandle provider = this->getDerivedType();

	auto definitionModifier = definition.getDetails().getDefinitionModifier();
	if (definitionModifier == nullptr)
	{
		return false;
	}

	Collection testCollection;
	bool isCollection = value.tryCast<Collection>(testCollection);

	if (!definitionModifier->addProperty(name, value.type()->typeId(), std::move(metadata), isCollection))
	{
		return false;
	}

	PropertyAccessor accessor = definition.bindProperty(name, provider);
	if (enableNotification)
	{
		return accessor.setValue(value);
	}
	else
	{
		return accessor.setValueWithoutNotification(value);
	}
}

bool BaseGenericObject::setProperty(const char* name, const Variant& value, bool enableNotification)
{
	// Get existing property
	const IClassDefinition& definition = *this->getDefinition();
	ObjectHandle provider = this->getDerivedType();
	PropertyAccessor accessor = definition.bindProperty(name, provider);

	// do nothing if property does not exist and the value is void
	if (!accessor.isValid() && value.isVoid())
	{
		return false;
	}

	// set value to the property if property exists and the value is not void
	if (accessor.isValid() && !value.isVoid())
	{
		if (enableNotification)
		{
			return accessor.setValue(value);
		}
		else
		{
			return accessor.setValueWithoutNotification(value);
		}
	}

	// Property does not exist
	// Add new property and set it
	auto definitionModifier = definition.getDetails().getDefinitionModifier();
	if (definitionModifier == nullptr)
	{
		return false;
	}

	if (accessor.isValid() && value.isVoid())
	{
		definitionModifier->removeProperty(name);
		return false;
	}

	return addProperty(name, value, nullptr, enableNotification);
}
} // end namespace wgt
