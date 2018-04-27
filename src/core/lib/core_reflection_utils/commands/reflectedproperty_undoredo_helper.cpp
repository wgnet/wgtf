#include "reflectedproperty_undoredo_helper.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/utilities/reflection_method_utilities.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_serialization/serializer/i_serializer.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_logging/logging.hpp"
#include <thread>

namespace wgt
{
namespace RPURU = ReflectedPropertyUndoRedoUtility;
namespace
{
/**
*	Function object which holds the algorithm for loading new properties
*	into a property cache.
*/
class PropertyCacheFiller
{
public:
	virtual std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& getNext() = 0;
};

/**
*	Function object which will add a new element to the cache and return it.
*/
class PropertyCacheCreator : public PropertyCacheFiller
{
public:
	PropertyCacheCreator(RPURU::UndoRedoHelperList& propertyCache) : propertyCache_(propertyCache)
	{
	}
	std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& getNext() override
	{
		propertyCache_.emplace_back();
		return propertyCache_.back();
	}

private:
	RPURU::UndoRedoHelperList& propertyCache_;
};

/**
*	Function object which will iterate over elements in a cache so they
*	can be overwritten.
*/
class PropertyCacheIterator : public PropertyCacheFiller
{
public:
	PropertyCacheIterator(RPURU::UndoRedoHelperList& propertyCache)
	    : propertyCache_(propertyCache), itr_(propertyCache.begin())
	{
	}

	std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& getNext() override
	{
		TF_ASSERT(itr_ != propertyCache_.end());
		auto& helper = (*itr_);
		++itr_;
		return helper;
	}

private:
	RPURU::UndoRedoHelperList& propertyCache_;
	RPURU::UndoRedoHelperList::iterator itr_;
};

/**
*	Function pointer for getting either pre- or post-values from the
*	property cache.
*/
typedef Variant (*PropertyGetter)(const RPURU::ReflectedPropertyUndoRedoHelper&);

/**
*	Function pointer for setting either pre- or post-values on the
*	property cache.
*/
typedef void (*PropertySetter)(RPURU::ReflectedPropertyUndoRedoHelper&, const Variant&);

/**
*	Function for getting undo data from the property cache.
*	@param helper the cache which contains the undo value.
*	@return the value from the cache.
*/
Variant undoPropertyGetter(const RPURU::ReflectedPropertyUndoRedoHelper& helper)
{
	return helper.preValue_;
}

/**
*	Function for getting redo data from the property cache.
*	@param helper the cache which contains the redo value.
*	@return the value from the cache.
*/
Variant redoPropertyGetter(const RPURU::ReflectedPropertyUndoRedoHelper& helper)
{
	return helper.postValue_;
}

/**
*	Function for setting undo data on the property cache.
*	@param helper the cache on which to set the value.
*	@param value the value to set on the cache.
*/
void undoPropertySetter(RPURU::ReflectedPropertyUndoRedoHelper& helper, const Variant& value)
{
	helper.preValue_ = value;
}

/**
*	Function for setting redo data on the property cache.
*	@param helper the cache on which to set the value.
*	@param value the value to set on the cache.
*/
void redoPropertySetter(RPURU::ReflectedPropertyUndoRedoHelper& helper, const Variant& value)
{
	helper.postValue_ = value;
}

void loadReflectedPropertyError(std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& helper,
                                PropertySetter propertySetter, const std::string& message)
{
	auto propertyHelper = new RPURU::ReflectedPropertyUndoRedoHelper();
	helper.reset(propertyHelper);
	propertySetter(*propertyHelper, Variant("Unknown"));
	std::string logMessage = "Failed to load reflected properties - " + message + "\n";
	NGT_TRACE_MSG(logMessage.c_str());
}

//==============================================================================
bool loadReflectedProperties(PropertyCacheFiller& outPropertyCache, ISerializer& serializer,
                             PropertySetter propertySetter, IObjectManager& objectManager,
                             IDefinitionManager& definitionManager)
{
	size_t propertyCount = 0;
	serializer.deserialize(propertyCount);

	const char* propertyHeaderTag = RPURU::getPropertyHeaderTag();
	const char* methodHeaderTag = RPURU::getMethodHeaderTag();

	for (; propertyCount > 0; --propertyCount)
	{
		auto& helper = outPropertyCache.getNext();

		// read header
		std::string header;
		serializer.deserialize(header);
		bool propertyHeader = header == propertyHeaderTag;
		bool methodHeader = header == methodHeaderTag;

		if (propertyHeader)
		{
			if (helper.get() == nullptr)
			{
				helper.reset(new RPURU::ReflectedPropertyUndoRedoHelper());
			}
			else if (helper->isMethod())
			{
				loadReflectedPropertyError(helper, propertySetter, "invalid header");
				return false;
			}
		}
		else if (methodHeader)
		{
			if (helper.get() == nullptr)
			{
				helper.reset(new RPURU::ReflectedMethodUndoRedoHelper());
			}
			else if (!helper->isMethod())
			{
				loadReflectedPropertyError(helper, propertySetter, "invalid header");
				return false;
			}
		}
		else
		{
			loadReflectedPropertyError(helper, propertySetter, "invalid header");
			return false;
		}

		// read root object id
		std::string id;
		serializer.deserialize(id);

		if (id.empty())
		{
			loadReflectedPropertyError(helper, propertySetter, "invalid ID");
			return false;
		}

		helper->objectId_ = RefObjectId(id);

		// read property fullpath
		serializer.deserialize(helper->path_);
		const auto& fullPath = helper->path_;

		ObjectHandle object = objectManager.getObject(helper->objectId_);
		if (!object.isValid())
		{
			loadReflectedPropertyError(helper, propertySetter, "invalid object");
			return false;
		}

		PropertyAccessor pa = definitionManager.getDefinition(object)->bindProperty(fullPath.c_str(), object);

		if (!pa.isValid())
		{
			loadReflectedPropertyError(helper, propertySetter, "invalid property");
			return false;
		}

		if (propertyHeader)
		{
			Variant value = pa.getValue();
			if (ReflectionUtilities::isStruct(pa))
			{
				auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());

				serializer.deserialize(value);
				propertySetter(*propertyHelper, value);
			}
			else
			{
				auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());

				Variant variant;
				serializer.deserialize(variant);
				propertySetter(*propertyHelper, variant);
			}
		}
		else
		{
			size_t parameterCount;
			serializer.deserialize(parameterCount);

			auto methodHelper = static_cast<RPURU::ReflectedMethodUndoRedoHelper*>(helper.get());
			std::string parameterType;

			while (parameterCount--)
			{
				Variant parameterValue;
				serializer.deserialize(parameterValue);
				methodHelper->parameters_.push_back(parameterValue);
			}

			serializer.deserialize(methodHelper->result_);
		}
	}

