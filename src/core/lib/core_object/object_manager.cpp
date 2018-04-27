#include "object_manager.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/generic/generic_definition.hpp"
#include "core_serialization/i_datastream.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_object/object_handle_storage_shared.hpp"
#include "core_object/object_reference.hpp"
#include "core_logging/logging.hpp"

#include <atomic>
#include <cassert>
#include <mutex>

namespace wgt
{
//==============================================================================
ObjectManager::ObjectManager()
{
	registerContext(this);
}

//------------------------------------------------------------------------------
ObjectManager::~ObjectManager()
{
	objLink_.clear();
	std::vector<IObjectManager*> contexts;
	{
		std::lock_guard<std::mutex> guard(oldObjectsLock_);
		for (ContextObjects::iterator it = contextObjects_.begin(), end = contextObjects_.end(); it != end; ++it)
		{
			contexts.push_back(it->first);
		}
	}

	for (auto it = contexts.begin(), end = contexts.end(); it != end; ++it)
	{
		deregisterContext(*it);
	}

	auto checkStorageEmptyFunction = [](const std::pair<ObjectIdentifier, std::weak_ptr<ObjectReference>>& element)
	{
		auto check = element.second.lock().get();
		TF_ASSERT(!check || !check->storage());
	};

	std::lock_guard<std::recursive_mutex> guard(objectsLock_);
	std::for_each(objects_.begin(), objects_.end(), checkStorageEmptyFunction);
	auto childReferencePaths = childReferencePaths_.begin();
	RefObjectId id;

	while (childReferencePaths != childReferencePaths_.end())
	{
		id = childReferencePaths->first;
		unregisterObject(id);
		childReferencePaths = childReferencePaths_.begin();
	}
}

//------------------------------------------------------------------------------
void ObjectManager::init(IDefinitionManager* pDefManager)
{
	TF_ASSERT(pDefManager);
	pDefManager_ = pDefManager;
}

//==============================================================================
ObjectHandle ObjectManager::getObject(const RefObjectId& id) const
{
	// Hack: this is temparary as this version of getObject should be removed.
	auto writableThis = const_cast<ObjectManager*>(this);
	return ObjectReference::asHandle(writableThis->getObject(id, ""));
}

//------------------------------------------------------------------------------
ObjectHandle ObjectManager::getObject(const void* pObj) const
{
    std::lock_guard<std::recursive_mutex> guard(objectsLock_);

	for (auto& obj : objects_)
    {
		auto object = obj.second.lock();

		if (!object)
		{
			continue;
		}

        if (pObj == object->data())
        {
            return std::static_pointer_cast<IObjectHandleStorage>(obj.second.lock());
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
std::shared_ptr<ObjectReference> ObjectManager::getObject(const RefObjectId& id, const std::string& path)
{
	std::lock_guard<std::recursive_mutex> guard(objectsLock_);
	TF_ASSERT(id != RefObjectId::zero());
	auto objectIdentifier = std::make_tuple(id, path);
	auto& weakReference = objects_[objectIdentifier];
	auto reference = weakReference.lock();

	if (reference)
	{
		return reference;
	}

	childReferencePaths_[id].insert(path);

	if (path.empty())
	{
		createRootReference(reference, id, nullptr);
		weakReference = reference;
		return reference;
	}

	size_t lastDot = path.find_last_of('.');
	std::string parentPath = lastDot != std::string::npos ? path.substr(0, lastDot) : "";
	auto parentReference = getObject(id, parentPath);

	auto position = lastDot + 1;
	std::string childPath = path.substr(position);
	reference = std::make_shared<ChildObjectReference>(parentReference, childPath, nullptr);
	weakReference = reference;
	return reference;
}

//------------------------------------------------------------------------------
bool ObjectManager::getContextObjects(std::vector<RefObjectId>& o_objects, IObjectManager* context) const
{
	if(!context)
	{
		context = const_cast<ObjectManager*>(this);
		NGT_WARNING_MSG("Using Object Manager global context");
	}

	std::lock_guard<std::mutex> guard(oldObjectsLock_);
	ContextObjects::const_iterator findIt = contextObjects_.find(context);
	TF_ASSERT(findIt != contextObjects_.end());
	if (findIt == contextObjects_.end())
	{
		return false;
	}
	for (const auto& it : *findIt->second)
	{
		o_objects.push_back(it->id_);
	}
	return true;
}

//------------------------------------------------------------------------------
IObjectManager::ObjectTuple ObjectManager::registerObject(const ObjectHandleStoragePtr& storage, const RefObjectId& id)
{
	std::lock_guard<std::recursive_mutex> guard(objectsLock_);
    auto objectStorage = std::make_shared<ObjectStorage>(storage);
	RefObjectId refId = id == RefObjectId::zero() ? RefObjectId::generate() : id;
	auto objectIdentifier = std::make_tuple(refId, std::string());
	auto& weakReference = objects_[objectIdentifier];
	auto reference = weakReference.lock();
	childReferencePaths_[refId].insert("");

	if (reference)
	{
		TF_ASSERT(id == RefObjectId::zero() || id == reference->id());
		TF_ASSERT(reference->storage() == nullptr);
		reference->setStorage(objectStorage);
	}
    else
    {
		createRootReference(reference, refId, objectStorage);
		weakReference = reference;
    }

    if (storage && storage->data() && storage->provider())
    {
        TF_ASSERT(pDefManager_);
        auto handle = ObjectReference::asHandle(reference);
        storage->provider()->setHandle(handle, *pDefManager_);
    }

    return std::make_tuple(objectStorage, std::static_pointer_cast<RootObjectReference>(reference));
}

//------------------------------------------------------------------------------
bool ObjectManager::unregisterObject(const RefObjectId& refId)
{
	std::lock_guard<std::recursive_mutex> guard(objectsLock_);
	auto& childPaths = childReferencePaths_[refId];
	ObjectIdentifier key;
	ObjectMap::iterator found;
	bool erased = false;

	for (auto& childPath : childPaths)
	{
		key = std::make_tuple(refId, childPath);
		found = objects_.find(key);

		if (found != objects_.end())
		{
			objects_.erase(found);
			erased = true;
		}
	}

	childReferencePaths_.erase(refId);
	return erased;
}

void ObjectManager::createRootReference(
	std::shared_ptr<ObjectReference>& reference, const RefObjectId& id, const ObjectStoragePtr& storage)
{
	auto deleter = [this](RootObjectReference* pointer)
	{
		unregisterObject(pointer->id());
		delete pointer;
	};

	reference.reset(new RootObjectReference(id, storage), deleter);
}

//------------------------------------------------------------------------------
ManagedObjectPtr ObjectManager::createGenericObject(ObjectHandleStoragePtr storage, RefObjectId id) const
{
	return ManagedObjectPtr(new ManagedObject<GenericObject>(storage, id));
}

//------------------------------------------------------------------------------
ObjectHandleStoragePtr ObjectManager::createObjectStorage(const Variant& variant) const
{
    TF_ASSERT(pDefManager_);
	return ObjectStorage(variant, *pDefManager_).storage();
}

//------------------------------------------------------------------------------
ObjectHandleStoragePtr ObjectManager::createObjectStorage(std::unique_ptr<GenericObject> object) const
{
    return std::make_shared<ObjectHandleStorage<std::unique_ptr<GenericObject>>>(std::move(object));
}

//------------------------------------------------------------------------------
void ObjectManager::updateObjectStorage(const std::shared_ptr<ObjectReference>& reference, const Variant& value) const
{
	TF_ASSERT(reference);
	auto parent = reference->parentReference();
	TF_ASSERT(parent);
	auto childStorage = reference->storage();
	auto handleStorage = createObjectStorage(value);

	if (!childStorage)
	{
		std::shared_ptr<ObjectStorage> parentStorage = parent->storage();
		TF_ASSERT(parentStorage);
		auto child = parentStorage->addChild(reference->path(), handleStorage);
		reference->setStorage(child);
	}
	else
	{
		childStorage->setValue(handleStorage);
	}
}

void ObjectManager::clearStorageRecursively(const std::shared_ptr<ObjectReference>& reference) const
{
	if (auto childStorage = reference->storage())
	{
		childStorage->clearRecursive();
	}
}

//------------------------------------------------------------------------------
bool ObjectManager::registerContext(IObjectManager* context)
{
	std::lock_guard<std::mutex> guard(oldObjectsLock_);
	auto insertIt = contextObjects_.insert(std::make_pair(context, std::make_shared<ObjIdSet>()));
	TF_ASSERT(insertIt.second);
	return insertIt.second;
}

//------------------------------------------------------------------------------
bool ObjectManager::deregisterContext(IObjectManager* context)
{
	std::shared_ptr<ObjIdSet> copy;
	{
		std::lock_guard<std::mutex> guard(oldObjectsLock_);

		auto findIt = contextObjects_.find(context);
		TF_ASSERT(findIt != contextObjects_.end());
		if (findIt == contextObjects_.end())
		{
			return false;
		}

		copy.swap(findIt->second);
		contextObjects_.erase(findIt);
	}

	for (auto it = copy->begin(); it != copy->end(); ++it)
	{
		auto metaData = *it;
		metaData->context_.reset();
		deregisterMetaData(*metaData);
	}

	return true;
}

//------------------------------------------------------------------------------
void ObjectManager::registerListener(IObjectManagerListener* listener)
{
	std::lock_guard<std::mutex> lisGuard(listenersLock_);
	TF_ASSERT(std::find(listeners_.begin(), listeners_.end(), listener) == listeners_.end());
	listeners_.push_back(listener);
}

//------------------------------------------------------------------------------
void ObjectManager::deregisterListener(IObjectManagerListener* listener)
{
	std::lock_guard<std::mutex> lisGuard(listenersLock_);
	ObjectManagerListener::iterator it = std::find(listeners_.begin(), listeners_.end(), listener);
	TF_ASSERT(it != listeners_.end());
	listeners_.erase(it);
}

//------------------------------------------------------------------------------
void ObjectManager::deregisterMetaData(ObjectMetaData& metaData)
{
	ObjectHandle handle = metaData.handle_;
	TF_ASSERT(handle.isValid());
	metaData.handle_ = nullptr;

	TF_ASSERT(!metaData.deregistered_);
	metaData.deregistered_ = true;

	{
		std::lock_guard<std::mutex> guard(oldObjectsLock_);

		RefObjectId id = metaData.id_;
		auto numFound = idMap_.erase(id);
		TF_ASSERT(numFound == 1);

		if(!metaData.context_.expired())
		{
			auto context = metaData.context_.lock();
			TF_ASSERT(context != nullptr);
			numFound = context->erase(&metaData);
			TF_ASSERT(numFound == 1);
		}

		numFound = unmanagedMetaDataMap_.erase(handle.data());
		TF_ASSERT(numFound == 1);
	}

	NotifyObjectDeregistred(handle);
}

//------------------------------------------------------------------------------
bool ObjectManager::saveObjects(ISerializer& serializer, IObjectManager* context)
{
	if(!context)
	{
		context = this;
		NGT_WARNING_MSG("Using Object Manager global context");
	}

	bool br = false;

	std::vector<RefObjectId> objIdList;
	br = getContextObjects(objIdList, context);
	TF_ASSERT(br);

	std::vector<ObjectHandle> objects;
	for (auto& objid : objIdList)
	{
		auto pObj = getObject(objid);
		const auto& classDef = pDefManager_->getDefinition(pObj);
		auto metaData = findFirstMetaData<MetaNoSerializationObj>(*classDef, *pDefManager_);
		if (metaData != nullptr)
		{
			continue;
		}
		TF_ASSERT(pObj != nullptr);
		objects.push_back(pObj);
		TF_ASSERT(br);
	}

	br = serializer.serialize(objects.size());
	for (auto obj : objects)
	{
		br = serializer.serialize(obj);
		TF_ASSERT(br);
	}
	return br;
}

//------------------------------------------------------------------------------
bool ObjectManager::loadObjects(ISerializer& serializer)
{
	bool br = false;
	size_t objCount = 0;
	br = serializer.deserialize(objCount);
	for (size_t j = 0; (j < objCount) && br; j++)
	{
		Variant variant;
		br = serializer.deserialize(variant);
		TF_ASSERT(br);
	}
	return br;
}

//------------------------------------------------------------------------------
void ObjectManager::addObjectLinks(const std::string& objId, const IBasePropertyPtr& property,
                                   const ObjectHandle& parent)
{
	std::lock_guard<std::mutex> objGuard(objLinkLock_);
	LinkPair pair = std::make_pair(property, parent);
	objLink_.insert(std::make_pair(objId, pair));
}

//------------------------------------------------------------------------------
void ObjectManager::resolveObjectLink(const RefObjectId& objId, const ObjectHandle& object)
{
	std::lock_guard<std::mutex> objGuard(objLinkLock_);
	auto findIt = objLink_.find(objId);
	if (findIt != objLink_.end())
	{
		auto property = findIt->second.first;
		auto& parent = findIt->second.second;
		property->set(parent, object, *pDefManager_);
		objLink_.erase(findIt);
	}
}

void ObjectManager::NotifyObjectRegistred(const ObjectHandle& handle) const
{
	std::lock_guard<std::mutex> lisGuard(listenersLock_);
	for (auto& it : listeners_)
	{
		it->onObjectRegistered(handle);
	}
}

void ObjectManager::NotifyObjectDeregistred(const ObjectHandle& handle) const
{
	std::lock_guard<std::mutex> lisGuard(listenersLock_);
	for (auto& it : listeners_)
	{
		it->onObjectDeregistered(handle);
	}
}
} // end namespace wgt
