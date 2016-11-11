#ifndef XML_SERIALIZER_HPP_INCLUDED
#define XML_SERIALIZER_HPP_INCLUDED

#include "i_serializer.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/serialization_dll.hpp"
#include <string>

namespace wgt
{
class IDefinitionManager;

class SERIALIZATION_DLL XMLSerializer : public ISerializer
{
public:
	using ISerializer::deserialize;

	struct SERIALIZATION_DLL Format
	{
		struct Unformatted
		{
		};

		/**
		Construct Format with default values.
		*/
		Format();

		/**
		Construct Format with all formatting turned off.
		*/
		Format(Unformatted);

		/**
		Name of the root node.

		Default is `"root"`.
		*/
		std::string rootName;

		/**
		Name of the property node.

		Default is `"property"`.
		*/
		std::string propertyName;

		/**
		String used for indentation.

		Default is `"\t"`.
		*/
		std::string indentString;

		/**
		Use new lines for formatting.

		Default is `true`.
		*/
		bool newLines;

		/**
		String used for tag content padding.

		Default is `"\t"`.
		*/
		std::string padding;

		/**
		Indentation level.

		Specifies initial indentation level of the root node.

		Default is `0`.
		*/
		unsigned indent;

		// names for XML nodes
		std::string typeAttribute;
		std::string objectIdAttribute;
		std::string objectReferenceAttribute;
		std::string propertyNameAttribute;
		std::string keyTypeAttribute;
		std::string keyAttribute;
		std::string collectionItemElement;
	};

	XMLSerializer(IDataStream& dataStream, IDefinitionManager& definitionManager);

	bool serialize(const Variant& value) override;
	bool deserialize(Variant& value) override;

	const Format& format() const
	{
		return format_;
	}

	void setFormat(const Format& format);

	bool sync();

private:
	TextStream stream_;
	IDefinitionManager& definitionManager_;
	Format format_;
};

} // end namespace wgt
#endif
