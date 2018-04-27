#pragma once

#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include <memory>
#include <unordered_map>

namespace wgt
{
class DefinitionManager;
class ObjectManager;
class IObjectManager;
class IDefinitionManager;
class ReflectionSystemHolder;
class ContextDefinitionManager;
class ContextObjectManager;
class MetaTypeCreator;
class IMetaTypeCreator;
class PropertyAccessorListener;

class ContextObjectManagerCreator : public Implements<IComponentContextCreator>
{
public:
	ContextObjectManagerCreator(ReflectionSystemHolder& holder);
	const char* getType() const;
	InterfacePtr createContext(const wchar_t* contextId);

private:
	ReflectionSystemHolder& holder_;
};

class ContextDefinitionManagerCreator : public Implements<IComponentContextCreator>
{
public:
	ContextDefinitionManagerCreator(ReflectionSystemHolder& holder);
	const char* getType() const;
	InterfacePtr createContext(const wchar_t* contextId);

private:
	ReflectionSystemHolder& holder_;
};

class ReflectionSystemHolder
{
public:
	ReflectionSystemHolder();
	~ReflectionSystemHolder();
    void finalise(std::shared_ptr<PropertyAccessorListener> listener);

	IMetaTypeCreator* getGlobalMetaTypeCreator();

	DefinitionManager* getGlobalDefinitionManager();
    void onCreateDefinitionManagerContext(const std::wstring& contextId, IDefinitionManager* manager);
    void onDestroyDefinitionManagerContext(const std::wstring& contextId);
	ContextDefinitionManagerCreator* contextDefinitionManagerCreator();

	ObjectManager* getGlobalObjectManager();
	ContextObjectManagerCreator* contextObjectManagerCreator();
	void onCreateObjectManagerContext(const std::wstring& contextId, IObjectManager* manager);
	void onDestroyObjectManagerContext(const std::wstring& contextId);
	IObjectManager* getContextObjectManager(const std::wstring& contextId);

private:
	std::unique_ptr<MetaTypeCreator> globalMetaTypeCreator_;
	std::unique_ptr<ObjectManager> globalObjectManager_;
	std::unique_ptr<ContextObjectManagerCreator> contextObjManagerCreator_;
	std::unique_ptr<DefinitionManager> globalDefinitionManager_;
	std::unique_ptr<ContextDefinitionManagerCreator> contextDefManagerCreator_;
	std::unordered_map<std::wstring, IObjectManager*> objectManagerContexts_;
    std::unordered_map<std::wstring, IDefinitionManager*> definitionManagerContexts_;
};
}
