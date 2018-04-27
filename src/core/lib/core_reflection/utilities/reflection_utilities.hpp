#ifndef REFLECTION_UTILITIES_HPP
#define REFLECTION_UTILITIES_HPP

#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include <type_traits>

namespace wgt
{
class IDefinitionManager;
class PropertyAccessor;
class IBaseProperty;
typedef std::shared_ptr<IBaseProperty> IBasePropertyPtr;
class IClassDefinition;

namespace ReflectionUtilities
{
REFLECTION_DLL bool isStruct(const PropertyAccessor& pa);
REFLECTION_DLL void copyProperties(IDefinitionManager& definitionManager, const ObjectHandle& src,
                                   const ObjectHandle& dst, bool notify);

// =============================================================================
template <typename T>
typename std::enable_if<!std::is_enum<T>::value, Variant>::type copy(T& value)
{
	return value;
}

// =============================================================================
template <typename T>
typename std::enable_if<std::is_enum<T>::value, Variant>::type copy(T& value)
{
	return static_cast<typename std::underlying_type<T>::type>(value);
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

	Collection col;
	if (variant.tryCast(col))
	{
		const T* valuePtr = col.container<T>();
		if (valuePtr != nullptr)
		{
			valueFn(*valuePtr);
			return true;
		}
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

// Returns a tuple to the property if found, and a bool to indicate whether anything is left to parse, and the name of
// the property.
std::tuple<std::shared_ptr<IBaseProperty>, bool, std::string> parseProperty(const char* path, const Variant& object,
	const IBasePropertyPtr& parentProperty, const IClassDefinition* definition, const IDefinitionManager& definitionManager);
}
} // end namespace wgt
#endif // REFLECTION_UTILITIES_HPP
