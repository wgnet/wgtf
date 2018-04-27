#include "xml_serializer.hpp"
#include "core_serialization_xml/xml_writer.hpp"
#include "core_serialization_xml/xml_reader.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
namespace
{
const char* const TYPE_ATTRIBUTE = "type";
const char* const ENCODE_TYPE_ATTRIBUTE = "encodedType";
const char* const OBJECTID_ATTRIBUTE = "id";
const char* const OBJECTPATH_ATTRIBUTE = "path";
const char* const OBJECTREFERENCE_ATTRIBUTE = "object_reference";
const char* const PROPERTYNAME_ATTRIBUTE = "name";
const char* const KEY_TYPE_ATTRIBUTE = "keyType";
const char* const ENCODE_KEY_TYPE_ATTRIBUTE = "encodedKeyType";
const char* const KEY_ATTRIBUTE = "key";
const char* const COLLECTION_ITEM_ELEMENT = "item";
}

XMLSerializer::Format::Format()
    : rootName("root"), propertyName("property"), indentString("\t"), newLines(true), padding("\t"), indent(0),
      typeAttribute(TYPE_ATTRIBUTE), encodedTypeAttribute(ENCODE_TYPE_ATTRIBUTE), objectIdAttribute(OBJECTID_ATTRIBUTE), objectPathAttribute(OBJECTPATH_ATTRIBUTE),
      objectReferenceAttribute(OBJECTREFERENCE_ATTRIBUTE), propertyNameAttribute(PROPERTYNAME_ATTRIBUTE),
      keyTypeAttribute(KEY_TYPE_ATTRIBUTE), encodedKeyTypeAttribute(ENCODE_KEY_TYPE_ATTRIBUTE), keyAttribute(KEY_ATTRIBUTE), collectionItemElement(COLLECTION_ITEM_ELEMENT)
{
}

XMLSerializer::Format::Format(Unformatted)
    : rootName("root"), propertyName("property"), indentString(), newLines(false), padding(), indent(0),
      typeAttribute(TYPE_ATTRIBUTE), encodedTypeAttribute(ENCODE_TYPE_ATTRIBUTE), objectIdAttribute(OBJECTID_ATTRIBUTE), objectPathAttribute(OBJECTPATH_ATTRIBUTE),
      objectReferenceAttribute(OBJECTREFERENCE_ATTRIBUTE), propertyNameAttribute(PROPERTYNAME_ATTRIBUTE),
      keyTypeAttribute(KEY_TYPE_ATTRIBUTE), encodedKeyTypeAttribute(ENCODE_KEY_TYPE_ATTRIBUTE), keyAttribute(KEY_ATTRIBUTE), collectionItemElement(COLLECTION_ITEM_ELEMENT)
{
}

XMLSerializer::XMLSerializer(IDataStream& dataStream, IDefinitionManager& definitionManager)
    : stream_(dataStream), definitionManager_(definitionManager), format_()
{
}

bool XMLSerializer::serialize(const Variant& value)
{
	XMLWriter writer(stream_, definitionManager_, format_);
	return writer.write(value);
}

bool XMLSerializer::deserialize(Variant& value)
{
	XMLReader reader(stream_, definitionManager_, format_);
	return reader.read(value);
}

bool XMLSerializer::serialize(const IManagedObject& object)
{
	XMLWriter writer(stream_, definitionManager_, format_);
	return writer.write(object);
}

bool XMLSerializer::deserialize(IManagedObject& object)
{
	XMLReader reader(stream_, definitionManager_, format_);
	return reader.read(object);
}

void XMLSerializer::setFormat(const Format& format)
{
	format_ = format;
}

bool XMLSerializer::sync()
{
	return stream_.sync();
}
} // end namespace wgt
