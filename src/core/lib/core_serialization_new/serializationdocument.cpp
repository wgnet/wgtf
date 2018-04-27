#include "serializationdocument.hpp"

namespace wgt
{
SerializationDocument::SerializationDocument(SerializationFormat format, SerializerNew* serializer)
    : format_(format), serializer_(serializer)
{
}

SerializationDocument::~SerializationDocument()
{
}

SerializationFormat SerializationDocument::getFormat() const
{
	return format_;
}

SerializerNew* const SerializationDocument::getSerializer() const
{
	return serializer_;
}

const SerializationDocument::SerializationPrimitiveNames& SerializationDocument::getPrimitiveNames() const
{
	return primitiveNames_;
}

} // end namespace wgt