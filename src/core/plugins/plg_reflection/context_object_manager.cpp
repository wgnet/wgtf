#include "context_object_manager.hpp"

#include "core_common/assert.hpp"
#include "core_object/object_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"

namespace wgt
{
ContextObjectManager::ContextObjectManager(const wchar_t* contextName)
    : baseManager_(nullptr), contextName_(contextName)
{
}

ContextObjectManager::~ContextObjectManager()
{
	if (baseManager_)
	{
		baseManager_->deregisterContext(context_);
	}
	if (onContextDestroy_)
	{
		onContextDestroy_();
	}
}

void ContextObjectManager::init(ObjectManager* baseObjectManager, std::function<void(void)> onContextDestroy)
{
	TF_ASSERT(!baseManager_ && baseObjectManager);
	context_ = this;
	onContextDestroy_ = onContextDestroy;
	baseManager_ = baseObjectManager;
	baseManager_->registerContext(context_);
}

ManagedObjectPtr ContextObjectManager::createGenericObject(ObjectHandleStoragePtr storage, RefObjectId id) const
{
	TF_ASSERT(baseManager_);
	return baseManager_->createGenericObject(storage, id);
}

ObjectHandleStoragePtr ContextObjectManager::createObjectStorage(const Variant& variant) const
{
	TF_ASSERT(baseManager_);
	return baseManager_->createObjectStorage(variant);
}

ObjectHandleStoragePtr ContextObjectManager::createObjectStorage(std::unique_ptr<GenericObject> object) const
{
    TF_ASSERT(baseManager_);
    return baseManager_->createObjectStorage(std::move(object));
}

void ContextObjectManager::updateObjectStorage(const std::shared_ptr<ObjectReference>& reference, const Variant& value) const
{
	TF_ASSERT(baseManager_);
	baseManager_->updateObjectStorage(reference, value);
}

void ContextObjectManager::clearStorageRecursively(const std::shared_ptr<ObjectReference>& reference) const
{
	baseManager_->clearStorageRecursively(reference);
}

ObjectHandle ContextObjectManager::getObject(const RefObjectId& id) const
{
	TF_ASSERT(baseManager_);
	return baseManager_->getObject(id);
}

ObjectHandle ContextObjectManager::getObject(const void* pObj) const
{
	TF_ASSERT(baseManager_);
	return baseManager_->getObject(pObj);
}

std::shared_ptr<ObjectReference> ContextObjectManager::getObject(const RefObjectId& id, const std::string& path)
{
	TF_ASSERT(baseManager_);
	return baseManager_->getObject(id, path);
}

bool ContextObjectManager::getContextObjects(std::vector<RefObjectId>& o_objects, IObjectManager* context) const
{
	TF_ASSERT(baseManager_);
	return baseManager_->getContextObjects(o_objects, context ? context : context_);
}

IObjectManager::ObjectTuple ContextObjectManager::registerObject(const ObjectHandleStoragePtr& storage, const RefObjectId& id)
{
	TF_ASSERT(baseManager_);
	return baseManager_->registerObject(storage, id);
}

bool ContextObjectManager::unregisterObject(const RefObjectId& id)
{
	TF_ASSERT(baseManager_);
	return baseManager_->unregisterObject(id);
}

bool ContextObjectManager::registerContext(IObjectManager* context)
{
	TF_ASSERT(baseManager_);
	return baseManager_->registerContext(context ? context : context_);
}

bool ContextObjectManager::deregisterContext(IObjectManager* context)
{
	TF_ASSERT(baseManager_);
	return baseManager_->deregisterContext(context ? context : context_);
}

void ContextObjectManager::registerListener(IObjectManagerListener* listener)
{
	TF_ASSERT(baseManager_);
	baseManager_->registerListener(listener);
}

void ContextObjectManager::deregisterListener(IObjectManagerListener* listener)
{
	TF_ASSERT(baseManager_);
	baseManager_->deregisterListener(listener);
}

bool ContextObjectManager::saveObjects(ISerializer& serializer, IObjectManager* context)
{
	TF_ASSERT(baseManager_);
	return baseManager_->saveObjects(serializer, context ? context : context_);
}

bool ContextObjectManager::loadObjects(ISerializer& serializer)
{
	TF_ASSERT(baseManager_);
	return baseManager_->loadObjects(serializer);
}

void ContextObjectManager::addObjectLinks(const std::string& objId, const IBasePropertyPtr& property,
                                          const ObjectHandle& parent)
{
	TF_ASSERT(baseManager_);
	baseManager_->addObjectLinks(objId, property, parent);
}
} // end namespace wgt