	return true;
}

bool applyReflectedMethodUndoRedo(const RPURU::ReflectedClassMemberUndoRedoHelper* helper, PropertyAccessor& accessor,
                                  ObjectHandle& object, bool undo)
{
	auto methodHelper = static_cast<const RPURU::ReflectedMethodUndoRedoHelper*>(helper);
	accessor.invokeUndoRedo(methodHelper->parameters_, methodHelper->result_, undo);
	return true;
}

bool applyReflectedProperty(const RPURU::ReflectedClassMemberUndoRedoHelper* helper, PropertyGetter propertyGetter,
                            PropertyAccessor& accessor)
{
	auto propertyHelper = static_cast<const RPURU::ReflectedPropertyUndoRedoHelper*>(helper);
	const auto& value = propertyGetter(*propertyHelper);
	return accessor.setValue(value);
}

//==============================================================================
bool applyReflectedProperties(const RPURU::UndoRedoHelperList& propertyCache, PropertyGetter propertyGetter,
                              IObjectManager& objectManager, IDefinitionManager& definitionManager, bool undo)
{
	for (const auto& helper : propertyCache)
	{
		// read root object id
		const auto& id = helper->objectId_;

		// read property fullpath
		const auto& fullPath = helper->path_;

		ObjectHandle object = objectManager.getObject(id);
		if (object == nullptr)
		{
			NGT_TRACE_MSG("Failed to apply reflected property - object is null\n");
			return false;
		}
		PropertyAccessor pa(definitionManager.getDefinition(object)->bindProperty(fullPath.c_str(), object));

		TF_ASSERT(pa.isValid());

		if (helper->isMethod())
		{
			applyReflectedMethodUndoRedo(helper.get(), pa, object, undo);
			continue;
		}

		if (!applyReflectedProperty(helper.get(), propertyGetter, pa))
		{
			return false;
		}
	}

	return true;
}

