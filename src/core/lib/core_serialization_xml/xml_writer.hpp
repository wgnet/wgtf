#ifndef XML_WRITER_HPP_INCLUDED
#define XML_WRITER_HPP_INCLUDED

#include "core_serialization/text_stream.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
class Variant;
class Collection;
class ObjectHandle;

class XMLWriter
{
public:
	XMLWriter(TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format);

	bool write(const IManagedObject& object);
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

	void writeValue(const Variant& value, bool explicitType);
	void writeObject(const ObjectHandle& object);
	void writeCollection(const Collection& collection, const char * path = nullptr, const std::shared_ptr<ObjectReference>& reference = nullptr);
	void writeLink(const Variant& value);
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

		std::string nameString = getEscapedString(name);
		std::string valueString = getEscapedString(value);

		stream_ << " " << nameString << "=" << valueString;
	}

	// If we find ourselves outputting other characters that
	// need escaping to work with expat, we can add them here.
	template<typename T>
	std::string getEscapedString(const T& value)
	{
		ResizingMemoryStream valueRStream;
		TextStream valueStream(valueRStream);
		valueStream << value;
		valueStream.seek(0, std::ios_base::beg);
		std::string valueString;
		while(!valueStream.eof())
		{
			char currentChar = valueStream.get();
			if(valueStream.bad() || currentChar == EOF)
			{
				break;
			}

			if(currentChar == '&')
			{
				valueString.append("&amp;");
			}
			else
			{
				valueString.push_back(currentChar);
			}
		}
		return std::move(valueString);
	}
};
} // end namespace wgt
#endif
