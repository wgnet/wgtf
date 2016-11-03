#ifndef XML_WRITER_HPP_INCLUDED
#define XML_WRITER_HPP_INCLUDED

#include "core_serialization/text_stream.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class Variant;
class Collection;
class ObjectHandle;

class XMLWriter
{
public:
	XMLWriter(TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format);

	bool write(const Variant& value);

private:
	TextStream& stream_;
	IDefinitionManager& definitionManager_;
	const XMLSerializer::Format& format_;
	unsigned indent_;
	bool tagOpening_;
	bool hasChildElements_;

	bool fail() const
	{
		return stream_.fail();
	}

	void writeValue(const Variant& value, bool explicitType, bool isObjectReference = false);
	void writeObject(const ObjectHandle& object, bool explicitType);
	void writeCollection(const Collection& collection);
	void writeVariant(const Variant& variant, bool explicitType);

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
#endif
