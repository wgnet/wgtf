#ifndef DEFINITION_HELPERS_HPP
#define DEFINITION_HELPERS_HPP

#include <stddef.h>
#include <type_traits>

namespace wgt
{
class IClassDefinition;
class MetaBase;

//==============================================================================
template< typename T, bool IsAbstract >
struct CreateHelperBase
{
	static T * create()
	{
		return nullptr;
	}
};

//==============================================================================
template< typename T >
struct CreateHelperBase < T, false >
{
	static T * create()
	{
		return new T();
	}

	template<class TArg1>
	static T * create(TArg1&& arg1)
	{
		return new T(std::forward<TArg1>(arg1));
	}

	template<class TArg1, class TArg2>
	static T * create(TArg1&& arg1, TArg2&& arg2)
	{
		return new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2));
	}

	template<class TArg1, class TArg2, class TArg3>
	static T * create(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3)
	{
		return new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2),
			std::forward<TArg3>(arg3));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	static T * create(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4)
	{
		return new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2),
			std::forward<TArg3>(arg3), std::forward<TArg4>(arg4));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	static T * create(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5)
	{
		return new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2),
			std::forward<TArg3>(arg3), std::forward<TArg4>(arg4), std::forward<TArg5>(arg5));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5, class TArg6>
	static T * create(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5, TArg6&& arg6)
	{
		return new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
			std::forward<TArg4>(arg4), std::forward<TArg5>(arg5), std::forward<TArg6>(arg6));
	}
};

//==============================================================================
template< typename T, bool IsAbstract = std::is_abstract< T >::value >
struct CreateHelper : public CreateHelperBase<T, IsAbstract>
{
};
} // end namespace wgt
#endif
