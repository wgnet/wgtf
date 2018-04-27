#ifndef PROPERTY_ACCESSOR_LISTENER_HPP
#define PROPERTY_ACCESSOR_LISTENER_HPP

#include "core_variant/collection.hpp"
#include "reflection_dll.hpp"

namespace wgt
{
class Variant;
class PropertyAccessor;
class ReflectedMethodParameters;

class REFLECTION_DLL PropertyAccessorListener
{
public:
	virtual void preSetValue(const PropertyAccessor& accessor, const Variant& value)
	{
	}
	virtual void postSetValue(const PropertyAccessor& accessor, const Variant& value)
	{
	}

	virtual void preInvoke(const PropertyAccessor& accessor, const ReflectedMethodParameters& parameters, bool undo)
	{
	}
	virtual void postInvoke(const PropertyAccessor& accessor, Variant result, bool undo)
	{
	}

	virtual void preInsert(const PropertyAccessor& accessor, size_t index, size_t count)
	{
	}
	virtual void postInserted(const PropertyAccessor& accessor, size_t index, size_t count)
	{
	}

	virtual void preErase(const PropertyAccessor& accessor, size_t index, size_t count)
	{
	}
	virtual void postErased(const PropertyAccessor& accessor, size_t index, size_t count)
	{
	}
};
} // end namespace wgt
#endif // PROPERTY_ACCESSOR_LISTENER_HPP
