#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "custom_xml_data.hpp"
#include "core_variant/variant.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "custom_xml_serializer.hpp"
#include <memory>
#include "core_serialization/text_stream_manip.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_reflection/i_definition_manager.hpp"

#if !defined USE_VARIANT_STREAM_OPERATOR
#define USE_VARIANT_STREAM_OPERATOR 1
#endif

#if USE_VARIANT_STREAM_OPERATOR
namespace wgt
{
namespace
{
const char g_separator = ',';
}

TextStream& operator<<(TextStream& stream, const CustomXmlData& data)
{
	stream << Variant(data.name_) << g_separator << Variant(data.filename_) << g_separator << Variant(data.createdBy_)
	       << g_separator << data.visibility_ << g_separator << Variant(data.position_);
	return stream;
}

TextStream& operator>>(TextStream& stream, CustomXmlData& data)
{
	Variant name(data.name_);
	Variant filename(data.filename_);
	Variant createdBy(data.createdBy_);
	Variant position(data.position_);
	stream >> name >> match(g_separator) >> filename >> match(g_separator) >> createdBy >> match(g_separator) >>
	data.visibility_ >> match(g_separator) >> position;
	bool isOk = name.tryCast(data.name_);
	assert(isOk);
	isOk = filename.tryCast(data.filename_);
	assert(isOk);
	isOk = createdBy.tryCast(data.createdBy_);
	assert(isOk);
	isOk = position.tryCast(data.position_);
	assert(isOk);

	return stream;
}

BinaryStream& operator<<(BinaryStream& stream, const CustomXmlData& data)
{
	return stream;
}

BinaryStream& operator>>(BinaryStream& stream, CustomXmlData& data)
{
	return stream;
}

#endif

/**
* A plugin which tests reading and writing XML data
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class CustomXmlSerializationPlugin : public PluginMain
{
public:
	//==========================================================================
	CustomXmlSerializationPlugin(IComponentContext& contextManager)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		testCase1(contextManager);
#if USE_VARIANT_STREAM_OPERATOR
		testCase2(contextManager);
#endif
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager)
	{
	}

	/**
	* store custom data into custom defined xml format, and using CustomXmlSerializer
	*/
	void testCase1(IComponentContext& contextManager)
	{
		auto fileSystem = contextManager.queryInterface<IFileSystem>();
		if (fileSystem)
		{
			std::string objectFile = "test_serialization1.xml";
			CustomXmlData data;
			{
				data.name_ = "obj1";
				data.filename_ = "pathtoobj1/obj1";
				data.createdBy_ = "Amy";
				data.visibility_ = true;
				data.position_ = Vector3(-6.0f, 0.0f, -10.0f);
			}
			// save data
			{
				ResizingMemoryStream stream;
				CustomXmlSerializer serializer(stream);
				serializer.serialize(data);
				serializer.sync();
				fileSystem->writeFile(objectFile.c_str(), stream.buffer().c_str(), stream.buffer().size(),
				                      std::ios::out | std::ios::binary);
			}

			// read data
			{
				if (fileSystem->exists(objectFile.c_str()))
				{
					IFileSystem::IStreamPtr fileStream =
					fileSystem->readFile(objectFile.c_str(), std::ios::in | std::ios::binary);
					CustomXmlSerializer serializer(*fileStream);
					const MetaType* metaType = MetaType::find("CustomXmlData");
					Variant value(metaType);
					serializer.deserialize(value);
					CustomXmlData newData;
					bool isOk = value.tryCast(newData);
					assert(isOk);
					assert(data == newData);
				}
			}
		}
		else
		{
			assert(false);
		}
	}
#if USE_VARIANT_STREAM_OPERATOR
	/**
	* store custom data as NGT internal xml format, and using XmlSerializer which lives in NGT core_serialization lib
	*/
	void testCase2(IComponentContext& contextManager)
	{
		auto fileSystem = contextManager.queryInterface<IFileSystem>();
		auto defManager = contextManager.queryInterface<IDefinitionManager>();
		if (fileSystem && defManager)
		{
			std::string objectFile = "test_serialization2.xml";
			CustomXmlData data;
			data.name_ = "obj1";
			data.filename_ = "pathtoobj1/obj1";
			data.createdBy_ = "Amy";
			data.visibility_ = true;
			data.position_ = Vector3(-6.0f, 0.0f, -10.0f);
			// save data
			{
				ResizingMemoryStream stream;
				XMLSerializer serializer(stream, *defManager);
				serializer.serialize(data);
				serializer.sync();
				fileSystem->writeFile(objectFile.c_str(), stream.buffer().c_str(), stream.buffer().size(),
				                      std::ios::out | std::ios::binary);
				;
			}
			// read data
			{
				if (fileSystem->exists(objectFile.c_str()))
				{
					IFileSystem::IStreamPtr fileStream =
					fileSystem->readFile(objectFile.c_str(), std::ios::in | std::ios::binary);
					// you can use either CustomXmlSerializer or XMLSerializer to read the data, both are working
					// XMLSerializer serializer( *fileStream, *defManager );
					CustomXmlSerializer serializer(*fileStream);
					const MetaType* metaType = MetaType::find("CustomXmlData");
					Variant value(metaType);
					serializer.deserialize(value);
					CustomXmlData newData;
					bool isOk = value.tryCast(newData);
					assert(isOk);
					assert(data == newData);
				}
			}
		}
		else
		{
			assert(false);
		}
	}
#endif
};

PLG_CALLBACK_FUNC(CustomXmlSerializationPlugin)
} // end namespace wgt
