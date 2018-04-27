#include "collectionhandler.hpp"

namespace wgt
{
CollectionHandler::CollectionHandler(const SerializationHandlerManager& handlerManager)
    : SerializationHandler("__CollectionHandler"), handlerManager_(handlerManager)
{
}

bool CollectionHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	node->setHandlerName(handlerName_);
	if (writeType)
	{
		node->setType(collectionName_, strlen(collectionName_));
	}

	bool success = false;

	Collection c;
	if (!v.tryCast<Collection>(c))
	{
		return false;
	}

	if (c.empty())
	{
		return true;
	}

	// Check that the key isn't a collection, since we can't serialize those.
	if (c.begin().key().canCast<Collection>())
	{
		return false;
	}

	// Find and hold on to our handlers
	std::shared_ptr<SerializationHandler> keyHandler = nullptr;
	std::shared_ptr<SerializationHandler> valueHandler = nullptr;
	const MetaType* keyMetaType = MetaType::find(c.keyType().getName());
	const MetaType* valueMetaType = MetaType::find(c.valueType().getName());
	// If a type in the collection is Variant, then the handler can vary, so leave it as nullptr.
	if (keyMetaType != MetaType::get<Variant>())
	{
		keyHandler = handlerManager_.findHandlerWrite(c.begin().key());
	}
	if (valueMetaType != MetaType::get<Variant>())
	{
		valueHandler = handlerManager_.findHandlerWrite(c.begin().value());
	}

	// Create a base node for all the elements in the collection.
	auto baseNode = node->createEmptyChild(collectionBaseNodeName_, strlen(collectionBaseNodeName_));

