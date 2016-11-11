#ifndef CUSTOM_XML_WRITER_HPP_INCLUDED
#define CUSTOM_XML_WRITER_HPP_INCLUDED

#include "core_serialization/text_stream.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class Variant;
class Collection;
class ObjectHandle;

// just a simple xml writer, you can make your own robust xml writer
class CustomXmlDataWriter
{
public:
	CustomXmlDataWriter(TextStream& stream);

	bool write(const Variant& value);

private:
	TextStream& stream_;
	unsigned indent_;
	bool tagOpening_;
	bool hasChildElements_;

	bool fail() const
	{
		return stream_.fail();
	}

	void writeValue(const Variant& value);

	void writeIndent();
	void writeNewline();

	void beginOpenTag(const char* tag);
	void endOpenTag();
	void closeTag(const char* tag);

	template <typename Name, typename T>
	void writeAttribute(const Name& name, const T& value)
	{
		if (!tagOpening_)
		{
			stream_.setState(std::ios_base::failbit);
			return;
		}

		stream_ << " " << name << "=" << value;
	}
};
} // end namespace wgt
#endif // CUSTOM_XML_WRITER_HPP_INCLUDED
