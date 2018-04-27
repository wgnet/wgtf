#pragma once

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_logging/logging.hpp"
#include "wg_types/base64.hpp"

//#define OUTPUT_DEBUG_TEXT

namespace wgt	
{
const char* GenericPathDivider = "|";

struct PreferenceUtils
{
static bool deserializeDefinitions(IDefinitionManager& manager, const GenericObjectPtr& preference)
{
	bool success = false;
	std::string definitions;
	preference->get("Definitions", definitions);
	if (!definitions.empty())
	{
		std::string buffer;
		Base64::decode(definitions, buffer);
		ResizingMemoryStream stream;
		XMLSerializer serializer(stream, manager);
		stream.setBuffer(buffer);
		success = manager.deserializeDefinitions(serializer);
	}

	if (!success)
	{
		NGT_ERROR_MSG("Failed to load setting definitions: "
			"This can cause some settings to be incorrectly loaded.");
	}
	return success;
}

static bool serializeDefinitions(IDefinitionManager& manager, const GenericObjectPtr& preference)
{
	bool success = false;
	ResizingMemoryStream stream;
	XMLSerializer serializer(stream, manager);
	if (manager.serializeDefinitions(serializer))
	{
		success = preference->set("Definitions", 
			Base64::encode(stream.buffer().c_str(), stream.buffer().length()).c_str());

#ifdef OUTPUT_DEBUG_TEXT
		NGT_LARGE_MSG(stream.buffer().c_str());
#endif
	}

	if(!success)
	{
		NGT_ERROR_MSG("Failed to save setting definitions: "
			"This can cause some settings to be incorrectly loaded.");
	}
	return success;
}

static bool deserialize(IDefinitionManager& manager,
                        const char* id, 
                        const GenericObjectPtr& preference, 
                        ManagedObject<GenericObject>& output, 
                        std::string& errors)
{
	std::string input;
	if (!preference->get(id, input) || input.empty())
	{
		errors += "\nFailed to load " + std::string(id);
		return false;
	}
	std::string buffer;
	Base64::decode(input, buffer);

	ResizingMemoryStream stream;
	stream.setBuffer(buffer);
	XMLSerializer serializer(stream, manager);
	if (!serializer.deserialize(output) || output.getHandleT() == nullptr)
	{
		errors += "\nFailed to deserialize " + std::string(id);
		return false;
	}

	return true;
}

static bool serialize(IDefinitionManager& manager, 
                      const char* id, 
                      GenericObjectPtr& preference, 
                      const ManagedObject<GenericObject>& input, 
                      std::string& errors)
{
	// We serialize all settings to a string instead of passing it 
	// directly to IPreferences. This is to prevent the preferences 
	// from holding reflected objects while plugins are finalizing
	ResizingMemoryStream stream;
	XMLSerializer serializer(stream, manager);
	if (!serializer.serialize(input.getHandleT()) || stream.buffer().empty())
	{
		errors += "\nFailed to serialize " + std::string(id);
		return false;
	}

	if (!preference->set(id, 
		Base64::encode(stream.buffer().c_str(), stream.buffer().length()).c_str()))
	{
		errors += "\nFailed to save " + std::string(id);
		return false;
	}

#ifdef OUTPUT_DEBUG_TEXT
	NGT_LARGE_MSG(stream.buffer().c_str());
#endif

	return true;
}

static int loadPropertyFromGeneric(IDefinitionManager& manager,
                                   IClassDefinition& definition,
                                   IBaseProperty& property,
                                   GenericObjectPtr& generic,
                                   ObjectHandle& object,
                                   std::function<bool(const char*)> loadFn,
                                   std::string& errors,
                                   std::string path = "")
{
	int count = 0;
	if ((!loadFn || loadFn(property.getName())) && !property.isMethod() &&
		findFirstMetaData<MetaNoSerializationObj>(property, manager) == nullptr &&
		findFirstMetaData<MetaActionObj>(property, manager) == nullptr)
	{
		bool defaultLoad = true;
		bool success = false;
		path += property.getName();

		if (property.isCollection())
		{
			Collection collection;
			auto variant = definition.bindProperty(property.getName(), object).getValue();
			if (variant.tryCast(collection))
			{
				ObjectHandle handle;
				for (size_t i = 0; i < collection.size(); ++i)
				{
					const auto index = std::to_string(i);
					const auto element = Collection::getIndexOpen() + index + Collection::getIndexClose();
					variant = definition.bindProperty((property.getName() + element).c_str(), object).getValue();
					if (!variant.tryCast(handle))
					{
						break; // Non-reflected collections use default load
					}

					defaultLoad = false;
					if (auto handleDef = manager.getDefinition(handle))
					{
						auto properties = handleDef->allProperties();
						for (const auto& prop : properties)
						{
							success = true;
							count += loadPropertyFromGeneric(
								manager, *handleDef, *prop, generic, handle, 
								loadFn, errors, path + GenericPathDivider + index + GenericPathDivider);
						}
					}
				}
			}
		}
		else
		{
			ObjectHandle handle;
			auto variant = definition.bindProperty(property.getName(), object).getValue();
			if (variant.tryCast(handle))
			{
				defaultLoad = false;
				if (auto handleDef = manager.getDefinition(handle))
				{
					auto properties = handleDef->allProperties();
					for (const auto& prop : properties)
					{
						success = true;
						count += loadPropertyFromGeneric(
							manager, *handleDef, *prop, generic, handle, 
							loadFn, errors, path + GenericPathDivider);
					}
				}
			}
		}

		if(defaultLoad)
		{
			success = true; // okay if property does not exist
			if(generic->findProperty(path.c_str()).isValid())
			{
				Variant value;
				success = generic->get(path.c_str(), value) && 
					property.set(object, value, manager);
				if(success)
				{
					++count;
				}
			}
		}

		if (!success)
		{
			errors += "\nFailed to load " + path;
		}
	}
	return count;
}

static int savePropertyAsGeneric(IDefinitionManager& manager,
                                 IClassDefinition& definition,
                                 IBaseProperty& property,
                                 ManagedObject<GenericObject>& generic,
                                 ObjectHandle& object,
                                 std::function<bool(const char*)> saveFn,
                                 std::string& errors,
                                 std::string path = "")
{
	int count = 0;
	if ((!saveFn || saveFn(property.getName())) && !property.isMethod() &&
		findFirstMetaData<MetaNoSerializationObj>(property, manager) == nullptr &&
		findFirstMetaData<MetaActionObj>(property, manager) == nullptr)
	{
		bool defaultSave = true;
		bool success = false;
		path += property.getName();

		if (property.isCollection())
		{
			Collection collection;
			auto variant = definition.bindProperty(property.getName(), object).getValue();
			if(variant.tryCast(collection))
			{
				ObjectHandle handle;
				for (size_t i = 0; i < collection.size(); ++i)
				{
					const auto index = std::to_string(i);
					const auto element = Collection::getIndexOpen() + index + Collection::getIndexClose();
					variant = definition.bindProperty((property.getName() + element).c_str(), object).getValue();
					if (!variant.tryCast(handle))
					{
						break; // Non-reflected collections use default save
					}

					defaultSave = false;
					if (auto handleDef = manager.getDefinition(handle))
					{
						success = true;
						auto properties = handleDef->allProperties();
						for (const auto& prop : properties)
						{
							count += savePropertyAsGeneric(
								manager, *handleDef, *prop, generic, handle,
								saveFn, errors, path + GenericPathDivider + index + GenericPathDivider);
						}
					}
				}
			}
		}
		else // Check if ObjectHandle
		{
			ObjectHandle handle;
			auto variant = definition.bindProperty(property.getName(), object).getValue();
			if (variant.tryCast(handle))
			{
				defaultSave = false;
				if (auto handleDef = manager.getDefinition(handle))
				{
					success = true;
					auto properties = handleDef->allProperties();
					for (const auto& prop : properties)
					{
						count += savePropertyAsGeneric(
							manager, *handleDef, *prop, generic, handle,
							saveFn, errors, path + GenericPathDivider);
					}
				}
			}
		}
		
		if(defaultSave)
		{
			auto value = property.get(object, manager);
			if (generic->set(path.c_str(), value))
			{
				success = true;
				++count;
			}
		}
		
		if(!success)
		{
			errors += "\nFailed to save " + path;
		}
	}
	return count;
}

};
} // end namespace wgt