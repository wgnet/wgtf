#include "definition_manager.hpp"

#include "class_definition.hpp"
#include "property_accessor_listener.hpp"
#include "i_object_manager.hpp"
#include "metadata/meta_base.hpp"
#include "interfaces/i_base_property.hpp"
#include "interfaces/i_definition_helper.hpp"
#include "generic/generic_definition.hpp"
#include "generic/generic_definition_helper.hpp"

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

	for (ClassDefCollection::const_iterator it = definitions_.begin(); it != definitions_.end(); ++it)
	{
		delete it->second;
	}
}

//==============================================================================
IClassDefinition* DefinitionManager::getDefinition(const char* name) const
{
	if (name == nullptr)
	{
		return nullptr;
	}

	ClassDefCollection::const_iterator findIt = definitions_.find(name);
	if (findIt != definitions_.end())
	{
		return findIt->second;
	}

	return nullptr;
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
	assert(defDetails);
	IClassDefinition* definition = new ClassDefinition(std::move(defDetails));
	const auto result = definitions_.insert(std::make_pair(definition->getName(), definition));
	assert(result.second && "Duplicate definition overwritten in map.");
	definition->setDefinitionManager(this);

	return definition;
}

//==============================================================================
bool DefinitionManager::deregisterDefinition(const IClassDefinition* definition)
{
	assert(definition);
	ClassDefCollection::iterator it = definitions_.find(definition->getName());
	assert(it != definitions_.end());
	if (it == definitions_.end())
	{
		return false;
	}
	definitions_.erase(it);
	return true;
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
	ClassDefCollection::const_iterator findIt = definitions_.find(baseType.c_str());
	if (findIt == definitions_.end())
	{
		return;
	}
	IClassDefinition* baseDefinition = findIt->second;
	o_Definitions.push_back(baseDefinition);
	getDefinitionsOfType(baseDefinition, o_Definitions, o_Definitions.size());
}

//==============================================================================
void DefinitionManager::getDefinitionsOfType(IClassDefinition* definition,
                                             std::vector<IClassDefinition*>& o_Definitions, size_t startIndex) const
{
	for (ClassDefCollection::const_iterator it = definitions_.begin(); it != definitions_.end(); ++it)
	{
		if (it->second->getParent() == definition)
		{
			o_Definitions.push_back(it->second);
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
	auto it = helpers_.insert(std::make_pair(helper.typeId(), &helper));
	assert(it.second);
}

//==============================================================================
void DefinitionManager::deregisterDefinitionHelper(const IDefinitionHelper& helper)
{
	auto it = helpers_.find(helper.typeId());
	assert(it != helpers_.end());
	helpers_.erase(it);
}

//==============================================================================
void DefinitionManager::registerPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener)
{
	listeners_.push_back(listener);
}

//==============================================================================
void DefinitionManager::deregisterPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener)
{
	listeners_.erase(listener);
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