	// Iterate through all the objects in the collection
	for (auto iter = c.begin(); iter != c.end(); ++iter)
	{
		// Write the key, if it exists
		Variant key = iter.key();
		if (!key.isVoid())
		{
			// Create the key node
			std::unique_ptr<SerializationNode> keyNode;
			if (keyHandler.get() == nullptr)
			{
				// Variant type, go through the entire system
				keyNode = baseNode->createChildVariant(keyName_, strlen(keyName_), key);
			}
			else
			{
				// Non-Variant type, use our handler
				keyNode = baseNode->createEmptyChild(keyName_, strlen(keyName_));
				if (!keyHandler->write(key, keyNode, false))
				{
					return false;
				}
			}
			if (keyNode == nullptr)
			{
				return false;
			}

			// Write the value
			Variant value = iter.value();
			if (!value.isVoid())
			{
				// Create the value node
				std::unique_ptr<SerializationNode> valueNode;
				if (valueHandler.get() == nullptr)
				{
					// Variant type, go through the entire system
					valueNode = keyNode->createChildVariant(valueName_, strlen(valueName_), value);
				}
				else
				{
					// Non-Variant type, use our handler
					valueNode = keyNode->createEmptyChild(valueName_, strlen(valueName_));
					if (!valueHandler->write(value, valueNode, false))
					{
						return false;
					}
				}
				if (valueNode == nullptr)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	// Write the types and handlers last so that the serialized list can be iterated through intuitively
	// Container type
	const char* collectionContainerName = this->getReservedNames().collectionContainerType;
	auto containerTypeNode = node->createEmptyChild(collectionContainerName, strlen(collectionContainerName));
	const char* collectionContainerType = c.impl()->containerType().getName();
	containerTypeNode->setType(collectionContainerType, strlen(collectionContainerType));
	// Key variant type
	const char* collectionKeyVName = this->getReservedNames().collectionKeyVariantType;
	auto keyVTypeNode = node->createEmptyChild(collectionKeyVName, strlen(collectionKeyVName));
	const char* collectionKeyVType = c.keyType().getName();
	keyVTypeNode->setType(collectionKeyVType, strlen(collectionKeyVType));
	// Value variant type
	const char* collectionValueVName = this->getReservedNames().collectionValueVariantType;
	auto valueVTypeNode = node->createEmptyChild(collectionValueVName, strlen(collectionValueVName));
	const char* collectionValueVType = c.valueType().getName();
	valueVTypeNode->setType(collectionValueVType, strlen(collectionValueVType));
	// Key internal type
	const char* collectionKeyIName = this->getReservedNames().collectionKeyInternalType;
	auto keyITypeNode = node->createEmptyChild(collectionKeyIName, strlen(collectionKeyIName));
	const char* collectionKeyIType = keyHandler->getInternalTypeOf(c.begin().key(), node);
	keyITypeNode->setType(collectionKeyIType, strlen(collectionKeyIType));
	// Value internal type
	const char* collectionValueIName = this->getReservedNames().collectionValueInternalType;
	auto valueITypeNode = node->createEmptyChild(collectionValueIName, strlen(collectionValueIName));
	const char* collectionValueIType = valueHandler->getInternalTypeOf(c.begin().value(), node);
	valueITypeNode->setType(collectionValueIType, strlen(collectionValueIType));
	// Key handler
	if (keyHandler.get() != nullptr)
	{
		const char* collectionKeyHandlerName = this->getReservedNames().collectionKeyHandler;
		auto keyHandlerNode = node->createEmptyChild(collectionKeyHandlerName, strlen(collectionKeyHandlerName));
		const char* collectionKeyHandler = keyHandler->getName();
		keyHandlerNode->setValueString(collectionKeyHandler, strlen(collectionKeyHandler));
	}
	// Value handler
	if (valueHandler.get() != nullptr)
	{
		const char* collectionValueHandlerName = this->getReservedNames().collectionValueHandler;
		auto valueHandlerNode = node->createEmptyChild(collectionValueHandlerName, strlen(collectionValueHandlerName));
		const char* collectionValueHandler = valueHandler->getName();
		valueHandlerNode->setValueString(collectionValueHandler, strlen(collectionValueHandler));
	}

	return true;
}

bool CollectionHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Check that we've been given a valid Variant
	if (v.isVoid() || v.type() == nullptr)
	{
		return false;
	}

	// Container, key, and value types
	TypeId containerType;
	TypeId keyVariantType;
	TypeId valueVariantType;
	std::string keyInternalType;
	std::string valueInternalType;

	// Container
	const char* containerName = this->getReservedNames().collectionContainerType;
	std::string containerTypeName = node->getChildNode(containerName, strlen(containerName))->getType();
	containerType = TypeId(containerTypeName.c_str());
	// Key variant type
	const char* keyVName = this->getReservedNames().collectionKeyVariantType;
	std::string keyVTypeName = node->getChildNode(keyVName, strlen(keyVName))->getType();
	keyVariantType = TypeId(keyVTypeName.c_str());
	// Value variant type
	const char* valueVName = this->getReservedNames().collectionValueVariantType;
	std::string valueVTypeName = node->getChildNode(valueVName, strlen(valueVName))->getType();
	valueVariantType = TypeId(valueVTypeName.c_str());
	// Key internal type
	const char* keyIName = this->getReservedNames().collectionKeyInternalType;
	keyInternalType = node->getChildNode(keyIName, strlen(keyIName))->getType();
	// Value internal type
	const char* valueIName = this->getReservedNames().collectionValueInternalType;
	valueInternalType = node->getChildNode(valueIName, strlen(valueIName))->getType();

	// Cast to a collection
	Collection c;
	if (!v.tryCast<Collection>(c))
	{
		return false;
	}

	// Check that the types match
	if (strcmp(c.impl()->containerType().getName(), containerType.getName()) != 0)
	{
		return false;
	}

	// Get handlers
	std::shared_ptr<SerializationHandler> keyHandler = nullptr;
	std::shared_ptr<SerializationHandler> valueHandler = nullptr;
	if (MetaType::find(keyVariantType.getName()) != MetaType::get<Variant>())
	{
		const char* keyHandlerName = this->getReservedNames().collectionKeyHandler;
		auto keyHandlerNode = node->getChildNode(keyHandlerName, strlen(keyHandlerName));
		std::string keyHandlerString = keyHandlerNode->getValueString();
		keyHandler = handlerManager_.findHandlerRead(node, keyHandlerString.c_str(), keyInternalType.c_str());
	}
	if (MetaType::find(valueVariantType.getName()) != MetaType::get<Variant>())
	{
		const char* valueHandlerName = this->getReservedNames().collectionValueHandler;
		auto valueHandlerNode = node->getChildNode(valueHandlerName, strlen(valueHandlerName));
		std::string valueHandlerString = valueHandlerNode->getValueString();
		valueHandler = handlerManager_.findHandlerRead(node, valueHandlerString.c_str(), valueInternalType.c_str());
	}

	// Get the base node of the collection
	auto baseNode = node->getChildNode(collectionBaseNodeName_, strlen(collectionBaseNodeName_));

	auto nodeChildren = baseNode->getAllChildren();

	auto collectionIter = c.begin();

	// Iterate through all the children until we reach the nodes for storing type data
	for (auto nodeIter = nodeChildren.begin(); nodeIter != nodeChildren.end(); ++nodeIter)
	{
		std::unique_ptr<SerializationNode>& currentNode = *nodeIter;
		Variant key = Variant(keyVariantType);

		// Assign key/value
		if (currentNode->getName() != keyName_)
		{
			return false;
		}

		// Read the key node
		if (keyHandler.get() == nullptr)
		{
			// Read as a Variant
			if (!currentNode->getValueVariant(key, keyVariantType.getName()))
			{
				return false;
			}
		}
		else
		{
			// Read with a handler, using the type supplied
			if (!keyHandler->read(key, currentNode, keyVariantType.getName()))
			{
				return false;
			}
		}

		// Insert/get the key, which creates the value for us.
		Collection::Iterator insertIter;
		if (c.canResize())
		{
			insertIter = c.insert(key);
		}
		else
		{
			if (key != collectionIter.key())
			{
				return false;
			}
			insertIter = collectionIter;
			++collectionIter;
		}
		if (insertIter == c.end())
		{
			return false;
		}

		Variant value = insertIter.value();

		auto childNode = currentNode->getChildNode(valueName_, strlen(valueName_));
		if (childNode == nullptr)
		{
			return false;
		}

		// Read the value node
		if (valueHandler.get() == nullptr)
		{
			// Read as a Variant
			if (!childNode->getValueVariant(value, valueVariantType.getName()))
			{
				return false;
			}
		}
		else
		{
			// Read with a handler, using the type supplied
			if (!valueHandler->read(value, childNode, valueVariantType.getName()))
			{
				return false;
			}
		}

		// If this collection doesn't support setting values, then it's invalid.
		if (!insertIter.setValue(value))
		{
			return false;
		}
	}

	return true;
}

const char* CollectionHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return collectionName_;
}

bool CollectionHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	if (v.typeIs<Collection>() || v.canCast<Collection>())
	{
		return true;
	}

	return false;
}

bool CollectionHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	std::string nodeType = typeName != nullptr ? typeName : node->getType();
	if (nodeType == collectionName_)
	{
		return true;
	}

	return false;
}

} // end namespace wgt