#include "context_definition_manager.hpp"
#include "core_serialization/serializer/i_serializer.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/generic/generic_definition.hpp"

namespace wgt
{
namespace ContextDefinitionManagerDetails
{
enum Flag
{
	IS_COLLECTION = 1
};
} // end namespace KeyframesModelDetails

//==============================================================================
ContextDefinitionManager::ContextDefinitionManager(const wchar_t* contextName)
    : pBaseManager_(NULL), contextName_(contextName)
{
}

//==============================================================================
ContextDefinitionManager::~ContextDefinitionManager()
{
    deregisterDefinitions();

    if (onContextDestroy_)
	{
        onContextDestroy_();
	}
}

//==============================================================================
void ContextDefinitionManager::init(IDefinitionManager* pBaseManager,
                                    std::function<IObjectManager*()> getContextObjManager,
                                    std::function<void(void)> onContextDestroy)
{
	TF_ASSERT(!pBaseManager_ && pBaseManager && getContextObjManager);
	pBaseManager_ = pBaseManager;

	getContextObjManager_ = getContextObjManager;
    onContextDestroy_ = onContextDestroy;
}

//==============================================================================
IDefinitionManager* ContextDefinitionManager::getBaseManager() const
{
	return pBaseManager_;
}

//==============================================================================
IClassDefinition* ContextDefinitionManager::getDefinition(const char* name) const
{
	TF_ASSERT(pBaseManager_);
	return pBaseManager_->getDefinition(name);
}

//==============================================================================
IClassDefinition* ContextDefinitionManager::findDefinition(const char* name) const
{
	TF_ASSERT(pBaseManager_);
	return pBaseManager_->findDefinition(name);
}

//==============================================================================
IClassDefinition* ContextDefinitionManager::getObjectDefinition(const ObjectHandle& object) const
{
	TF_ASSERT(pBaseManager_);
	return pBaseManager_->getObjectDefinition(object);
}

//==============================================================================
IClassDefinition* ContextDefinitionManager::registerDefinition(std::unique_ptr<IClassDefinitionDetails> defDetails)
{
	TF_ASSERT(defDetails);
	TF_ASSERT(pBaseManager_);
	IClassDefinitionModifier* modifier = nullptr;
	auto definition = pBaseManager_->registerDefinition(std::move(defDetails));
	if (definition)
	{
		definition->setDefinitionManager(this);
		contextDefinitions_.insert(definition);
	}
	return definition;
}

//==============================================================================
bool ContextDefinitionManager::deregisterDefinition(const IClassDefinition* definition)
{
	TF_ASSERT(definition);
	TF_ASSERT(pBaseManager_);
	auto it = contextDefinitions_.find(definition);
	TF_ASSERT(it != contextDefinitions_.end());
	if (it == contextDefinitions_.end())
	{
		return false;
	}
	contextDefinitions_.erase(it);
	auto ok = pBaseManager_->deregisterDefinition(definition);
	return ok;
}

//==============================================================================
void ContextDefinitionManager::deregisterDefinitions()
{
    for (auto it = contextDefinitions_.begin(); it != contextDefinitions_.end();)
    {
        auto preIt = it;
        preIt++;
        auto definition = *it;
        deregisterDefinition(definition);
        it = preIt;
    }
    contextDefinitions_.clear();
}

//==============================================================================
void ContextDefinitionManager::getDefinitionsOfType(const IClassDefinition* definition,
                                                    std::vector<IClassDefinition*>& o_Definitions) const
{
	TF_ASSERT(pBaseManager_);
	pBaseManager_->getDefinitionsOfType(definition, o_Definitions);
}

//==============================================================================
void ContextDefinitionManager::getDefinitionsOfType(const std::string& type,
                                                    std::vector<IClassDefinition*>& o_Definitions) const
{
	TF_ASSERT(pBaseManager_);
	pBaseManager_->getDefinitionsOfType(type, o_Definitions);
}

//==============================================================================
IObjectManager* ContextDefinitionManager::getObjectManager() const
{
	if (!contextObjManager_)
	{
		TF_ASSERT(getContextObjManager_);
		contextObjManager_ = getContextObjManager_();
		TF_ASSERT(contextObjManager_);
	}
	return contextObjManager_;
}

//==============================================================================
void ContextDefinitionManager::registerDefinitionHelper(const IDefinitionHelper& helper)
{
	TF_ASSERT(pBaseManager_);
	pBaseManager_->registerDefinitionHelper(helper);
}

//==============================================================================
void ContextDefinitionManager::deregisterDefinitionHelper(const IDefinitionHelper& helper)
{
	TF_ASSERT(pBaseManager_);
	pBaseManager_->deregisterDefinitionHelper(helper);
}

//==============================================================================
void ContextDefinitionManager::registerPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener)
{
	TF_ASSERT(pBaseManager_);
	pBaseManager_->registerPropertyAccessorListener(listener);
}

