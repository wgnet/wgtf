#ifndef DEFINITION_HELPERS_HPP
#define DEFINITION_HELPERS_HPP

#include "core_common/assert.hpp"

#include <stddef.h>
#include <type_traits>

namespace wgt
{
class IClassDefinition;

template <typename T>
struct CreateHelperArgsBase
{
	template <class Parameter>
	static T* create(Parameter&& parameter)
	{
		return new T(std::forward<Parameter>(parameter));
	}

	template <class... Parameter>
	static T* create(Parameter&&... parameter)
	{
		return new T(std::forward<Parameter>(parameter)...);
	}
};

//==============================================================================
template <typename T, bool IsDefaultConstructible>
struct CreateHelperBase : public CreateHelperArgsBase<T>
{
	static T* create()
	{
		return new T();
	}

	using CreateHelperArgsBase<T>::create;
};

//==============================================================================
template <typename T>
struct CreateHelperBase<T, false> : public CreateHelperArgsBase<T>
{
	static T* create()
	{
		TF_ASSERT(!"Calling create on a non-constructible type, check your type and/or constructor arguments");
		return nullptr;
	}

	using CreateHelperArgsBase<T>::create;
};

//==============================================================================
template <typename T, bool IsAbstract = std::is_abstract<T>::value,
          bool IsDefaultConstructible = std::is_default_constructible<T>::value>
struct CreateHelper : public CreateHelperBase<T, !IsAbstract && IsDefaultConstructible>
{
};
} // end namespace wgt
#endif
