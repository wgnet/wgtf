#ifndef OBJECT_MANAGER_HPP
#define OBJECT_MANAGER_HPP

#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <mutex>
#include "core_object/i_object_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_serialization/serializer/i_serializer.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{
class IDefinitionManager;
struct ObjectMetaData;
typedef std::unordered_set<ObjectMetaData*> ObjIdSet;

/**
 *	Default implementation of IObjectManager.
 *
 *	Note that ObjectManager holds strong references to "unmanaged" objects.
 *	So the owner must unregister the object with ObjectManager before deletion.
 */
class ObjectManager : public Implements<IObjectManager>
{
public:
	ObjectManager();
	virtual ~ObjectManager();
	void init(IDefinitionManager* pDefManager);

	virtual ObjectHandle getObject(const RefObjectId& id) const override;
	virtual ObjectHandle getObject(const void* pObj) const override;
	virtual std::shared_ptr<ObjectReference> getObject(const RefObjectId& id, const std::string& path) override;
	virtual bool getContextObjects(std::vector<RefObjectId>& o_objects, IObjectManager* context) const override;
	virtual ObjectTuple registerObject(const ObjectHandleStoragePtr& storage, const RefObjectId& id) override;
	virtual bool unregisterObject(const RefObjectId& refId) override;
	virtual bool registerContext(IObjectManager* context) override;
	virtual bool deregisterContext(IObjectManager* context) override;
	virtual void registerListener(IObjectManagerListener* listener) override;
	virtual void deregisterListener(IObjectManagerListener* listener) override;
	virtual bool saveObjects(ISerializer& serializer, IObjectManager* context) override;
	virtual bool loadObjects(ISerializer& serializer) override;
	virtual void addObjectLinks(const std::string& objId, const IBasePropertyPtr& property,
	                            const ObjectHandle& parent) override;

	virtual ManagedObjectPtr createGenericObject(ObjectHandleStoragePtr storage, RefObjectId id = RefObjectId::zero()) const override;
	virtual ObjectHandleStoragePtr createObjectStorage(const Variant& variant) const override;
    virtual ObjectHandleStoragePtr createObjectStorage(std::unique_ptr<GenericObject> object) const override;
	virtual void updateObjectStorage(const std::shared_ptr<ObjectReference>& reference, const Variant& value) const override;
	virtual void clearStorageRecursively(const std::shared_ptr<ObjectReference>& reference) const override;

private:
	void resolveObjectLink(const RefObjectId& objId, const ObjectHandle& object);
	void deregisterMetaData(ObjectMetaData& metaData);
	void NotifyObjectRegistred(const ObjectHandle& handle) const;
	void NotifyObjectDeregistred(const ObjectHandle& handle) const;
	void createRootReference(
		std::shared_ptr<ObjectReference>& reference, const RefObjectId& id, const ObjectStoragePtr& storage);

	// all references to a reflected object
	// maps id to reflected object & its references
	std::unordered_map<const RefObjectId, std::weak_ptr<ObjectMetaData>> idMap_;
	std::unordered_map<const void*, std::shared_ptr<ObjectMetaData>> unmanagedMetaDataMap_;
	mutable std::mutex oldObjectsLock_;

	// all objects of a definition context
	// maps definition context to all its objects
	typedef std::unordered_map<IObjectManager*, std::shared_ptr<ObjIdSet>> ContextObjects;
	ContextObjects contextObjects_;

	IDefinitionManager* pDefManager_;

	typedef std::vector<IObjectManagerListener*> ObjectManagerListener;
	ObjectManagerListener listeners_;

	mutable std::mutex listenersLock_;
	typedef std::pair<IBasePropertyPtr, ObjectHandle> LinkPair;
	std::unordered_map<const RefObjectId, LinkPair> objLink_;
	mutable std::mutex objLinkLock_;

	typedef std::tuple<RefObjectId, std::string> ObjectIdentifier;
	struct ObjectIdentifierHash: public std::unary_function<ObjectIdentifier, uint64_t>
	{
		uint64_t operator()(const ObjectIdentifier& id) const
		{
			uint64_t seed = 0;
			wgt::HashUtilities::combine(seed, std::get<0>(id));
			wgt::HashUtilities::combine(seed, std::get<1>(id));
			return seed;
		}
	};

	struct RefObjectIdHash: public std::unary_function<RefObjectId, uint64_t>
	{
		uint64_t operator()(const RefObjectId& id) const
		{
			return wgt::HashUtilities::compute(id);
		}
	};

	std::unordered_map<RefObjectId, std::set<std::string>, RefObjectIdHash> childReferencePaths_;
	typedef std::unordered_map<ObjectIdentifier, std::weak_ptr<ObjectReference>, ObjectIdentifierHash> ObjectMap;
	static const int kDefaultBucketCount = 262144;
	ObjectMap objects_ = ObjectMap(kDefaultBucketCount);
	mutable std::recursive_mutex objectsLock_;
};
} // end namespace wgt
#endif // OBJECT_MANAGER_HPP
