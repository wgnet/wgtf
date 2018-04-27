#include "object_handle.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_object/i_object_handle_storage.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
ObjectHandleBase::ObjectHandleBase()
	: storage_(nullptr)
{
}

//------------------------------------------------------------------------------
ObjectHandleBase::ObjectHandleBase(const ObjectHandleStoragePtr& storage, CastType castType)
	: storage_(storage)
	, castType_(castType)
{
}


//------------------------------------------------------------------------------
ObjectHandleBase::ObjectHandleBase(const ObjectHandleBase& other)
	: storage_(other.storage_)
	, castType_(other.castType_)
{
}

//------------------------------------------------------------------------------
ObjectHandleBase::ObjectHandleBase(ObjectHandleBase&& other)
	: storage_(std::move(other.storage_))
	, castType_(std::move(other.castType_))
{
}

//------------------------------------------------------------------------------
Connection ObjectHandleBase::connectStorageDestroyed(StorageDestroyedCallback cb) const
{
	if(storage_)
	{
		return storage_->connectDestroyed(cb);
	}
	return Connection();
}

//------------------------------------------------------------------------------
uint64_t ObjectHandleBase::getRecursiveHash() const
{
	auto parent = this->parent();
	if (parent.isValid())
	{
		auto hash = parent.getRecursiveHash();
		HashUtilities::combine(hash, HashUtilities::compute(path()));
		return hash;
	}
	return getHash();
}

//------------------------------------------------------------------------------
uint64_t ObjectHandleBase::getHash() const
{
	return id().getHash();
}

//------------------------------------------------------------------------------
RefObjectId ObjectHandleBase::id() const
{
	return storage_ != nullptr ? storage_->id() : RefObjectId::zero();
}


//------------------------------------------------------------------------------
TypeId ObjectHandleBase::type() const
{
	return castType_.type != nullptr ? castType_.type : (storage_ != nullptr ? storage_->type() : nullptr);
}


//------------------------------------------------------------------------------
ObjectHandle ObjectHandleBase::parent() const
{
	return storage_ == nullptr ? ObjectHandle() : storage_->parent();
}


//------------------------------------------------------------------------------
std::string ObjectHandleBase::path() const
{
	return storage_ == nullptr ? "" : storage_->path();
}


//------------------------------------------------------------------------------
void ObjectHandleBase::setParent(const ObjectHandle& parent, const std::string& path)
{
	if (storage_ == nullptr)
	{
		return;
	}

	storage_->setParent(parent.storage(), path);
}


//------------------------------------------------------------------------------
void * ObjectHandleBase::getData(const TypeId & typeId) const
{
	if (storage_ == nullptr || storage_->data() == nullptr)
	{
		return nullptr;
	}

	if (storage_->type() == typeId)
	{
		return storage_->data();
	}
	if (castType_.type == typeId)
	{
		return castType_.cast(storage_->data(), storage_->type());
	}

	return nullptr;
}


//------------------------------------------------------------------------------
ObjectHandleBase & ObjectHandleBase ::operator=(const std::nullptr_t&)
{
	storage_ = nullptr;
	castType_ = nullptr;
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandleBase & ObjectHandleBase::operator=(const ObjectHandleBase & other)
{
	storage_ = other.storage_;
	castType_ = other.castType_;
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandleBase & ObjectHandleBase::operator=(ObjectHandleBase && other)
{
	storage_ = std::move(other.storage_);
	castType_ = std::move(other.castType_);
	return *this;
}


//------------------------------------------------------------------------------
bool ObjectHandleBase::operator==(const ObjectHandleBase& other) const
{
	if (storage_ == other.storage_)
	{
		return true;
	}

	if (storage_ == nullptr || other.storage_ == nullptr)
	{
		return false;
	}

	return *storage_ == *other.storage_;
}


//------------------------------------------------------------------------------
bool ObjectHandleBase::operator<(const ObjectHandleBase & other) const
{
	if (storage_ == other.storage_)
	{
		return false;
	}

	if (storage_ == nullptr)
	{
		return true;
	}

	if (other.storage_ == nullptr)
	{
		return false;
	}

	auto left = storage_->data();
	auto right = other.storage_->data();
	if (left == right)
	{
		return storage_->type() < other.storage_->type();
	}
	return left < right;
}

//==============================================================================
ObjectHandle::ObjectHandle()
	: ObjectHandleBase()
{
}


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle(const std::nullptr_t&)
	: ObjectHandleBase()
{
}


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle(const ObjectHandle& other)
	: ObjectHandleBase(other)
{
}


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle(ObjectHandle&& other)
	: ObjectHandleBase(std::move(other))
{
}


//------------------------------------------------------------------------------
void* ObjectHandle::data() const
{
	return storage_ != nullptr ? storage_->data() : nullptr;
}


//------------------------------------------------------------------------------
bool ObjectHandle::isValid() const
{
	return data() != nullptr;
}


//------------------------------------------------------------------------------
ObjectHandleStoragePtr ObjectHandleBase::storage() const
{
	return storage_;
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator==(const std::nullptr_t&) const
{
	return data() == nullptr;
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator!=(const std::nullptr_t&) const
{
	return data() != nullptr;
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator==(const ObjectHandle& other) const
{
	return ObjectHandleBase::operator==( static_cast< const ObjectHandleBase & >( other ) );
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator!=(const ObjectHandle& other) const
{
	return !operator==(other);
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator<(const ObjectHandle& other) const
{
	return ObjectHandleBase::operator<(other);
}


//------------------------------------------------------------------------------
ObjectHandle& ObjectHandle::operator=(const std::nullptr_t&)
{
	ObjectHandleBase::operator=(nullptr);	
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle& ObjectHandle::operator=(const ObjectHandle& other)
{
	ObjectHandleBase::operator=(other);
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle& ObjectHandle::operator=(ObjectHandle&& other)
{
	ObjectHandleBase::operator=(std::move( other ) );
	return *this;
}

//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle(
	const ObjectHandleStoragePtr& storage, CastType castType )
	: ObjectHandleBase(storage, castType)
{
}

}//namespace wgt

