#include "object_handle_storage.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
ParentedObjectHandleStorage::~ParentedObjectHandleStorage()
{
	destroyedSignal_();
}


//------------------------------------------------------------------------------
Connection ParentedObjectHandleStorage::connectDestroyed(StorageDestroyedCallback cb) 
{
	return destroyedSignal_.connect(cb);
}


//------------------------------------------------------------------------------
void ParentedObjectHandleStorage::setParent(
	const ObjectHandleStoragePtr& parent, const std::string& path) 
{
	TF_ASSERT(this != parent.get());
	parent_ = parent;
	path_ = path;
}


//------------------------------------------------------------------------------
ObjectHandleStoragePtr ParentedObjectHandleStorage::parent() const
{
	return parent_;
}


//------------------------------------------------------------------------------
std::string ParentedObjectHandleStorage::path() const 
{
	return path_;
}


//------------------------------------------------------------------------------
const RefObjectId& ParentedObjectHandleStorage::id() const
{
	return RefObjectId::zero();
}


//------------------------------------------------------------------------------
bool ParentedObjectHandleStorage::operator==(
	const ParentedObjectHandleStorage & other) const
{
	if (path_ != other.path_)
	{
		return false;
	}
	if (parent_ == other.parent_)
	{
		return true;
	}
	if (parent_ == nullptr || other.parent_ == nullptr)
	{
		return false;
	}
	return *parent_ == *other.parent_;
}


//------------------------------------------------------------------------------
bool ParentedObjectHandleStorage::operator==(
	const IObjectHandleStorage & other) const 
{
	auto pOther = dynamic_cast<const ParentedObjectHandleStorage *>(&other);
	if (pOther == nullptr)
	{
		return false;
	}
	return operator==(*pOther);
}


//------------------------------------------------------------------------------
uint64_t ParentedObjectHandleStorage::getHashcode() const 
{
	uint64_t hash = 0;
	if (parent_)
	{
		hash = parent_->getHashcode();
	}
	HashUtilities::combine(hash, HashUtilities::compute(path_));
	return hash;
}

}