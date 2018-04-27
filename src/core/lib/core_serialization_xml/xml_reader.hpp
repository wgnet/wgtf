#ifndef XML_READER_HPP_INCLUDED
#define XML_READER_HPP_INCLUDED

#include "core_serialization_xml/xml_serializer.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_reflection/object_handle.hpp"

#include "core_serialization_xml/simple_api_for_xml.hpp"
#include <list>
#include <string>
#include <utility>
#include <cstdint>

namespace wgt
{
class ObjectHandle;
class IDefinitionManager;
class ObjectReference;
class ObjectStorage;
typedef std::shared_ptr<class IBaseProperty> IBasePropertyPtr;

class XMLReader : private SimpleApiForXml
{
	typedef SimpleApiForXml base;

public:
	XMLReader(TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format);

	bool read(Variant& value);
	bool read(IManagedObject& object);

private:
	struct StackItem
	{
		explicit StackItem(Variant&& value);

		Variant value;
		std::shared_ptr<ObjectReference> reference;

		IBasePropertyPtr property;
		Collection::Iterator pos;

		std::string characterData;
		bool hasChildren;
		intmax_t assumedKey;
	};

	IDefinitionManager& definitionManager_;
	const XMLSerializer::Format& format_;
	std::list<StackItem> stack_;
	bool pushed_;
	bool done_;
	unsigned ignore_;
	TypeId objectType_;
	std::shared_ptr<ObjectStorage> objectStorage_;

	void elementStart(const char* elementName, const char* const* attributes) override;
	void elementEnd(const char* elementName) override;
	void characterData(const char* data, size_t length) override;
};
} // end namespace wgt
#endif
