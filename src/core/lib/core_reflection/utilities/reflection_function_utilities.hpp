#ifndef REFLECTION_FUNCTION_UTILITIES_HPP
#define REFLECTION_FUNCTION_UTILITIES_HPP

#include "core_variant/type_id.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/reflected_property.hpp"
#include <type_traits>

namespace wgt
{
template<typename T, bool is_enum = std::is_enum<T>::value>
struct target_type { using type = T; };
template<typename T>
struct target_type<T, true> { using type = typename std::underlying_type<T>::type; };

template <typename SelfType>
struct FunctionPropertyHelper
{
	template <typename DataType>
	static BaseProperty* getBaseProperty(const char* name, DataType SelfType::*memberPointer)
	{
		using TargetType = typename target_type<DataType>::type;
		return new ReflectedProperty<TargetType, SelfType>(name, (TargetType SelfType::*)memberPointer, TypeId::getType<TargetType>());
	}

	template <typename DataType>
	static BaseProperty* getBaseProperty(
	const char* name, DataType (SelfType::*getterFunc)() const,
	typename FunctionPropertySet<DataType, SelfType>::SetterFunc setterFunc = nullptr)
	{
		return new FunctionProperty<DataType, SelfType, !std::is_reference<DataType>::value, false>(
		name, getterFunc, setterFunc, TypeId::getType<DataType>());
	}

	template <typename DataType>
	static BaseProperty* getBaseProperty(
	const char* name, void (SelfType::*getterFunc)(DataType*) const,
	typename FunctionPropertySet<DataType, SelfType>::SetterFunc setterFunc = nullptr)
	{
		return new FunctionProperty<DataType, SelfType, true, true>(name, getterFunc, setterFunc,
		                                                            TypeId::getType<DataType>());
	}

	template <typename DataType>
	static BaseProperty* getBaseProperty(const char* name, size_t (SelfType::*getSizeFunc)() const,
	                                     DataType& (SelfType::*getValueFunc)(size_t) const)
	{
		return new FunctionCollectionProperty<
			size_t, DataType, SelfType, decltype(getValueFunc) >(
				name, getSizeFunc, nullptr, getValueFunc, nullptr, nullptr );
	}


	template <typename DataType>
	static BaseProperty* getBaseProperty(
		const char* name,
		size_t(SelfType::*getSizeFunc)() const,
		DataType (SelfType::*getValueFunc)(size_t) const,
		size_t (SelfType::*addKeyFunc)(const size_t & ) = nullptr,
		size_t(SelfType::*removeKeyFunc)(const size_t &) = nullptr )
	{
		return new FunctionCollectionProperty<
			size_t, DataType, SelfType, decltype( getValueFunc ) >(
				name, getSizeFunc, nullptr, getValueFunc, addKeyFunc, removeKeyFunc );
	}

	template <typename DataType, typename KeyType>
	static BaseProperty* getBaseProperty(const char* name, size_t (SelfType::*getSizeFunc)() const,
	                                     DataType& (SelfType::*getValueFunc)(size_t) const,
	                                     size_t (SelfType::*addKeyFunc)(const KeyType&) = nullptr,
	                                     KeyType& (SelfType::*getKeyFunc)(size_t) const = nullptr )
	{
		return new FunctionCollectionProperty<
			KeyType, DataType, SelfType, decltype(getValueFunc)>(
				name, getSizeFunc, getKeyFunc, getValueFunc, addKeyFunc, nullptr );
	}
};
} // end namespace wgt
#endif // REFLECTION_FUNCTION_UTILITIES_HPP
