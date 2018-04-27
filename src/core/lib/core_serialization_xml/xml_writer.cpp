#include "xml_writer.hpp"

#include "core_common/assert.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "core_object/object_reference.hpp"
#include "core_object/object_manager.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "wg_types/base64.hpp"

#include <sstream>

namespace wgt
{
namespace
{
const char* valueType(const Variant& value, IDefinitionManager& definitionManager)
{
	const char* result = value.type()->name();

	value.visit<ObjectHandle>([&](const ObjectHandle& v) {
		if (const IClassDefinition* definition = definitionManager.getDefinition(v))
		{
			result = definition->getName();
		}
	});

	return result;
}

bool writeTypeExplicitly(const TypeId& type)
{
	return type == TypeId::getType<Variant>() || type == TypeId::getType<ObjectHandle>() ||
	type == TypeId::getType<Vector2>() || type == TypeId::getType<Vector3>() || type == TypeId::getType<Vector4>() ||
	type == TypeId::getType<BinaryBlock>();
}

bool canExtractObject(IDefinitionManager& definitionManager, const Variant& variant)
{
    return variant.canCast<ObjectHandle>() || definitionManager.getDefinition(variant.type()->typeId().getName()) != nullptr;
}
}

XMLWriter::XMLWriter(TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format)
    : stream_(stream), definitionManager_(definitionManager), format_(format), indent_(format.indent),
      tagOpening_(false), hasChildElements_(false)
{
}

bool XMLWriter::write(const IManagedObject& object)
{
	beginOpenTag(format_.rootName.c_str());
	writeObject(object.getHandle());
	closeTag(format_.rootName.c_str());

	return !fail();
}

bool XMLWriter::write(const Variant& value)
{
	beginOpenTag(format_.rootName.c_str());
	writeValue(value, true);
	closeTag(format_.rootName.c_str());

	return !fail();
}

void XMLWriter::writeValue(const Variant& value, bool explicitType)
{
    ObjectHandle handle;
    if (value.tryCast(handle))
    {
        writeObject(handle);
        return;
    }
    else if (auto definition = definitionManager_.getDefinition(value.type()->typeId().getName()))
    {
        handle = definitionManager_.getObjectManager()->getObject(value.value<const void*>());
        TF_ASSERT(handle != nullptr);
        writeObject(handle);
        return;
    }

	value.visit<Collection>([=](const Collection& v) { writeCollection(v, nullptr, nullptr); }) ||
	value.visit<BinaryBlock>([=](const BinaryBlock& v) { writeVariant(v, true); }) ||
	value.visit<Variant>([=](const Variant& v) { writeVariant(v, explicitType); });
}

void XMLWriter::writeObject(const ObjectHandle& object)
{
	if (!object.isValid())
	{
		// nullptr objecthandle
		return;
	}

	auto root = reflectedRoot(object, definitionManager_);
	const IClassDefinition* definition = definitionManager_.getDefinition(root);
	static TypeId ObjectHandleType = TypeId::getType<ObjectHandle>();

	if (!definition)
	{
		stream_.setState(std::ios_base::failbit);
		return;
	}

	// actual object type
	// HACK: encode the type string to avoid invalid xml symbols such as "<", ">" which comes from the template class name
	// After refine our xml write which needs to correctly deal with xml symbols, we could remove this hack code.
	std::string attributeType = definition->getName();
	if (definition->isGeneric())
	{
		attributeType = "#" + attributeType;
	}
	std::string encodeValue = Base64::encode(attributeType.c_str(), attributeType.length());
	writeAttribute(format_.encodedTypeAttribute, quoted(encodeValue));

	std::shared_ptr<ObjectReference> reference = std::static_pointer_cast<ObjectReference>(root.storage());
	if (reference->path().empty())
	{
		RefObjectId id = reference->id();
		writeAttribute(format_.objectIdAttribute, quoted(id.toString()));
	}

	for (IBasePropertyPtr property : definition->allProperties())
	{
		if (property->isMethod())
		{
			continue;
		}

		// write object property
		const char* propertyName = property->getName();
		bool linkValue = property->getType() == ObjectHandleType;
		PropertyAccessor accessor = definition->bindProperty(propertyName, root);

		if (!accessor.isValid())
		{
			continue;
		}

		beginOpenTag(format_.propertyName.c_str());
		writeAttribute(format_.propertyNameAttribute, quoted(propertyName));
		Variant value = accessor.getValue();
		TypeId type = value.type()->typeId();
		std::string typeName = type.getName();

		if (linkValue)
		{
			writeLink(value);
		}
		else if (value.canCast<Collection>())
		{
			Collection collection;
			value.tryCast(collection);
			writeCollection(collection, propertyName, reference);
		}
		else
		{
			writeValue(value, writeTypeExplicitly(property->getType()));
		}

		closeTag(format_.propertyName.c_str());

		if (fail())
		{
			return;
		}
	}
}

void XMLWriter::writeCollection(const Collection& collection, const char * path, const std::shared_ptr<ObjectReference>& reference)
{
	// HACK: encode the type string to avoid invalid xml symbols such as "<", ">" which comes from the template class name
	// After refine our xml write which needs to correctly deal with xml symbols, we could remove this hack code.
	std::string attributeType = MetaType::get<Collection>()->name();
	std::string encodeValue = Base64::encode(attributeType.c_str(), attributeType.length());
	writeAttribute(format_.encodedTypeAttribute, quoted(encodeValue));
	static TypeId ObjectHandleType = TypeId::getType<ObjectHandle>();
	bool linkValue = collection.valueType() == ObjectHandleType;
	bool objectValue;
	bool collectionValue;
	Variant value;
	Collection childCollection;
	auto objectManager = definitionManager_.getObjectManager();
	TF_ASSERT(objectManager);
	std::string pathPrefix;
	std::string propertyName;
	std::stringstream indexFormatter;

	intmax_t assumedKey = 0;
	for (auto it = collection.begin(), end = collection.end(); it != end; ++it, ++assumedKey)
	{
		const Variant key = it.key();

		beginOpenTag(format_.collectionItemElement.c_str());

		// write key
		bool keyMatchesAssumed = false;
		intmax_t keyIndex = 0;

		if (key.tryCast(keyIndex))
		{
			if (keyIndex == assumedKey)
			{
				keyMatchesAssumed = true;
			}
			else
			{
				assumedKey = keyIndex; // assumedKey tracks actual key
			}

			indexFormatter.str("");
			indexFormatter << '[' << keyIndex << ']';
			propertyName = indexFormatter.str();
		}

		if (!keyMatchesAssumed)
		{
			std::string keyValue;
			if (!key.type()->testFlags(MetaType::DeducibleFromText))
			{
				// HACK: encode the type string to avoid invalid xml symbols such as "<", ">" which comes from the template class name
				// After refine our xml write which needs to correctly deal with xml symbols, we could remove this hack code.
				std::string attributeType = valueType(key, definitionManager_);
				std::string encodeValue = Base64::encode(attributeType.c_str(), attributeType.length());
				writeAttribute(format_.encodedKeyTypeAttribute, quoted(encodeValue));
				if (!key.tryCast(keyValue))
				{
					// arbitrary type can be saved in attribute only as string
					stream_.setState(std::ios_base::failbit);
					return;
				}
			}
			else
			{
				if (!key.tryCast(keyValue))
				{
					// arbitrary type can be saved in attribute only as string
					stream_.setState(std::ios_base::failbit);
					return;
				}
			}

			writeAttribute(format_.keyAttribute, quoted(keyValue));
			propertyName = Collection::getIndexOpen() + keyValue + Collection::getIndexClose();
		}

		value = it.value();
		collectionValue = !linkValue && value.tryCast(childCollection);
		objectValue = !linkValue && !collectionValue && canExtractObject(definitionManager_, value);

		if (linkValue)
		{
			writeLink(value);
		}
		else if (!objectValue && !collectionValue || !reference)
		{
			writeValue(it.value(), writeTypeExplicitly(collection.valueType()));
		}
		else if (collectionValue)
		{
			std::string fullPath = path + propertyName;
			writeCollection(childCollection, fullPath.c_str(), reference);
		}
		else
		{
			std::string referencePath = reference->fullPath();
			std::string fullPath = referencePath.empty() ? path + propertyName : referencePath + "." + path + propertyName;
			auto childReference = objectManager->getObject(reference->id(), fullPath);
			objectManager->updateObjectStorage(childReference, value);

			writeObject(ObjectReference::asHandle(childReference));
		}

		closeTag(format_.collectionItemElement.c_str());

		if (fail())
		{
			return;
		}
	}
}

void XMLWriter::writeLink(const Variant& value)
{
	ObjectHandle handle;

	if (!value.tryCast(handle))
	{
		return;
	}

	std::shared_ptr<ObjectReference> reference = std::static_pointer_cast<ObjectReference>(handle.storage());
	std::string id = reference->id().toString();
	std::string path = reference->fullPath();

	if (!id.empty())
	{
		writeAttribute(format_.objectIdAttribute, quoted(id));

		if (!path.empty())
		{
			writeAttribute(format_.objectPathAttribute, quoted(path));
		}
	}

	writeAttribute(format_.objectReferenceAttribute, quoted("reference"));
	endOpenTag();
}

void XMLWriter::writeVariant(const Variant& variant, bool explicitType)
{
	if (variant.isVoid())
	{
		// nop
		return;
	}

	if (explicitType && !variant.type()->testFlags(MetaType::DeducibleFromText))
	{
		// HACK: encode the type string to avoid invalid xml symbols such as "<", ">" which comes from the template class name
		// After refine our xml write which needs to correctly deal with xml symbols, we could remove this hack code.
		std::string attributeType = variant.type()->name();
		std::string encodeValue = Base64::encode(attributeType.c_str(), attributeType.length());
		writeAttribute(format_.encodedTypeAttribute, quoted(encodeValue));
	}

	endOpenTag();

	std::string valueString = getEscapedString(variant);

	stream_ << format_.padding << valueString << format_.padding;
}

void XMLWriter::writeIndent()
{
	if (!format_.indentString.empty())
	{
		for (unsigned i = 0; i < indent_; i++)
		{
			stream_ << format_.indentString;
		}
	}
}

void XMLWriter::writeNewline()
{
	if (format_.newLines)
	{
		stream_ << "\n";
	}
}

void XMLWriter::beginOpenTag(const char* tag)
{
	if (tagOpening_)
	{
		stream_ << ">";
		writeNewline();
	}

	writeIndent();
	stream_ << "<" << tag;
	tagOpening_ = true;
	hasChildElements_ = false;
	indent_ += 1;
}

void XMLWriter::endOpenTag()
{
	if (!tagOpening_)
	{
		return;
	}

	stream_ << ">";
	tagOpening_ = false;
}

void XMLWriter::closeTag(const char* tag)
{
	indent_ -= 1;

	if (tagOpening_)
	{
		stream_ << "/>";
		tagOpening_ = false;
	}
	else
	{
		if (hasChildElements_)
		{
			writeIndent();
		}
		stream_ << "</" << tag << ">";
	}

	writeNewline();

	hasChildElements_ = true;
}
} // end namespace wgt