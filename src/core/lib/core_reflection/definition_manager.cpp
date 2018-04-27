#include "definition_manager.hpp"

#include "class_definition.hpp"
#include "property_accessor_listener.hpp"
#include "i_object_manager.hpp"
#include "metadata/meta_base.hpp"
#include "interfaces/i_base_property.hpp"
#include "interfaces/i_definition_helper.hpp"
#include "generic/generic_definition.hpp"
#include "generic/generic_definition_helper.hpp"

#include "core_common/assert.hpp"

namespace wgt
{
//==============================================================================
DefinitionManager::DefinitionManager(IObjectManager& objectManager) : objectManager_(objectManager)
{
	genericDefinitionHelper_.reset(new GenericDefinitionHelper());
	registerDefinitionHelper(*genericDefinitionHelper_);
}

//==============================================================================
DefinitionManager::~DefinitionManager()
{
	deregisterDefinitionHelper(*genericDefinitionHelper_);
	genericDefinitionHelper_.reset();

	listeners_.clear();

    deregisterDefinitions();
}


//==============================================================================
IClassDefinition* DefinitionManager::getDefinition(const char* name) const
{
	if (name == nullptr)
	{
		return nullptr;
	}

	wg_read_lock_guard readGuard(lock_);
	auto findIt = definitions_.find(name);
	return findIt != definitions_.end() ? findIt->second : nullptr;
}

//==============================================================================
IClassDefinition* DefinitionManager::findDefinition(const char* name) const
{
	if (name == nullptr)
	{
		return nullptr;
	}

	auto definition = getDefinition(name);
	if (definition)
	{
		return definition;
	}

	auto findFn = [name](const ClassDefCollection::value_type& v) {
		return std::string(v.first.str()).find(name) != std::string::npos;
	};

	wg_read_lock_guard readGuard(lock_);
	auto findIt = std::find_if(definitions_.begin(), definitions_.end(), findFn);
	return findIt != definitions_.end() ? findIt->second : nullptr;
}

//==============================================================================
IClassDefinition* DefinitionManager::getObjectDefinition(const ObjectHandle& object) const
{
	auto typeId = object.type();
	auto definition = getDefinition(typeId.getName());
	if (definition != nullptr)
	{
		return definition;
	}

	wg_read_lock_guard readGuard(helpersLock_);
	auto helperIt = helpers_.find(typeId);
	if (helperIt != helpers_.end())
	{
		definition = helperIt->second->getDefinition(object);
		if (definition != nullptr)
		{
			return definition;
		}
	}

	return nullptr;
}

//==============================================================================
std::unique_ptr<IClassDefinitionDetails> DefinitionManager::createGenericDefinition(const char* name) const
{
	return std::unique_ptr<IClassDefinitionDetails>(new GenericDefinition(name));
}

//==============================================================================
IClassDefinition* DefinitionManager::registerDefinition(std::unique_ptr<IClassDefinitionDetails> defDetails)
{
	TF_ASSERT(defDetails);
	IClassDefinition* definition = new ClassDefinition(std::move(defDetails));
	wg_write_lock_guard writeGuard(lock_);
	const auto result = definitions_.insert(std::make_pair(definition->getName(), definition));
	TF_ASSERT(result.second && "Duplicate definition overwritten in map.");
	definition->setDefinitionManager(this);

	return definition;
}

//==============================================================================
bool DefinitionManager::deregisterDefinition(const IClassDefinition* definition)
{
	TF_ASSERT(definition);
	wg_write_lock_guard writeGuard(lock_);
	ClassDefCollection::iterator it = definitions_.find(definition->getName());
	TF_ASSERT(it != definitions_.end());
	if (it == definitions_.end())
	{
		return false;
	}
    delete it->second;
	definitions_.erase(it);
	return true;
}

//==============================================================================
void DefinitionManager::deregisterDefinitions()
{
	wg_write_lock_guard writeGuard(lock_);
	for (ClassDefCollection::const_iterator it = definitions_.begin(); it != definitions_.end(); ++it)
    {
        delete it->second;
    }
    definitions_.clear();
}

//==============================================================================
void DefinitionManager::getDefinitionsOfType(const IClassDefinition* definition,
                                             std::vector<IClassDefinition*>& o_Definitions) const
{
	getDefinitionsOfType(definition->getName(), o_Definitions);
}

//==============================================================================
void DefinitionManager::getDefinitionsOfType(const std::string& baseType,
                                             std::vector<IClassDefinition*>& o_Definitions) const
{
	wg_read_lock_guard readGuard(lock_);
	ClassDefCollection::const_iterator findIt = definitions_.find(baseType.c_str());
	if (findIt == definitions_.end())
	{
		return;
	}
	IClassDefinition* baseDefinition = findIt->second;
	if (!baseDefinition->getDetails().isAbstract())
	{
		o_Definitions.push_back(baseDefinition);
	}
	getDefinitionsOfType(baseDefinition, o_Definitions, o_Definitions.size());
}

//==============================================================================
void DefinitionManager::getDefinitionsOfType(IClassDefinition* definition,
                                             std::vector<IClassDefinition*>& o_Definitions, size_t startIndex) const
{
	wg_read_lock_guard readGuard(lock_);
	for (ClassDefCollection::const_iterator it = definitions_.begin(); it != definitions_.end(); ++it)
	{
		const auto & parentNames = it->second->getParentNames();
		for (const auto & parentName : parentNames)
		{
			auto def = getDefinition(parentName.c_str());
			if (def == definition)
			{
				o_Definitions.push_back(it->second);
			}
		}
	}

	size_t endIndex = o_Definitions.size();
	for (size_t i = startIndex; i < endIndex; ++i)
	{
		getDefinitionsOfType(o_Definitions[i], o_Definitions, o_Definitions.size());
	}
}

//==============================================================================
void DefinitionManager::registerDefinitionHelper(const IDefinitionHelper& helper)
{
	wg_write_lock_guard writeGuard(helpersLock_);
	auto it = helpers_.insert(std::make_pair(helper.typeId(), &helper));
	TF_ASSERT(it.second);
}

//==============================================================================
void DefinitionManager::deregisterDefinitionHelper(const IDefinitionHelper& helper)
{
	wg_write_lock_guard writeGuard(helpersLock_);
	auto it = helpers_.find(helper.typeId());
	TF_ASSERT(it != helpers_.end());
	helpers_.erase(it);
}

//==============================================================================
void DefinitionManager::registerPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener)
{
	wg_write_lock_guard writeGuard(listenersLock_);
	listeners_.push_back(listener);
}

//==============================================================================
void DefinitionManager::deregisterPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener)
{
	wg_write_lock_guard writeGuard(listenersLock_);
	auto it = std::find(listeners_.begin(), listeners_.end(), listener);
	if (it != listeners_.end())
	{
		listeners_.erase(it);
	}
}

//==============================================================================
const IDefinitionManager::PropertyAccessorListeners& DefinitionManager::getPropertyAccessorListeners() const
{
	return listeners_;
}

//==============================================================================
IObjectManager* DefinitionManager::getObjectManager() const
{
	return &objectManager_;
}

bool DefinitionManager::serializeDefinitions(ISerializer& serializer)
{
	return false;
}
bool DefinitionManager::deserializeDefinitions(ISerializer& serializer)
{
	return false;
}
} // end namespace wgt
