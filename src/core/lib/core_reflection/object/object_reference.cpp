#include "object_reference.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
ObjectStorage::ObjectStorage(const ObjectHandleStoragePtr& storage)
	: storage_(storage)
{
}


//------------------------------------------------------------------------------
ObjectStoragePtr ObjectStorage::child(const std::string& path)
{
	auto iterator = children_.find(path);
	return iterator != children_.end() ? iterator->second : nullptr;
}


//------------------------------------------------------------------------------
ObjectStoragePtr ObjectStorage::addChild(
	const std::string & path,
	std::function< ObjectStoragePtr() > storageCreator )
{
	auto& storage = children_[path];
	storage = storageCreator();
	return storage;
}

//------------------------------------------------------------------------------
ObjectHandleStoragePtr ObjectStorage::storage() const
{
	return storage_;
}


//------------------------------------------------------------------------------
void ObjectStorage::setValue(
	const Variant& value,
	const IDefinitionManager& definitionManager,
	std::function< ObjectHandleStoragePtr( const Variant &) > storageCreator)
{
	if (auto handlePtr = value.value<ObjectHandle*>())
	{
		storage_ = handlePtr->storage();
		return;
	}
		
	ObjectHandle handle;
	if (value.tryCast(handle))
	{
		storage_ = reflectedRoot(handle, definitionManager).storage();
		return;
	}
	setValue(storageCreator(value));
}


//------------------------------------------------------------------------------
void ObjectStorage::setValue(const ObjectHandleStoragePtr& value)
{
	storage_ = value;
}

//------------------------------------------------------------------------------
Connection ObjectStorage::connectDestroyed(IObjectHandleStorage::StorageDestroyedCallback cb)
{
	if(storage_)
	{
		storage_->connectDestroyed(cb);
	}
	return Connection();
}


//------------------------------------------------------------------------------
bool ObjectStorage::operator==(const ObjectStorage & other) const
{
	if (storage_ == other.storage_)
	{
		if (storage_ == nullptr)
		{
			return false;
		}
		return *storage_ == *other.storage_;
	}
	if (storage_ == nullptr || other.storage_ == nullptr)
	{
		return false;
	}
	return *storage_ == *other.storage_;
}


//==============================================================================
ObjectReference::ObjectReference(const ObjectStoragePtr& storage)
	: storage_(storage)
{
}


//------------------------------------------------------------------------------
ObjectStoragePtr ObjectReference::storage() const
{
	return storage_.lock();
}


//------------------------------------------------------------------------------
void ObjectReference::setStorage(const ObjectStoragePtr& storage)
{
	TF_ASSERT(storage_.use_count() == 0);
	storage_ = storage;
}


//------------------------------------------------------------------------------
bool ObjectReference::isValid() const 
{
	if (auto storage = this->storage())
	{
		auto innerStorage = storage->storage();
		return innerStorage != nullptr ? innerStorage->isValid() : false;
	}

	return false;
}


//------------------------------------------------------------------------------
IObjectHandleProvider* ObjectReference::provider() const 
{
    if (auto storage = this->storage())
    {
        auto innerStorage = storage->storage();
        return innerStorage != nullptr ? innerStorage->provider() : nullptr;
    }
    return nullptr;
}


//------------------------------------------------------------------------------
void* ObjectReference::data() const 
{
	if (auto storage = this->storage())
	{
		auto innerStorage = storage->storage();
		return innerStorage != nullptr ? innerStorage->data() : nullptr;
	}

	return nullptr;
}


//------------------------------------------------------------------------------
TypeId ObjectReference::type() const 
{
	if (auto storage = this->storage())
	{
		auto innerStorage = storage->storage();
		return innerStorage != nullptr ? innerStorage->type() : nullptr;
	}

	return TypeId();
}

//------------------------------------------------------------------------------
Connection ObjectReference::connectDestroyed(StorageDestroyedCallback cb)
{
	if (auto storage = this->storage())
	{
		return storage->connectDestroyed(cb);
	}
	return Connection();
}

//------------------------------------------------------------------------------
ObjectHandleStoragePtr ObjectReference::parent() const 
{
	return parentReference();
}


//------------------------------------------------------------------------------
std::string ObjectReference::fullPath() const
{
	std::string path;
	getFullPath(path);
	return path;
}

//------------------------------------------------------------------------------
std::shared_ptr<RootObjectReference> ObjectReference::rootReference(const std::shared_ptr<ObjectReference>& reference)
{
	TF_ASSERT(reference);
	std::shared_ptr<ObjectReference> parent = reference->parentReference();
	std::shared_ptr<ObjectReference> root = reference;

	while (parent)
	{
		root = parent;
		parent = parent->parentReference();
	}

	return std::static_pointer_cast<RootObjectReference>(root);
}