//==============================================================================
bool performReflectedUndoRedo(ISerializer& serializer, PropertyGetter propertyGetter, PropertySetter propertySetter,
                              const char* expectedFormatHeader, IObjectManager& objectManager,
                              IDefinitionManager& definitionManager, bool undo)
{
	std::string formatHeader;
	serializer.deserialize(formatHeader);
	TF_ASSERT(formatHeader == expectedFormatHeader);

	RPURU::UndoRedoHelperList propertyCache;
	PropertyCacheCreator creator(propertyCache);
	const bool loaded = loadReflectedProperties(creator, serializer, propertySetter, objectManager, definitionManager);
	const bool applied = loaded &&
	applyReflectedProperties(propertyCache, propertyGetter, objectManager, definitionManager, undo);
	return applied;
}

} // end namespace

//==============================================================================
void RPURU::resolveProperty(const ObjectHandle& handle, const IClassDefinition& classDef, const char* propertyPath,
                            PropertyAccessor& o_Pa, IDefinitionManager& definitionManager)
{
	o_Pa = definitionManager.getDefinition(handle)->bindProperty(propertyPath, handle);
	if (o_Pa.isValid())
	{
		return;
	}
	const PropertyIteratorRange& props = classDef.allProperties();
	for (PropertyIterator pi = props.begin(); pi != props.end(); ++pi)
	{
		std::string parentPath = pi->getName();
		const PropertyAccessor& prop = classDef.bindProperty(parentPath.c_str(), handle);
		TF_ASSERT(prop.isValid());
		if (prop.getProperty()->isMethod())
		{
			continue;
		}
		const Variant& value = prop.getValue();
		if (value.typeIs<ObjectHandle>())
		{
			ObjectHandle subHandle;
			bool isOk = value.tryCast(subHandle);
			TF_ASSERT(isOk);
			if ((subHandle == nullptr) || (definitionManager.getDefinition(subHandle) == nullptr))
			{
				continue;
			}
			parentPath = parentPath + "." + propertyPath;

			resolveProperty(subHandle, *definitionManager.getDefinition(subHandle), parentPath.c_str(), o_Pa,
			                definitionManager);

			if (o_Pa.isValid())
			{
				return;
			}
		}
	}
}

//==============================================================================
const char* RPURU::getUndoStreamHeaderTag()
{
	static const char* s_ReflectionUndoData = "ReflectionUndoData";
	return s_ReflectionUndoData;
}

//==============================================================================
const char* RPURU::getRedoStreamHeaderTag()
{
	static const char* s_ReflectionRedoData = "ReflectionRedoData";
	return s_ReflectionRedoData;
}

//==============================================================================
const char* RPURU::getPropertyHeaderTag()
{
	static const char* s_ReflectionPropertyData = "ReflectionPropertyData";
	return s_ReflectionPropertyData;
}

//==============================================================================
const char* RPURU::getMethodHeaderTag()
{
	static const char* s_ReflectionMethodData = "ReflectionMethodData";
	return s_ReflectionMethodData;
}

//==============================================================================
bool RPURU::loadReflectedProperties(UndoRedoHelperList& outPropertyCache, ISerializer& undoSerializer,
                                    ISerializer& redoSerializer, IObjectManager& objectManager,
                                    IDefinitionManager& definitionManager)
{
	PropertyCacheCreator pcc(outPropertyCache);
	const bool undoSuccess =
	loadReflectedProperties(pcc, undoSerializer, &undoPropertySetter, objectManager, definitionManager);

	PropertyCacheIterator pci(outPropertyCache);
	const bool redoSuccess =
	loadReflectedProperties(pci, redoSerializer, &redoPropertySetter, objectManager, definitionManager);

	return (undoSuccess && redoSuccess);
}

//==============================================================================
std::string RPURU::resolveContextObjectPropertyPath(const ObjectHandle& contextObject, const char* propertyPath,
                                                    IDefinitionManager& definitionManager)
{
	TF_ASSERT(contextObject != nullptr);
	const auto classDef = definitionManager.getDefinition(contextObject);
	TF_ASSERT(classDef != nullptr);
	std::string tmp = propertyPath;
	std::vector<std::string> paths;
	paths.push_back(tmp);

	char* pch;
	pch = strtok(const_cast<char*>(tmp.c_str()), ".");
	if (pch != NULL)
	{
		pch = strtok(NULL, ".");
		while (pch != NULL)
		{
			paths.push_back(pch);
			pch = strtok(NULL, ".");
		}
	}
	PropertyAccessor pa;
	for (auto& path : paths)
	{
		resolveProperty(contextObject, *classDef, path.c_str(), pa, definitionManager);
		if (pa.isValid())
		{
			break;
		}
	}
	return pa.getFullPath();
}

