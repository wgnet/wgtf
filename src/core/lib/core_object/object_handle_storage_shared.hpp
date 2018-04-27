#ifndef OBJECT_HANDLE_STORAGE_SHARED_HPP
#define OBJECT_HANDLE_STORAGE_SHARED_HPP

#include "object_handle_storage.hpp"
#include <unordered_set>
#include <memory>

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
	template<typename T = void>
	ObjectHandleStorage(const std::shared_ptr<ObjectMetaData>& metaData)
	    : metaData_(metaData)
	{
		ENFORCE_OBJECT_HANDLE_CONTEXT
	}

	//--------------------------------------------------------------------------
	virtual bool isValid() const override
	{
		return metaData_ != nullptr;
	}

    //--------------------------------------------------------------------------
    virtual IObjectHandleProvider* provider() const override
    {
        return data() != nullptr ? metaData_->handle_.storage()->provider() : nullptr;
    }

	//--------------------------------------------------------------------------
	virtual void* data() const override
	{
		return metaData_ != nullptr ? metaData_->handle_.data() : nullptr;
	}

	//--------------------------------------------------------------------------
	virtual TypeId type() const override
	{
		return metaData_ != nullptr ? metaData_->handle_.type() : nullptr;
	}

	//--------------------------------------------------------------------------
	virtual const RefObjectId& id() const override
	{
		return metaData_->id_;
	}

	//--------------------------------------------------------------------------
	ObjectHandleStoragePtr parent() const override
	{
		if (metaData_ == nullptr)
		{
			return nullptr;
		}

		return metaData_->handle_.parent().storage();
	}

	//--------------------------------------------------------------------------
	void setParent(const ObjectHandleStoragePtr& storage, const std::string& path) override
	{
		if (metaData_ == nullptr)
		{
			return;
		}

		metaData_->handle_.setParent(ObjectHandle(storage), path);
	}

	//--------------------------------------------------------------------------
	std::string path() const override
	{
		if (metaData_ == nullptr)
		{
			return nullptr;
		}

		return metaData_->handle_.path();
	}

private:
	std::shared_ptr<ObjectMetaData> metaData_;
};
} // end namespace wgt
#endif // OBJECT_HANDLE_STORAGE_SHARED_HPP
