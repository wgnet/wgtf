#pragma once
#include "property_accessor_listener.hpp"
#include "reflection_dll.hpp"

namespace wgt
{
struct REFLECTION_DLL MetaCallbackPropertyAccessorListener : public PropertyAccessorListener
{
	void postSetValue(const PropertyAccessor& accessor, const Variant& value) override;
};
} // end namespace wgt