//------------------------------------------------------------------------------
ObjectHandle ObjectReference::asHandle(const std::shared_ptr<ObjectReference>& reference)
{
	return ObjectHandle(std::static_pointer_cast<IObjectHandleStorage>(reference));
}


//------------------------------------------------------------------------------
void ObjectReference::setParent(const ObjectHandleStoragePtr& parent, const std::string& path) 
{
}


//------------------------------------------------------------------------------
void * ObjectReference::getStoragePtr() const
{
	ObjectStoragePtr storage = storage_.lock();

	if (!storage)
	{
		return nullptr;
	}

	return storage->storage()->data();
}

//------------------------------------------------------------------------------
bool ObjectReference::operator==(const IObjectHandleStorage & other) const
{
	if (auto pOther = dynamic_cast<const ObjectReference*>(&other))
	{
		return operator==(*pOther);
	}
	return false;
}

//------------------------------------------------------------------------------
bool ObjectReference::operator==(const ObjectReference & other) const
{
	ObjectStoragePtr storageLeft = storage_.lock();
	ObjectStoragePtr storageRight = other.storage_.lock();
	if (storageLeft == storageRight)
	{
		if (storageLeft == nullptr)
		{
			return false;
		}
		return *storageLeft == *storageRight;
	}
	if (storageLeft == nullptr || storageRight == nullptr)
	{
		return false;
	}
	return *storageLeft == *storageRight;
}


//==============================================================================
RootObjectReference::RootObjectReference(const RefObjectId& id, const ObjectStoragePtr& storage)
	: id_(id), ObjectReference(storage)
{
}


//------------------------------------------------------------------------------
std::string RootObjectReference::path() const 
{
	return "";
}


//------------------------------------------------------------------------------
void RootObjectReference::getFullPath(std::string& path) const 
{
	path.clear();
}


//------------------------------------------------------------------------------
const RefObjectId& RootObjectReference::id() const 
{
	return id_;
}


//------------------------------------------------------------------------------
std::shared_ptr<ObjectReference> RootObjectReference::parentReference() const 
{
	return nullptr;
}


//------------------------------------------------------------------------------
void RootObjectReference::setStorage(const ObjectStoragePtr& storage)
{
	TF_ASSERT(storage_.expired());
	storage_ = storage;
}


//------------------------------------------------------------------------------
bool RootObjectReference::operator==(const IObjectHandleStorage & other) const
{
	auto pOther = dynamic_cast<const RootObjectReference * >( &other );
	if (pOther == nullptr)
	{
		return false;
	}
	if (id_ != pOther->id_)
	{
		return false;
	}
	return ObjectReference::operator==( *pOther );
}


//------------------------------------------------------------------------------
uint64_t RootObjectReference::getHashcode() const
{
	return id_.getHash();
}


//==============================================================================
ChildObjectReference::ChildObjectReference(
	const std::shared_ptr<ObjectReference>& parent,
	const std::string& path, const ObjectStoragePtr& storage)
	: parent_(parent), path_(path), ObjectReference(storage)
{
}


//------------------------------------------------------------------------------
std::string ChildObjectReference::path() const 
{
	return path_;
}


//------------------------------------------------------------------------------
void ChildObjectReference::getFullPath(std::string& path) const 
{
	parent_->getFullPath(path);

	if (!path.empty() && path_[0] != '[')
	{
		path += ".";
	}

	path += path_;
}


//------------------------------------------------------------------------------
const RefObjectId& ChildObjectReference::id() const 
{
	return parent_->id();
}


//------------------------------------------------------------------------------
std::shared_ptr<ObjectReference> ChildObjectReference::parentReference() const 
{
	return parent_;
}


//------------------------------------------------------------------------------
bool ChildObjectReference::operator==(const IObjectHandleStorage & other) const
{
	auto pOther = dynamic_cast<const ChildObjectReference *>(&other);
	if (pOther == nullptr)
	{
		return false;
	}
	if (parent_ != pOther->parent_)
	{
		if (parent_ == nullptr || pOther == nullptr)
		{
			return false;
		}
		const IObjectHandleStorage * pLeft = parent_.get();
		const IObjectHandleStorage * pRight = pOther->parent_.get();
		if( (*pLeft == *pRight) == false )
		{
			return false;
		}
	}

	std::string path;
	getFullPath(path);

	std::string path2;
	pOther->getFullPath(path2);

	auto equal = path == path2;
	if (equal)
	{
		return ObjectReference::operator==(*pOther);
	}
	return false;
}


//------------------------------------------------------------------------------
uint64_t ChildObjectReference::getHashcode() const
{
	auto hash = id().getHash();
	std::string path;
	getFullPath(path);
	HashUtilities::combine( hash, HashUtilities::compute( path ) ); 
	return hash;
}

} // end namespace wgt
