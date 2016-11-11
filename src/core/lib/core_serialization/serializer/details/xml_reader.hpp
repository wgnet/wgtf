#ifndef XML_READER_HPP_INCLUDED
#define XML_READER_HPP_INCLUDED

#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"

#include "simple_api_for_xml.hpp"
#include <list>
#include <string>
#include <utility>
#include <cstdint>

namespace wgt
{
class ObjectHandle;
class IBaseProperty;
class IDefinitionManager;

class XMLReader : private SimpleApiForXml
{
	typedef SimpleApiForXml base;

public:
	XMLReader(TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format);

	bool read(Variant& value);

private:
	struct StackItem
	{
		explicit StackItem(Variant value = Variant());

		void cast(IDefinitionManager& definitionManager);

		Variant value;
		ObjectHandle object;
		Collection* collection;

		IBasePropertyPtr property;
		Collection::Iterator pos;

		std::string characterData;
		bool hasChildren;
		intmax_t assumedKey;
		std::string objectId;
		bool needResolve;
	};

	IDefinitionManager& definitionManager_;
	const XMLSerializer::Format& format_;
	std::list<StackItem> stack_;
	bool pushed_;
	bool done_;
	unsigned ignore_;

	void elementStart(const char* elementName, const char* const* attributes) override;
	void elementEnd(const char* elementName) override;
	void characterData(const char* data, size_t length) override;
};
} // end namespace wgt
#endif
