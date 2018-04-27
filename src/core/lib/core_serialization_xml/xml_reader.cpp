#include "xml_reader.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "core_object/object_reference.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "wg_types/base64.hpp"
#include <utility>
#include <cstring>
#include <sstream>

namespace wgt
{
namespace XMLReaderDetail
{
//==========================================================================
class CollectionElementHolder : public BaseProperty
{
	typedef BaseProperty base;

public:
	CollectionElementHolder(const Collection& collection, const Collection::Iterator& collectionIt,
	                        const TypeId& valueType)
	    :

	      base("", valueType),
	      collection_(collection), collectionIt_(collectionIt)
	{
	}

	const TypeId& getType() const override
	{
		return collectionIt_.valueType();
	}

	virtual bool isValue() const override
	{
		return true;
	}

	virtual bool isCollection() const override
	{
		Collection value;
		return collectionIt_.value().tryCast<Collection>(value);
	}

	virtual bool isByReference() const override
	{
		return true;
	}

	Variant get(const ObjectHandle& pBase, const IDefinitionManager& definitionManager) const override
	{
		TF_ASSERT(this->isValue());
		return collectionIt_.value();
	}

	bool set(const ObjectHandle& handle, const Variant& value, const IDefinitionManager& definitionManager) const override
	{
		TF_ASSERT(!this->readOnly(handle));

		auto valueType = collectionIt_.valueType();
		if (valueType.isPointer())
		{
			auto targetType = valueType.removePointer();
			ObjectHandle source;
			if (value.tryCast(source))
			{
				auto target = reflectedCast(source, targetType, definitionManager);
				return collectionIt_.setValue(target);
			}
		}

		return collectionIt_.setValue(value);
	}

private:
	Collection collection_; // need to keep Collection data alive to keep iterator valid
	Collection::Iterator collectionIt_;
	std::string propName_;
};
}

XMLReader::StackItem::StackItem(Variant&& value)
    : value(std::move(value)), reference(nullptr), property(nullptr), pos(), characterData(),
      hasChildren(false), assumedKey(0)
{
}

XMLReader::XMLReader(TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format)
    : base(stream), definitionManager_(definitionManager), format_(format), stack_(), pushed_(false), done_(false),
      ignore_(0)
{
}

bool XMLReader::read(Variant& value)
{
	objectType_ = TypeId();
	stack_.emplace_back(std::move(value));
	pushed_ = true;
	done_ = false;

	if (!parse() && !done_)
	{
		return false;
	}

	value = std::move(stack_.back().value);
	stack_.pop_back();
	return true;
}

bool XMLReader::read(IManagedObject& object)
{
	objectType_ = object.type();
	stack_.emplace_back(Variant());
	pushed_ = true;
	done_ = false;

	if (!parse() && !done_)
	{
		return false;
	}

	std::shared_ptr<RootObjectReference> reference =
		std::static_pointer_cast<RootObjectReference>(stack_.back().reference);
	stack_.pop_back();
	
	return reference && object.set(std::move(reference), std::move(objectStorage_));
}

void XMLReader::elementStart(const char* elementName, const char* const* attributes)
{
	if (ignore_)
	{
		ignore_ += 1;
		return;
	}

	// parse attributes
	const char* type = nullptr;
	const char* objectId = nullptr;
	const char* objectPath = nullptr;
	const char* objectReference = nullptr;
	const char* propertyName = nullptr;
	const char* keyType = nullptr;
	const char* key = nullptr;
	std::string decodeTypeValue;
	std::string decodeKeyTypeValue;

	for (auto attribute = attributes; *attribute; attribute += 2)
	{
		const char* attributeName = attribute[0];
		const char* attributeValue = attribute[1];

		if (attributeName == format_.typeAttribute)
		{
			type = attributeValue;
		}
		else if(attributeName == format_.encodedTypeAttribute)
		{
			// HACK: decode the type string
			// After refine our xml write which needs to correctly deal with xml symbols, we could remove this hack code.
			bool ret = Base64::decode(attributeValue, decodeTypeValue);
			TF_ASSERT(ret);
			type = decodeTypeValue.c_str();
		}
		else if (attributeName == format_.objectIdAttribute)
		{
			objectId = attributeValue;
		}
		else if (attributeName == format_.objectPathAttribute)
		{
			objectPath = attributeValue;
		}
		else if (attributeName == format_.objectReferenceAttribute)
		{
			objectReference = attributeValue;
		}
		else if (attributeName == format_.propertyNameAttribute)
		{
			propertyName = attributeValue;
		}
		else if (attributeName == format_.keyTypeAttribute)
		{
			keyType = attributeValue;
		}
		else if (attributeName == format_.encodedKeyTypeAttribute)
		{
			// HACK: decode the type string
			// After refine our xml write which needs to correctly deal with xml symbols, we could remove this hack code.
			bool ret = Base64::decode(attributeValue, decodeKeyTypeValue);
			TF_ASSERT(ret);
			keyType = decodeKeyTypeValue.c_str();
		}
		else if (attributeName == format_.keyAttribute)
		{
			key = attributeValue;
		}
		else
		{
			// ignore unknown attributes
		}
	}

	// prepare stack top item
	TF_ASSERT(!stack_.empty());

	if (pushed_)
	{
		// asusme that value for the current element is already on the stack
		pushed_ = false;
	}
	else
	{
		auto& current = stack_.back();
		current.hasChildren = true;
		current.characterData.clear();

		ObjectHandle handle;
		if (current.reference)
		{
			// find and push property
			handle = ObjectReference::asHandle(current.reference);
		}
		else if (definitionManager_.getDefinition(current.value.type()->typeId().getName()) != nullptr)
		{
			handle = ObjectHandle(std::static_pointer_cast<IObjectHandleStorage>(
				std::make_shared<ObjectHandleVariantStorage>(current.value)));
		}

		if (handle.isValid())
		{
			const IClassDefinition* classDefinition = definitionManager_.getDefinition(handle);

			if (!classDefinition)
			{
				// type is not reflected
				abortParsing();
				return;
			}


			TF_ASSERT(propertyName != nullptr);
			IBasePropertyPtr property = classDefinition->findProperty(propertyName);
			if (!property)
			{
				// ignore unknown properties
				ignore_ = 1;
				return;
			}

			if (property->isMethod())
			{
				// ignore method properties
				ignore_ = 1;
				return;
			}


			stack_.emplace_back(property->get(handle, definitionManager_));
			auto& newItem = stack_.back();
			newItem.property = property;
		}
		else if (auto collection = current.value.value<Collection*>())
		{
			TF_ASSERT(collection->isValid());
			if (elementName != format_.collectionItemElement)
			{
				// ignore non-item properties
				ignore_ = 1;
				return;
			}

			Variant k;
			if (key)
			{
				k = key;

				if (keyType)
				{
					const MetaType* keyMetaType = MetaType::find(keyType);
					if (!keyMetaType)
					{
						// key type not found
						abortParsing();
						return;
					}

					if (!k.setType(keyMetaType))
					{
						// key type conversion failed
						abortParsing();
						return;
					}
				}

				intmax_t keyIndex = 0;
				if (k.tryCast(keyIndex))
				{
					current.assumedKey = keyIndex;
				}
			}
			else
			{
				k = current.assumedKey;
			}

			current.assumedKey += 1;
			auto findIt = collection->find(k);
			if (findIt == collection->end())
			{
				findIt = collection->insert(k);
			}

			stack_.emplace_back(findIt.value());
			auto& newItem = stack_.back();
			newItem.property = std::make_shared<XMLReaderDetail::CollectionElementHolder>(
				*collection, findIt, collection->valueType());
		}
		else
		{
			// This may occur if we didn't find ClassDefinition for parent
			// element and assumed parent to be primitive value. So consider
			// this situation as an error.
			abortParsing();
			return;
		}
	}

	auto& current = stack_.back();

	if (objectReference != nullptr)
	{
		if (!current.value.isVoid() && strcmp("object", current.value.type()->name()) != 0)
		{
			// Reference in XML, non reference property.
			abortParsing();
			return;
		}

		if (!objectId)
		{
			current.value = ObjectHandle();
			return;
		}

		auto objectManager = definitionManager_.getObjectManager();
		std::string path = objectPath ? objectPath : "";
		auto reference = objectManager->getObject(RefObjectId(objectId), path);
		current.value = ObjectReference::asHandle(reference);
		return;
	}

	if (type == nullptr || 
		strcmp(type, current.value.type()->name()) == 0 || 
		strcmp(type, current.value.type()->typeId().getName()) == 0)
	{
		return;
	}

	if (!current.value.isVoid() && strcmp("object", current.value.type()->name()) != 0)
	{
		// types do not match
		abortParsing();
		return;
	}

	IClassDefinition * definition = nullptr;
	if (*type == '#')
	{
		definition = definitionManager_.getDefinition(++type);
		if (definition == nullptr)
		{
			auto definitionDetails = definitionManager_.createGenericDefinition(type);
			definition = definitionManager_.registerDefinition(std::move(definitionDetails));
		}
	}
	else
	{
		definition = definitionManager_.getDefinition(type);
	}

	if (definition != nullptr)
	{
		RefObjectId id = objectId ? objectId : "";
		if (id != RefObjectId::zero())
		{
			auto reference = definitionManager_.getObjectManager()->getObject(id, "");
			if (reference && reference->storage())
			{
				if(strcmp(reference->type().getName(), type) == 0)
				{
					current.reference = reference;
				}
				else if(definition->isGeneric())
				{
					auto refDefinition = definitionManager_.getDefinition(
						ObjectReference::asHandle(reference));
					if(refDefinition && refDefinition->isGeneric())
					{
						current.reference = reference;
					}
				}
			}
		}
		if (!current.reference)
		{
			// TODO pass in id
			auto object = definition->createShared(id);
			current.reference = std::dynamic_pointer_cast<ObjectReference>(object.storage());
		}
		return;
	}

	if (const MetaType* metaType = MetaType::find(type))
	{
		current.value = Variant(metaType);
		return;
	}

	// type not found
	abortParsing();
	return;
}

void XMLReader::elementEnd(const char* elementName)
{
	if (ignore_)
	{
		ignore_ -= 1;
		return;
	}

	TF_ASSERT(!stack_.empty());

	// parse character data
	auto& current = stack_.back();
	if (current.reference != nullptr)
	{
		current.value = ObjectReference::asHandle(current.reference);
	}
	else if (!current.characterData.empty())
	{
		FixedMemoryStream dataStream(current.characterData.c_str(), current.characterData.size());
		TextStream stream(dataStream);
		stream >> current.value;
		stream.skipWhiteSpace();

		if (stream.fail() || !stream.eof())
		{
			// failed to deserialize primitive value
			abortParsing();
			return;
		}
	}

	if (stack_.size() <= 1)
	{
		// this seems to be a root value: we're done
		done_ = true;
		abortParsing();
		return;
	}

	// move current element to its parent
	ObjectHandle handle;
	auto parentIt = stack_.rbegin();
	do
	{
		++parentIt;
		if (parentIt->reference != nullptr)
		{
			handle = ObjectReference::asHandle(parentIt->reference);
		}
		else if (definitionManager_.getDefinition(current.value.type()->typeId().getName()) != nullptr)
		{
			handle = ObjectHandle(std::static_pointer_cast<IObjectHandleStorage>(
				std::make_shared<ObjectHandleVariantStorage>(current.value)));
		}
	} while (parentIt != stack_.rend() && !handle.isValid());
	TF_ASSERT(handle.isValid());

	TF_ASSERT(current.property);
	current.property->set(handle, std::move(current.value), definitionManager_);
	stack_.pop_back();
}

void XMLReader::characterData(const char* data, size_t length)
{
	if (ignore_)
	{
		return;
	}

	TF_ASSERT(!stack_.empty());

	auto& current = stack_.back();

	if (current.reference)
	{
		if (!current.hasChildren)
		{
			current.characterData.append(data, data + length);
		}
	}
	else if (current.value.value<Collection*>())
	{
		// ignore character data in collection
	}
	else
	{
		current.characterData.append(data, data + length);
	}
}
} // end namespace wgt