bool RPURU::performReflectedUndo(ISerializer& serializer, IObjectManager& objectManager,
                                 IDefinitionManager& definitionManager)
{
	return performReflectedUndoRedo(serializer, &undoPropertyGetter, &undoPropertySetter, getUndoStreamHeaderTag(),
	                                objectManager, definitionManager, true);
}

bool RPURU::performReflectedRedo(ISerializer& serializer, IObjectManager& objectManager,
                                 IDefinitionManager& definitionManager)
{
	return performReflectedUndoRedo(serializer, &redoPropertyGetter, &redoPropertySetter, getRedoStreamHeaderTag(),
	                                objectManager, definitionManager, false);
}

void RPURU::saveUndoData(ISerializer& serializer, const ReflectedClassMemberUndoRedoHelper& helper)
{
	if (helper.isMethod())
	{
		auto methodHelper = static_cast<const ReflectedMethodUndoRedoHelper*>(&helper);
		saveUndoData(serializer, *methodHelper);
	}
	else
	{
		auto propertyHelper = static_cast<const ReflectedPropertyUndoRedoHelper*>(&helper);
		saveUndoData(serializer, *propertyHelper);
	}
}

void RPURU::saveRedoData(ISerializer& serializer, const ReflectedClassMemberUndoRedoHelper& helper)
{
	if (helper.isMethod())
	{
		auto methodHelper = static_cast<const ReflectedMethodUndoRedoHelper*>(&helper);
		saveRedoData(serializer, *methodHelper);
	}
	else
	{
		auto propertyHelper = static_cast<const ReflectedPropertyUndoRedoHelper*>(&helper);
		saveRedoData(serializer, *propertyHelper);
	}
}

void RPURU::saveUndoData(ISerializer& serializer, const ReflectedPropertyUndoRedoHelper& helper)
{
	const char* propertyHeaderTag = RPURU::getPropertyHeaderTag();
	// write header
	serializer.serialize(propertyHeaderTag);
	// write root object id
	serializer.serialize(helper.objectId_.toString());
	// write property fullPath
	serializer.serialize(helper.path_);
	// write value
	serializer.serialize(helper.preValue_);
}

void RPURU::saveRedoData(ISerializer& serializer, const ReflectedPropertyUndoRedoHelper& helper)
{
	const char* propertyHeaderTag = RPURU::getPropertyHeaderTag();
	// write header
	serializer.serialize(propertyHeaderTag);
	// write root object id
	serializer.serialize(helper.objectId_.toString());
	// write property fullPath
	serializer.serialize(helper.path_);
	// write value
	serializer.serialize(helper.postValue_);
}

void RPURU::saveUndoData(ISerializer& serializer, const ReflectedMethodUndoRedoHelper& helper)
{
	const char* methodHeaderTag = RPURU::getMethodHeaderTag();
	serializer.serialize(methodHeaderTag);
	serializer.serialize(helper.objectId_.toString());
	serializer.serialize(helper.path_);
	serializer.serialize(helper.parameters_.size());

	for (auto itr = helper.parameters_.cbegin(); itr != helper.parameters_.cend(); ++itr)
	{
		serializer.serialize(*itr);
	}

	serializer.serialize(helper.result_);
}

void RPURU::saveRedoData(ISerializer& serializer, const ReflectedMethodUndoRedoHelper& helper)
{
	const char* methodHeaderTag = RPURU::getMethodHeaderTag();
	serializer.serialize(methodHeaderTag);
	serializer.serialize(helper.objectId_.toString());
	serializer.serialize(helper.path_);
	serializer.serialize(helper.parameters_.size());

	for (auto itr = helper.parameters_.cbegin(); itr != helper.parameters_.cend(); ++itr)
	{
		serializer.serialize(*itr);
	}

	serializer.serialize(helper.result_);
}
} // end namespace wgt
