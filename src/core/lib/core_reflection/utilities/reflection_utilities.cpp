#include "reflection_utilities.hpp"
#include "../property_accessor.hpp"

namespace wgt
{
namespace ReflectionUtilities
{
// =============================================================================
bool isPolyStruct(const PropertyAccessor& pa)
{
	auto type = pa.getType();
	if (!type.isPointer())
	{
		return false;
	}

	return pa.getDefinitionManager()->getDefinition(type.removePointer().getName()) != nullptr;
}

// =============================================================================
bool isStruct(const PropertyAccessor& pa)
{
	auto type = pa.getType();
	if (type.isPointer())
	{
		return false;
	}

	auto value = pa.getValue();
	ObjectHandle handle;
	if (!value.tryCast(handle))
	{
		return false;
	}

	return handle.getDefinition(*pa.getDefinitionManager()) != nullptr;
}

// =============================================================================
template <>
Variant copy<Variant>(Variant& value)
{
	return value;
}

// =============================================================================
template <>
Variant copy<const Variant>(const Variant& value)
{
	return value;
}

// =============================================================================
template <>
Variant reference<Variant>(Variant& value)
{
	return value;
}

// =============================================================================
template <>
Variant reference<const Variant>(const Variant& value)
{
	return value;
}
}
} // end namespace wgt
