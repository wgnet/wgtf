#include "reflectedhandler.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_object/object_handle_reflection_utils.hpp"

namespace wgt
{
ReflectedHandler::ReflectedHandler(const IDefinitionManager& definitionManager)
    : SerializationHandler("__Reflected"), definitionManager_(definitionManager)
{
}

bool ReflectedHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	// Set node handler and type
	node->setHandlerName(handlerName_);
	if (writeType)
	{
		node->setType(reflectedName_, strlen(reflectedName_));
	}

	// Set internal variant system type
	if (writeType)
	{
		auto reflectedTypeName = this->getReservedNames().reflectedInternalType;
		auto typeNode = node->createEmptyChild(reflectedTypeName, strlen(reflectedTypeName));
		typeNode->setType(v.type()->typeId().getName(), strlen(v.type()->typeId().getName()));
	}

	// Get definition
	auto definition = definitionManager_.getDefinition(v.type()->typeId().getName());
	assert(definition != nullptr);
	if (definition == nullptr)
	{
		return false;
	}

	ObjectHandle vHandle(v, definition);

	// Get root definition
	auto rootHandle = ObjectHandleStorageReflected::root(vHandle, definitionManager_);
	auto rootDefinition = definitionManager_.getDefinition(rootHandle);

	// Write all the properties
	for (IBasePropertyPtr prop : rootDefinition->allProperties())
	{
		if (prop->isMethod())
		{
			continue;
		}

		// Create node for property
		const char* propertyName = prop->getName();
		// Ensure name isn't reserved
		if (strcmp(propertyName, this->getReservedNames().reflectedInternalType) == 0)
		{
			return false;
		}

		Variant propertyValue = prop->get(vHandle, definitionManager_);
		// We'll need to create a primitive type handler to serialize primitives correctly.
		auto propertyNode = node->createChildVariant(propertyName, strlen(propertyName), propertyValue);
	}
	return true;
}

bool ReflectedHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Check that we've been given a valid Variant
	if (v.isVoid() || v.type() == nullptr)
	{
		return false;
	}

	// Get the Variant system supplied type
	auto reflectedTypeName = this->getReservedNames().reflectedInternalType;
	std::string internalTypeName;
	// If we were not supplied with a type name, use the saved to the node
	if (typeName == nullptr)
	{
		internalTypeName = node->getChildNode(reflectedTypeName, strlen(reflectedTypeName))->getType().c_str();
	}
	else
	{
		internalTypeName = typeName;
	}

	// Check that the types match
	if (strcmp(v.type()->typeId().getName(), internalTypeName.c_str()) != 0)
	{
		return false;
	}

	auto definition = definitionManager_.getDefinition(internalTypeName.c_str());

	if (definition == nullptr)
	{
		return false;
	}

	ObjectHandle vHandle(v, definition);

	for (IBasePropertyPtr prop : definition->allProperties())
	{
		if (prop->isMethod())
		{
			continue;
		}

		// Get node with property name
		const char* propertyName = prop->getName();
		auto propertyNode = node->getChildNode(propertyName, strlen(propertyName));

		// Get Variant value
		Variant propertyVariant = prop->get(vHandle, definitionManager_);
		if (!propertyNode->getValueVariant(propertyVariant))
		{
			// Failed!
			return false;
		}

		// Set the node with the Variant value
		if (!prop->set(vHandle, propertyVariant, definitionManager_))
		{
			return false;
		}
	}

	return true;
}

const char* ReflectedHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return reflectedName_;
}

bool ReflectedHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	// Check if the type is reflected
	auto definition = definitionManager_.getDefinition(v.type()->typeId().getName());
	if (definition == nullptr)
	{
		return false;
	}
	return true;
}

bool ReflectedHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	// If this was written with the type, node->getType() will give "__TFSreflected"
	if (node->getType() == reflectedName_)
	{
		return true;
	}

	// If it wasn't written with the type, then the supplied typeName should have a metaType.
	if (typeName == nullptr)
	{
		return false;
	}

	if (MetaType::find(typeName) != nullptr)
	{
		return true;
	}

	return false;
}

} // end namespace wgt