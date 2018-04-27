#include "custom_xml_serializer.hpp"
#include "custom_xml_writer.hpp"
#include "custom_xml_reader.hpp"

namespace wgt
{
CustomXmlSerializer::CustomXmlSerializer(IDataStream& dataStream) : stream_(dataStream)
{
}

bool CustomXmlSerializer::serialize(const Variant& value)
{
	CustomXmlDataWriter writer(stream_);
	return writer.write(value);
}

bool CustomXmlSerializer::deserialize(Variant& value)
{
	CustomXmlDataReader reader(stream_);
	return reader.read(value);
}

bool CustomXmlSerializer::sync()
{
	return stream_.sync();
}
} // end namespace wgt
