#include "meta_base.hpp"

#include "meta_utilities_impl.hpp"
#include "core_common/assert.hpp"

namespace wgt
{

//==============================================================================
struct MetaDataStorage
{
	ManagedObjectPtr object_;
	ObjectHandle handle_;
	mutable MetaData next_ = nullptr;
};

//==============================================================================
MetaData::MetaData(const std::nullptr_t& )
	: storage_( nullptr )
{
}


//------------------------------------------------------------------------------
MetaData::MetaData(MetaData&& rhs)
	: storage_( std::move( rhs.storage_ ) )
{
}


//------------------------------------------------------------------------------
MetaData::MetaData(ManagedObjectPtr obj, ObjectHandle & handle)
	: storage_(new MetaDataStorage())
{
	storage_->object_ = std::move(obj);
	storage_->handle_ = handle;
}


//------------------------------------------------------------------------------
MetaData::~MetaData()
{
}


//------------------------------------------------------------------------------
MetaData& MetaData::operator=(MetaData&& rhs)
{
	if (this != &rhs)
	{
		storage_ = std::move(rhs.storage_);
	}
	return *this;
}


//------------------------------------------------------------------------------
bool MetaData::operator==(const std::nullptr_t&) const
{
	return storage_? storage_->object_.get() == nullptr : true;
}


//------------------------------------------------------------------------------
bool MetaData::operator!=(const std::nullptr_t&) const
{
	return !operator==( nullptr );
}


//------------------------------------------------------------------------------
const ObjectHandle & MetaData::getHandle() const
{
	if (storage_ == nullptr)
	{
		static ObjectHandle s_Empty;
		return s_Empty;
	}
	return storage_->handle_;
}


//------------------------------------------------------------------------------
const MetaData & MetaData::next() const
{
	if (storage_ == nullptr)
	{
		static MetaData s_Empty;
		return s_Empty;
	}
	return storage_->next_;
}


//------------------------------------------------------------------------------
void MetaData::setNext(MetaData next) const
{
	TF_ASSERT(storage_ != nullptr);
	storage_->next_ = std::move(next);
}


//------------------------------------------------------------------------------
IMetaUtilities & MetaData::getMetaUtils()
{
	static MetaUtilities s_MetaUtils;
	return s_MetaUtils;
}

} // end namespace wgt