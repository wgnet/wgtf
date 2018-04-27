#include "reflection_system_holder.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_object/object_manager.hpp"
#include "core_reflection/metadata/meta_type_creator.hpp"
#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "context_definition_manager.hpp"
#include "context_object_manager.hpp"

namespace wgt
{
ReflectionSystemHolder::ReflectionSystemHolder()
    : globalObjectManager_(new ObjectManager()), globalDefinitionManager_(new DefinitionManager(*globalObjectManager_)),
      contextDefManagerCreator_(new ContextDefinitionManagerCreator(*this)),
      contextObjManagerCreator_(new ContextObjectManagerCreator(*this)),
	  globalMetaTypeCreator_(new MetaTypeCreator())
{
	globalObjectManager_->init(globalDefinitionManager_.get());
}

ReflectionSystemHolder::~ReflectionSystemHolder()
{
	globalMetaTypeCreator_.reset();
	globalObjectManager_.reset();
	globalDefinitionManager_.reset();
	contextDefManagerCreator_.reset();
	contextObjManagerCreator_.reset();
}

void ReflectionSystemHolder::finalise(std::shared_ptr<PropertyAccessorListener> listener)
{
    globalDefinitionManager_->deregisterPropertyAccessorListener(listener);

    for (auto context : definitionManagerContexts_)
    {
        context.second->deregisterDefinitions();
    }

    globalDefinitionManager_->deregisterDefinitions();
}

IMetaTypeCreator* ReflectionSystemHolder::getGlobalMetaTypeCreator()
{
	return globalMetaTypeCreator_.get();
}

DefinitionManager* ReflectionSystemHolder::getGlobalDefinitionManager()
{
	return globalDefinitionManager_.get();
}

ContextDefinitionManagerCreator* ReflectionSystemHolder::contextDefinitionManagerCreator()
{
	return contextDefManagerCreator_.get();
}

ObjectManager* ReflectionSystemHolder::getGlobalObjectManager()
{
	return globalObjectManager_.get();
}

ContextObjectManagerCreator* ReflectionSystemHolder::contextObjectManagerCreator()
{
	return contextObjManagerCreator_.get();
}

IObjectManager* ReflectionSystemHolder::getContextObjectManager(const std::wstring& contextId)
{
	return objectManagerContexts_.find(contextId) != objectManagerContexts_.end() ?
	objectManagerContexts_.at(contextId) :
	nullptr;
}

void ReflectionSystemHolder::onCreateObjectManagerContext(const std::wstring& contextId, IObjectManager* manager)
{
	auto r = objectManagerContexts_.insert(std::pair<std::wstring, IObjectManager*>(contextId, manager));
	TF_ASSERT(r.second);
}

void ReflectionSystemHolder::onDestroyObjectManagerContext(const std::wstring& contextId)
{
	objectManagerContexts_.erase(contextId);
}

void ReflectionSystemHolder::onCreateDefinitionManagerContext(const std::wstring& contextId, IDefinitionManager* manager)
{
    auto r = definitionManagerContexts_.insert(std::pair<std::wstring, IDefinitionManager*>(contextId, manager));
    TF_ASSERT(r.second);
}

void ReflectionSystemHolder::onDestroyDefinitionManagerContext(const std::wstring& contextId)
{
    definitionManagerContexts_.erase(contextId);
}

ContextObjectManagerCreator::ContextObjectManagerCreator(ReflectionSystemHolder& holder) : holder_(holder)
{
}

const char* ContextObjectManagerCreator::getType() const
{
	return typeid(ContextObjectManager).name();
}

InterfacePtr ContextObjectManagerCreator::createContext(const wchar_t* contextId)
{
	const std::wstring id = contextId;
	auto onContextDestroy = [this, id]() { holder_.onDestroyObjectManagerContext(id); };

	std::unique_ptr<ContextObjectManager> contextManager(new ContextObjectManager(contextId));
	holder_.onCreateObjectManagerContext(contextId, contextManager.get());
	contextManager->init(holder_.getGlobalObjectManager(), onContextDestroy);
	return std::make_shared<InterfaceHolder<ContextObjectManager>>(contextManager.release(), true);
}

ContextDefinitionManagerCreator::ContextDefinitionManagerCreator(ReflectionSystemHolder& holder) : holder_(holder)
{
}

const char* ContextDefinitionManagerCreator::getType() const
{
	return typeid(ContextDefinitionManager).name();
}

InterfacePtr ContextDefinitionManagerCreator::createContext(const wchar_t* contextId)
{
	const std::wstring id = contextId;
	auto getContextObjectManager = [this, id]() { return holder_.getContextObjectManager(id); };
    auto onContextDestroy = [this, id]() { holder_.onDestroyDefinitionManagerContext(id); };

	std::unique_ptr<ContextDefinitionManager> contextManager(new ContextDefinitionManager(contextId));
    holder_.onCreateDefinitionManagerContext(contextId, contextManager.get());
	contextManager->init(holder_.getGlobalDefinitionManager(), getContextObjectManager, onContextDestroy);
	return std::make_shared<InterfaceHolder<ContextDefinitionManager>>(contextManager.release(), true);
}
}
