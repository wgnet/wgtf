#ifndef CONTAINER_HELPER_HPP
#define CONTAINER_HELPER_HPP

#include <vector>

namespace wgt
{
template< typename Container >
struct ContainerHelper
{
	typedef void FirstArgType;
	typedef void SecondArgType;
	typedef void ThirdArgType;
	typedef void ForthArgType;
};

template< template <typename> class Container, typename A1 >
struct ContainerHelper< Container< A1 > >
{
	typedef A1 FirstArgType;
	typedef void SecondArgType;
	typedef void ThirdArgType;
	typedef void ForthArgType;
};


template< template <typename, typename > class Container, typename A1, typename A2 >
struct ContainerHelper< Container< A1, A2 > >
{
	template< typename TypeToCheck, bool isBase >
	struct ContainerType
	{
		typedef void type;
	};

	template< typename TypeToCheck >
	struct ContainerType< TypeToCheck, true >
	{
		typedef Container<
			typename ContainerHelper< TypeToCheck >::FirstArgType,
			typename ContainerHelper< TypeToCheck >::SecondArgType > type;
	};

	template< typename TypeToCheck >
	struct BaseTypeExtractor
	{
		typedef Container<
			typename ContainerHelper< TypeToCheck >::FirstArgType,
			typename ContainerHelper< TypeToCheck >::SecondArgType > potentialType;

		typedef typename ContainerType<
			TypeToCheck,
			std::is_base_of< potentialType, TypeToCheck >::value >::type type;

		static const bool isBase = std::is_base_of< potentialType, TypeToCheck >::value;
	};

	typedef A1 FirstArgType;
	typedef A2 SecondArgType;
	typedef void ThirdArgType;
	typedef void ForthArgType;
};


template< template <typename, typename, typename > class Container, typename A1, typename A2, typename A3 >
struct ContainerHelper< Container< A1, A2, A3 > >
{
	typedef A1 FirstArgType;
	typedef A2 SecondArgType;
	typedef A3 ThirdArgType;
	typedef void ForthArgType;
};


template< template <typename, typename, typename, typename> class Container, typename A1, typename A2, typename A3, typename A4 >
struct ContainerHelper< Container< A1, A2, A3, A4 > >
{
	template< typename TypeToCheck, bool isBase >
	struct ContainerType
	{
		typedef void type;
	};

	template< typename TypeToCheck >
	struct ContainerType< TypeToCheck, true >
	{
		typedef Container<
			typename ContainerHelper< TypeToCheck >::FirstArgType,
			typename ContainerHelper< TypeToCheck >::SecondArgType,
			typename ContainerHelper< TypeToCheck >::ThirdArgType,
			typename ContainerHelper< TypeToCheck >::ForthArgType > type;
	};


	template< typename TypeToCheck >
	struct BaseTypeExtractor
	{
		typedef Container<
			typename ContainerHelper< TypeToCheck >::FirstArgType,
			typename ContainerHelper< TypeToCheck >::SecondArgType,
			typename ContainerHelper< TypeToCheck >::ThirdArgType,
			typename ContainerHelper< TypeToCheck >::ForthArgType > potentialType;

		typedef typename ContainerType<
			TypeToCheck,
			std::is_base_of< potentialType, TypeToCheck >::value >::type type;

		static const bool isBase = std::is_base_of< potentialType, TypeToCheck >::value;
	};

	typedef A1 FirstArgType;
	typedef A2 SecondArgType;
	typedef A3 ThirdArgType;
	typedef A4 ForthArgType;
};
} // end namespace wgt
#endif //CONTAINER_HELPER_HPP