//==============================================================================
void ContextDefinitionManager::deregisterPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener)
{
	TF_ASSERT(pBaseManager_);
	pBaseManager_->deregisterPropertyAccessorListener(listener);
}

//==============================================================================
const IDefinitionManager::PropertyAccessorListeners& ContextDefinitionManager::getPropertyAccessorListeners() const
{
	TF_ASSERT(pBaseManager_);
	return pBaseManager_->getPropertyAccessorListeners();
}

//==============================================================================
bool ContextDefinitionManager::serializeDefinitions(ISerializer& serializer)
{
	std::set<const IClassDefinition*> genericDefs;
	for (auto& definition : contextDefinitions_)
	{
		if (definition->isGeneric())
		{
			genericDefs.insert(definition);
		}
	}

	serializer.serialize(genericDefs.size());
	for (auto& classDef : genericDefs)
	{
		TF_ASSERT(classDef);
		serializer.serialize(classDef->getName());
		const auto & parentNames = classDef->getParentNames();
		if (parentNames.size() == 0)
		{
			serializer.serialize("");
		}
		else
		{
			bool first = true;
			for (const auto & parentName : parentNames )
			{
				if (first)
				{
					serializer.serialize(parentName);
					first = false;
				}
				else
				{
					serializer.serialize("|");
					serializer.serialize(parentName);
				}
			}
		}

		// write all properties
		std::vector<IBasePropertyPtr> baseProps;
		for (PropertyIterator pi = classDef->directProperties().begin(), end = classDef->directProperties().end();
		     (pi != end); ++pi)
		{
			auto metaData = findFirstMetaData<MetaNoSerializationObj>(*(*pi), *this);
			if (metaData != nullptr)
			{
				continue;
			}
			baseProps.push_back(*pi);
		}
		size_t count = baseProps.size();
		serializer.serialize(count);
		for (auto baseProp : baseProps)
		{
			TF_ASSERT(baseProp);
			serializer.serialize(baseProp->getName());
			auto metaType = MetaType::find(baseProp->getType());
			if (metaType != nullptr)
			{
				serializer.serialize(metaType->name());
			}
			else
			{
				serializer.serialize(baseProp->getType().getName());
			}

			uint32_t flags = 0;
			flags |= baseProp->isCollection() ? ContextDefinitionManagerDetails::IS_COLLECTION : 0;
			serializer.serialize(flags);
		}
	}

	genericDefs.clear();
	return true;
}

//==============================================================================
bool ContextDefinitionManager::deserializeDefinitions(ISerializer& serializer)
{
	// load generic definitions
	size_t count = 0;
	serializer.deserialize(count);
	for (size_t i = 0; i < count; i++)
	{
		std::string defName;
		serializer.deserialize(defName);

		std::string parentDefName;
		serializer.deserialize(parentDefName);
		auto pDef = getDefinition(defName.c_str());
		IClassDefinitionModifier* modifier = nullptr;
		if (!pDef)
		{
			auto definition = registerDefinition(createGenericDefinition(defName.c_str()));
			modifier = definition->getDetails().getDefinitionModifier();
		}

		size_t size = 0;
		serializer.deserialize(size);
		std::string propName;
		std::string typeName;
		uint32_t flags;
		for (size_t j = 0; j < size; j++)
		{
			propName.clear();
			typeName.clear();
			serializer.deserialize(propName);
			serializer.deserialize(typeName);
			serializer.deserialize(flags);
			IBasePropertyPtr property = nullptr;
			auto metaType = MetaType::find(typeName.c_str());
			if (modifier)
			{
				bool isCollection = flags & ContextDefinitionManagerDetails::IS_COLLECTION;
				auto property = modifier->addProperty(
				propName.c_str(), metaType != nullptr ? metaType->typeId().getName() : typeName.c_str(), nullptr,
				isCollection);
				// TF_ASSERT( property );
			}
		}
	}
	return true;
}

//==============================================================================
std::unique_ptr<IClassDefinitionDetails> ContextDefinitionManager::createGenericDefinition(const char* name) const
{
	TF_ASSERT(pBaseManager_);
	return pBaseManager_->createGenericDefinition(name);
}
} // end namespace wgt
