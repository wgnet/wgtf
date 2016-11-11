#ifndef REFLECTION_UTILITIES_HPP
#define REFLECTION_UTILITIES_HPP

#include "core_variant/variant.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflection_dll.hpp"
#include <type_traits>

namespace wgt
{
class IDefinitionManager;

namespace ReflectionUtilities
{
REFLECTION_DLL bool isPolyStruct(const PropertyAccessor& pa);
REFLECTION_DLL bool isStruct(const PropertyAccessor& pa);

// =============================================================================
template <typename T>
Variant copy(T& value)
{
	return value;
}

// =============================================================================
template <typename T>
typename std::enable_if<!Variant::traits<T>::pass_through, Variant>::type reference(T& value)
{
	// If T has upcast then allow it to do its job ...
	return value;
}

// =============================================================================
template <typename T>
typename std::enable_if<Variant::traits<T>::pass_through && !Variant::traits<T>::shared_ptr_storage, Variant>::type
reference(T& value)
{
	// ... otherwise it's OK to pass pointer to value
	return &value;
}

// =============================================================================
template <typename T>
typename std::enable_if<Variant::traits<T>::pass_through && Variant::traits<T>::shared_ptr_storage, Variant>::type
reference(T& value)
{
	// ... otherwise it's OK to pass pointer to value
	return value;
}

// =============================================================================
template <typename T>
Variant reference(T* value)
{
	return value;
}

// =============================================================================
template <>
REFLECTION_DLL Variant copy<Variant>(Variant& value);

// =============================================================================
template <>
REFLECTION_DLL Variant copy<const Variant>(const Variant& value);

// =============================================================================
template <>
REFLECTION_DLL Variant reference<Variant>(Variant& value);

// =============================================================================
template <>
REFLECTION_DLL Variant reference<const Variant>(const Variant& value);

// =============================================================================
template <typename T>
bool extract(const Variant& variant, T& value, const IDefinitionManager& defManager)
{
	if (variant.isVoid())
	{
		return false;
	}

	if (variant.tryCast(value))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast(handle))
	{
		auto valuePtr = reflectedCast<T>(handle.data(), handle.type(), defManager);
		if (valuePtr)
		{
			value = *valuePtr;
			return true;
		}
	}

	return false;
}

// =============================================================================
template <typename T>
bool extract(const Variant& variant, T*& value, const IDefinitionManager& defManager)
{
	if (variant.isVoid())
	{
		value = nullptr;
		return true;
	}

	if (variant.tryCast(value))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast(handle))
	{
		value = reflectedCast<T>(handle.data(), handle.type(), defManager);
		return true;
	}

	return false;
}

// =============================================================================
template <typename T>
bool extract(const Variant& variant, ObjectHandleT<T>& value, const IDefinitionManager& defManager)
{
	if (variant.isVoid())
	{
		value = nullptr;
		return true;
	}

	if (variant.tryCast(value))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast(handle))
	{
		value = reflectedCast<T>(handle, defManager);
		return true;
	}

	return false;
}

// =============================================================================
template <typename T>
bool extract(const Variant& variant, std::function<void(const T&)> valueFn, const IDefinitionManager& defManager)
{
	if (variant.isVoid())
	{
		valueFn(T());
		return true;
	}

	if (variant.visit<T, std::function<void(const T&)>>(valueFn))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast(handle))
	{
		auto valuePtr = reflectedCast<T>(handle.data(), handle.type(), defManager);
		if (valuePtr != nullptr)
		{
			valueFn(*valuePtr);
			return true;
		}
	}

	return false;
}

// =============================================================================
template <typename T>
bool extract(const Variant& variant, std::function<void(const T*&)> valueFn, const IDefinitionManager& defManager)
{
	if (variant.isVoid())
	{
		valueFn(nullptr);
		return true;
	}

	if (variant.visit<T*, std::function<void(const T*&)>>(valueFn))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast(handle))
	{
		valueFn(reflectedCast<T>(handle.data(), handle.type(), defManager));
		return true;
	}

	return false;
}

// =============================================================================
template <typename T>
bool extract(const Variant& variant, std::function<void(const ObjectHandleT<T>&)> valueFn,
             const IDefinitionManager& defManager)
{
	if (variant.isVoid())
	{
		valueFn(ObjectHandleT<T>());
		return true;
	}

	if (variant.visit<ObjectHandleT<T>, std::function<void(const ObjectHandleT<T>&)>>(valueFn))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast(handle))
	{
		valueFn(reflectedCast<T>(handle, defManager));
		return true;
	}

	return false;
}
}
} // end namespace wgt
#endif // REFLECTION_UTILITIES_HPP
