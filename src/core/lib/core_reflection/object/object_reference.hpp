#ifndef OBJECT_REFERENCE_HPP
#define OBJECT_REFERENCE_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "core_common/assert.hpp"
#include "core_object/i_object_handle_storage.hpp"
#include "core_object/object_handle_variant_storage.hpp"
#include "core_common/signal.hpp"
#include "core_reflection/object/object_handle.hpp"
#include "core_reflection/reflection_dll.hpp"

namespace wgt
{

class IDefinitionManager;
class RootObjectReference;
class Variant;
using ObjectHandleStoragePtr = std::shared_ptr<class IObjectHandleStorage>;
using ObjectStoragePtr = std::shared_ptr<class ObjectStorage>;

//==============================================================================
class REFLECTION_DLL ObjectStorage
{
public:
	ObjectStorage(const ObjectHandleStoragePtr& storage);

	//Force ObjectHandleVariantStorage to be created in calling translation unit
	template<typename _dummy = void>
	ObjectStorage(const Variant& variant, const IDefinitionManager& definitionManager)
	{
		setValue( variant, definitionManager, []( const Variant & value )
		{
			return std::make_shared<ObjectHandleVariantStorage>(value);
		});
	}

	//Force ObjectStorage to be created in calling translation unit
	template<typename _dummy = void>
	ObjectStoragePtr addChild(const std::string& path, const Variant& value, const IDefinitionManager& definitionManager)
	{
		return addChild(path, [&value, &definitionManager]()
		{
			return std::make_shared<ObjectStorage>(value, definitionManager);
		});
	}

	//Force ObjectStorage to be created in calling translation unit
	ObjectStoragePtr ObjectStorage::addChild(const std::string& path, const ObjectHandleStoragePtr& value)
	{
		return addChild(path, [&value]()
		{
			return std::make_shared<ObjectStorage>(value);
		});
	}

	void clearRecursive()
	{
		for (auto& child: children_)
		{
			child.second->clearRecursive();
		}
		children_.clear();
		storage_.reset();
	}

	ObjectStoragePtr child(const std::string& path);
	ObjectHandleStoragePtr storage() const;
	void setValue(const ObjectHandleStoragePtr& value);
	Connection connectDestroyed(IObjectHandleStorage::StorageDestroyedCallback cb);

	bool operator==(const ObjectStorage & other) const;
private:
	ObjectStorage(ObjectStorage&& rhs) = delete;
	ObjectStorage& operator=(ObjectStorage&& rhs) = delete;
	ObjectStorage(const ObjectStorage& rhs) = delete;
	ObjectStorage& operator=(const ObjectStorage& rhs) = delete;

	void setValue(
		const Variant& value,
		const IDefinitionManager& definitionManager,
		std::function< ObjectHandleStoragePtr( const Variant & ) > storageCreator );
	ObjectStoragePtr addChild(
		const std::string & path,
		std::function< ObjectStoragePtr() > storageCreator );

	ObjectHandleStoragePtr storage_;
	std::unordered_map<std::string, ObjectStoragePtr> children_;
};

//==============================================================================
// TODO: Change to class ObjectReference: public std::enable_shared_from_this after changing ObjectHandle
class REFLECTION_DLL ObjectReference: public IObjectHandleStorage
{
public:
	typedef void StorageSetCallback();

	ObjectReference(const ObjectStoragePtr& storage);
	ObjectStoragePtr storage() const;
	void setStorage(const ObjectStoragePtr& storage);
	virtual bool isValid() const override;
	virtual IObjectHandleProvider* provider() const override;
	virtual void* data() const override;
	virtual TypeId type() const override;
	virtual ObjectHandleStoragePtr parent() const override;
	virtual std::string fullPath() const;
	virtual void getFullPath(std::string& path) const = 0;
	virtual std::shared_ptr<ObjectReference> parentReference() const = 0;
	virtual Connection connectDestroyed(StorageDestroyedCallback cb) override;

	template<class T>
	T* get() const
	{
		static const TypeId castType = TypeId::getType<T>();

		if (castType != type())
		{
			return nullptr;
		}
		return static_cast<T*>( getStoragePtr() );
	}

	// TODO: move from static after class changed to enable_shared_from_this.
	static std::shared_ptr<RootObjectReference> rootReference(const std::shared_ptr<ObjectReference>& reference);

	static ObjectHandle asHandle(const std::shared_ptr<ObjectReference>& reference);

	template<typename T>
	static ObjectHandleT<T> asHandleT(const std::shared_ptr<ObjectReference>& reference)
	{
		static const TypeId thisType = TypeId::getType<T>();
		TF_ASSERT(reference);
		TF_ASSERT(reference->type() == thisType);
		return ObjectHandleT<T>(std::static_pointer_cast<IObjectHandleStorage>(reference));
	}

protected:
	std::weak_ptr<ObjectStorage> storage_;
	bool operator==(const ObjectReference & other) const;
	bool operator==(const IObjectHandleStorage & other) const override;

private:
	virtual void setParent(const ObjectHandleStoragePtr& parent, const std::string& path) override;
	void * getStoragePtr() const;
};

//==============================================================================
class REFLECTION_DLL RootObjectReference: public ObjectReference
{
public:
	RootObjectReference(const RefObjectId& id, const ObjectStoragePtr& storage);

	virtual std::string path() const override;
	virtual void getFullPath(std::string& path) const override;
	virtual const RefObjectId& id() const override;
	virtual std::shared_ptr<ObjectReference> parentReference() const override;
	void setStorage(const ObjectStoragePtr& storage);

	bool operator==(const IObjectHandleStorage & other) const override;
	uint64_t getHashcode() const override;
private:
	RefObjectId id_;
};

//==============================================================================
class REFLECTION_DLL ChildObjectReference: public ObjectReference
{
public:
	ChildObjectReference(const std::shared_ptr<ObjectReference>& parent, const std::string& path, const ObjectStoragePtr& storage);

	virtual std::string path() const override;
	virtual void getFullPath(std::string& path) const override;
	virtual const RefObjectId& id() const override;
	virtual std::shared_ptr<ObjectReference> parentReference() const override;

	bool operator==(const IObjectHandleStorage & other) const override;
	uint64_t getHashcode() const override;

private:
	std::shared_ptr<ObjectReference> parent_;
	std::string path_;
};
} // end namespace wgt

#endif //OBJECT_REFERENCE_HPP
