#ifndef OBJECT_HANDLE_STORAGE_SHARED_HPP
#define OBJECT_HANDLE_STORAGE_SHARED_HPP

#include "object_handle_storage.hpp"
#include "object_handle.hpp"

namespace wgt
{
struct ObjectMetaData;
typedef std::unordered_set<ObjectMetaData*> ObjIdSet;

struct ObjectMetaData
{
	bool deregistered_;
	std::weak_ptr<ObjIdSet> context_;
	ObjectHandle handle_;
	RefObjectId id_;
};

//==============================================================================
template <>
class ObjectHandleStorage<std::shared_ptr<ObjectMetaData>> : public IObjectHandleStorage
{
public:
	//--------------------------------------------------------------------------
	ObjectHandleStorage(const std::shared_ptr<ObjectMetaData>& metaData, const IClassDefinition* definition = nullptr)
	    : metaData_(metaData)
	{
	}

	//--------------------------------------------------------------------------
	void* data() const override
	{
		return metaData_ != nullptr ? metaData_->handle_.data() : nullptr;
	}

	//--------------------------------------------------------------------------
	TypeId type() const override
	{
		return metaData_ != nullptr ? metaData_->handle_.type() : nullptr;
	}

	//--------------------------------------------------------------------------
	bool getId(RefObjectId& o_Id) const override
	{
		o_Id = metaData_->id_;
		return true;
	}

private:
	std::shared_ptr<ObjectMetaData> metaData_;
};
} // end namespace wgt
#endif // OBJECT_HANDLE_STORAGE_SHARED_HPP
