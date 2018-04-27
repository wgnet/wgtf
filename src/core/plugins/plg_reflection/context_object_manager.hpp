#pragma once

#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/ref_object_id.hpp"

namespace wgt
{
class ObjectManager;
class ObjectStorage;
class RootObjectReference;

class ContextObjectManager : public Implements<IObjectManager>
{
public:
	ContextObjectManager(const wchar_t* contextName);
	~ContextObjectManager();
	void init(ObjectManager* baseObjectManager, std::function<void(void)> onContextDestroy);

private:
	virtual ManagedObjectPtr createGenericObject(ObjectHandleStoragePtr storage, RefObjectId id = RefObjectId::zero()) const override;
	virtual ObjectHandleStoragePtr createObjectStorage(const Variant& variant) const override;
    virtual ObjectHandleStoragePtr createObjectStorage(std::unique_ptr<GenericObject> object) const override;
	virtual void updateObjectStorage(const std::shared_ptr<ObjectReference>& reference, const Variant& value) const override;
	virtual void clearStorageRecursively(const std::shared_ptr<ObjectReference>& reference) const override;

	virtual ObjectHandle getObject(const RefObjectId& id) const override;
	virtual ObjectHandle getObject(const void* pObj) const override;
	virtual std::shared_ptr<ObjectReference> getObject(const RefObjectId& id, const std::string& path) override;
	virtual bool getContextObjects(std::vector<RefObjectId>& o_objects,
	                               IObjectManager* context = nullptr) const override;

	virtual ObjectTuple registerObject(const ObjectHandleStoragePtr& storage, const RefObjectId& id) override;
	virtual bool unregisterObject(const RefObjectId& id) override;
	virtual bool registerContext(IObjectManager* context = nullptr) override;
	virtual bool deregisterContext(IObjectManager* context = nullptr) override;
	virtual void registerListener(IObjectManagerListener* listener) override;
	virtual void deregisterListener(IObjectManagerListener* listener) override;
	virtual bool saveObjects(ISerializer& serializer, IObjectManager* context = nullptr) override;
	virtual bool loadObjects(ISerializer& serializer) override;
	virtual void addObjectLinks(const std::string& objId, const IBasePropertyPtr& property,
	                            const ObjectHandle& parent) override;

	IObjectManager* context_ = nullptr;
	ObjectManager* baseManager_ = nullptr;
	std::function<void(void)> onContextDestroy_ = nullptr;
	const std::wstring contextName_;
};
} // end namespace wgt
