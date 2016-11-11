#include "object_manager.hpp"

#include "property_accessor.hpp"
#include "metadata/meta_types.hpp"
#include "metadata/meta_impl.hpp"
#include "metadata/meta_utilities.hpp"
#include "core_serialization/i_datastream.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "object_handle.hpp"
#include "object_handle_storage_shared.hpp"
#include "interfaces/i_base_property.hpp"

#include <atomic>
#include <cassert>
#include <mutex>

namespace wgt
{
//==============================================================================
ObjectManager::ObjectManager() : pDefManager_(NULL)
{
}

//------------------------------------------------------------------------------
ObjectManager::~ObjectManager()
{
	objLink_.clear();
	std::vector<IDefinitionManager*> contexts;
	{
		std::lock_guard<std::mutex> guard(objectsLock_);
		for (ContextObjects::iterator it = contextObjects_.begin(), end = contextObjects_.end(); it != end; ++it)
		{
			contexts.push_back(it->first);
		}
	}

	for (auto it = contexts.begin(), end = contexts.end(); it != end; ++it)
	{
		deregisterContext(*it);
	}
}

//------------------------------------------------------------------------------
void ObjectManager::init(IDefinitionManager* pDefManager)
{
	assert(pDefManager);
	pDefManager_ = pDefManager;
}

//==============================================================================
ObjectHandle ObjectManager::getObject(const RefObjectId& id) const
{
	std::lock_guard<std::mutex> guard(objectsLock_);

	auto findIt = idMap_.find(id);
	if (findIt == idMap_.end())
	{
		return nullptr;
	}
	return ObjectHandle(findIt->second.lock());
}

//------------------------------------------------------------------------------
ObjectHandle ObjectManager::getObject(const void* pObj) const
{
	std::lock_guard<std::mutex> guard(objectsLock_);
	auto findIt = metaDataMap_.find(pObj);
	if (findIt == metaDataMap_.end())
	{
		return nullptr;
	}
	return ObjectHandle(findIt->second.lock());
}

//------------------------------------------------------------------------------
ObjectHandle ObjectManager::getUnmanagedObject(const void* pObj) const
{
	std::lock_guard<std::mutex> guard(objectsLock_);
	auto findIt = unmanagedMetaDataMap_.find(pObj);
	if (findIt == unmanagedMetaDataMap_.end())
	{
		return nullptr;
	}
	return ObjectHandle(findIt->second);
}

//------------------------------------------------------------------------------
bool ObjectManager::getUnmanagedObjectId(const void* pObj, RefObjectId& id) const
{
	std::lock_guard<std::mutex> guard(objectsLock_);
	auto findIt = unmanagedMetaDataMap_.find(pObj);
	if (findIt == unmanagedMetaDataMap_.end())
	{
		return false;
	}
	id = findIt->second->id_;
	return true;
}

//------------------------------------------------------------------------------
bool ObjectManager::getContextObjects(IDefinitionManager* context, std::vector<RefObjectId>& o_objects) const
{
	std::lock_guard<std::mutex> guard(objectsLock_);
	ContextObjects::const_iterator findIt = contextObjects_.find(context);
	assert(findIt != contextObjects_.end());
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
void ObjectManager::getObjects(std::vector<ObjectHandle>& o_objects) const
{
	std::lock_guard<std::mutex> guard(objectsLock_);
	o_objects.reserve(metaDataMap_.size());
	for (const auto& it : metaDataMap_)
	{
		auto pObj = it.second.lock();
		o_objects.push_back(ObjectHandle(pObj));
	}
}

//------------------------------------------------------------------------------
RefObjectId ObjectManager::registerUnmanagedObject(const ObjectHandle& handle, const RefObjectId& id)
{
	RefObjectId newId = id;
	{
		std::lock_guard<std::mutex> guard(objectsLock_);

		if (newId == RefObjectId::zero())
		{
			newId = RefObjectId::generate();
		}

		auto metaData = std::shared_ptr<ObjectMetaData>(new ObjectMetaData);
		metaData->id_ = newId;
		metaData->handle_ = handle;
		metaData->deregistered_ = false;

		unmanagedMetaDataMap_.insert(std::make_pair(handle.data(), metaData));

		auto insertResult = idMap_.insert(std::make_pair(newId, metaData));
		assert(insertResult.second);
	}
	resolveObjectLink(newId, handle);
	NotifyObjectRegistred(handle);

	return newId;
}

//------------------------------------------------------------------------------
ObjectHandle ObjectManager::registerObject(ObjectHandle& handle, const RefObjectId& id)
{
	RefObjectId newId = id;
	ObjectHandle returnHandle;
	{
		std::lock_guard<std::mutex> guard(objectsLock_);

		if (newId == RefObjectId::zero())
		{
			newId = RefObjectId::generate();
		}

		auto metaData = std::shared_ptr<ObjectMetaData>(new ObjectMetaData, [=](ObjectMetaData* metaData) {
			std::unique_ptr<ObjectMetaData> holder(metaData);
			if (metaData->handle_.isValid() == false)
			{
				assert(metaData->deregistered_);
				return;
			}
			deregisterMetaData(*metaData);
		});
		metaData->id_ = newId;
		metaData->handle_ = handle;
		metaData->deregistered_ = false;

		metaDataMap_.insert(std::make_pair(handle.data(), metaData));

		// TODO: This is an enormous hack. We need to register this object in a context mapped to the
		// definitionManager that the definition belongs to.
		auto contextDefManager = handle.getDefinition(*pDefManager_)->getDefinitionManager();

		auto contextIt = contextObjects_.find(contextDefManager);
		std::shared_ptr<ObjIdSet> objSet;
		if (contextIt == contextObjects_.end())
		{
			objSet = std::make_shared<ObjIdSet>();
			contextObjects_.insert(std::make_pair(contextDefManager, objSet));
		}
		else
		{
			objSet = contextIt->second;
		}
		objSet->insert(metaData.get());
		metaData->context_ = objSet;
		auto insertResult = idMap_.insert(std::make_pair(newId, metaData));
		assert(insertResult.second);
		returnHandle = ObjectHandle(metaData);
	}
	resolveObjectLink(newId, returnHandle);
	NotifyObjectRegistred(returnHandle);

	return returnHandle;
}

//------------------------------------------------------------------------------
ObjectHandle ObjectManager::createObject(const RefObjectId& id, const std::string& classDef) const
{
	assert(pDefManager_);
	if (!pDefManager_)
	{
		return nullptr;
	}

	auto pObj = getObject(id);
	if (pObj != nullptr)
	{
		assert(classDef.empty() || classDef == pObj.getDefinition(*pDefManager_)->getName());
		return pObj;
	}

	const auto pClassDef = pDefManager_->getDefinition(classDef.c_str());
	if (!pClassDef)
	{
		return nullptr;
	}

	pObj = pClassDef->createManagedObject(id);
	assert(pObj != nullptr);

	return pObj;
}

//------------------------------------------------------------------------------
ObjectHandle ObjectManager::create(const std::string& classDef) const
{
	assert(!classDef.empty());
	assert(pDefManager_);
	if (!pDefManager_)
	{
		return NULL;
	}

	const auto pClassDef = pDefManager_->getDefinition(classDef.c_str());
	assert(pClassDef);
	if (!pClassDef)
	{
		return NULL;
	}

	return pClassDef->create();
}

//------------------------------------------------------------------------------
bool ObjectManager::registerContext(IDefinitionManager* context)
{
	std::lock_guard<std::mutex> guard(objectsLock_);
	auto insertIt = contextObjects_.insert(std::make_pair(context, std::make_shared<ObjIdSet>()));
	assert(insertIt.second);
	return insertIt.second;
}

//------------------------------------------------------------------------------
bool ObjectManager::deregisterContext(IDefinitionManager* context)
{
	std::shared_ptr<ObjIdSet> copy;
	{
		std::lock_guard<std::mutex> guard(objectsLock_);

		auto findIt = contextObjects_.find(context);
		assert(findIt != contextObjects_.end());
		if (findIt == contextObjects_.end())
		{
			return false;
		}

		copy.swap(findIt->second);
		contextObjects_.erase(findIt);
	}

	for (auto it = copy->begin(); it != copy->end(); ++it)
	{
		auto& metaData = *it;
		ObjectHandle& handle = metaData->handle_;
		assert(handle.isValid());
		RefObjectId id;
		{
			std::lock_guard<std::mutex> guard(objectsLock_);
			id = metaData->id_;
			idMap_.erase(id);
			metaDataMap_.erase(handle.data());
		}
		metaData->deregistered_ = true;

		NotifyObjectDeregistred(handle);
		handle = nullptr;
	}
	return true;
}

//------------------------------------------------------------------------------
void ObjectManager::registerListener(IObjectManagerListener* listener)
{
	std::lock_guard<std::mutex> lisGuard(listenersLock_);
	assert(std::find(listeners_.begin(), listeners_.end(), listener) == listeners_.end());
	listeners_.push_back(listener);
}

//------------------------------------------------------------------------------
void ObjectManager::deregisterListener(IObjectManagerListener* listener)
{
	std::lock_guard<std::mutex> lisGuard(listenersLock_);
	ObjectManagerListener::iterator it = std::find(listeners_.begin(), listeners_.end(), listener);
	assert(it != listeners_.end());
	listeners_.erase(it);
}

//------------------------------------------------------------------------------
void ObjectManager::deregisterMetaData(ObjectMetaData& metaData)
{
	const ObjectHandle& handle = metaData.handle_;

	std::lock_guard<std::mutex> guard(objectsLock_);

	assert(handle.isValid());

	RefObjectId id = metaData.id_;
	auto numFound = idMap_.erase(id);
	assert(numFound == 1);
	auto context = metaData.context_.lock();
	assert(context != nullptr);
	numFound = context->erase(&metaData);
	assert(numFound == 1);
	numFound = metaDataMap_.erase(handle.data());
	assert(numFound == 1);

	metaData.deregistered_ = true;

	NotifyObjectDeregistred(handle);
}

//------------------------------------------------------------------------------
bool ObjectManager::saveObjects(IDefinitionManager& contextDefinitonManager, ISerializer& serializer)
{
	bool br = false;

	std::vector<RefObjectId> objIdList;
	br = getContextObjects(&contextDefinitonManager, objIdList);
	assert(br);

	std::vector<ObjectHandle> objects;
	for (auto& objid : objIdList)
	{
		auto pObj = getObject(objid);
		const auto& classDef = pObj.getDefinition(contextDefinitonManager);
		auto metaData = findFirstMetaData<MetaNoSerializationObj>(*classDef, contextDefinitonManager);
		if (metaData != nullptr)
		{
			continue;
		}
		assert(pObj != nullptr);
		objects.push_back(pObj);
		assert(br);
	}

	br = serializer.serialize(objects.size());
	for (auto obj : objects)
	{
		br = serializer.serialize(obj);
		assert(br);
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
		assert(br);
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
