#ifndef CUSTOM_XML_READER_HPP_INCLUDED
#define CUSTOM_XML_READER_HPP_INCLUDED

#include "core_serialization_xml/xml_serializer.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"

#include "core_serialization_xml/simple_api_for_xml.hpp"
#include <list>
#include <string>
#include <utility>
#include <cstdint>
#include "custom_xml_data.hpp"

namespace wgt
{
class ObjectHandle;
class IBaseProperty;
class IDefinitionManager;

// just a simple xml reader, you can make your own robust xml reader
class CustomXmlDataReader : private SimpleApiForXml
{
	typedef SimpleApiForXml base;

public:
	CustomXmlDataReader(TextStream& stream);

	bool read(Variant& value);

private:
	struct StackItem
	{
		explicit StackItem(Variant value = Variant());

		void cast();

		Variant value;
		CustomXmlData* data;
		std::string characterData;
		bool hasChildren;
	};

	std::list<StackItem> stack_;
	bool pushed_;
	bool done_;
	unsigned ignore_;

	void elementStart(const char* elementName, const char* const* attributes) override;
	void elementEnd(const char* elementName) override;
	void characterData(const char* data, size_t length) override;
};
} // end namespace wgt
#endif // CUSTOM_XML_READER_HPP_INCLUDED
