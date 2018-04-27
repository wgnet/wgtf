#pragma once

#include "core_common/assert.hpp"
#include "core_object/i_object_handle_provider.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"

namespace wgt
{
template<typename T>
class ObjectHandleProvider : public IObjectHandleProvider
{
public:
    virtual ~ObjectHandleProvider() {};

	const ObjectHandleT<T>& handle() const
	{
		return handle_;
	}
	
private:
    virtual void setHandle(const ObjectHandle& handle, const IDefinitionManager& manager) override
    {
        handle_ = reflectedCast<T>(handle, manager);
        TF_ASSERT(handle_ != nullptr);
    }

	ObjectHandleT<T> handle_;
};
} // end namespace wgt
