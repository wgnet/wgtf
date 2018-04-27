#ifndef TUPLE_HELPER_HPP
#define TUPLE_HELPER_HPP

#include <functional>
#include <tuple>

namespace wgt 
{

template< unsigned int N >
struct apply_tuple_impl;

template<>
struct apply_tuple_impl< 0 >
{
	template< typename ...ArgsF, typename ...ArgsT, typename ...Args >
	static void apply_tuple(
		const std::function< void(ArgsF...) > && func,
		const std::tuple< ArgsT... > && tuple, Args &&... args)
	{
		func(args...);
	}
};

template< unsigned int N >
struct apply_tuple_impl
{
	template< typename ...ArgsF, typename ...ArgsT, typename ...Args >
	static void apply_tuple(
		const std::function< void(ArgsF...) > && func,
		const std::tuple< ArgsT... > && tuple, Args && ... args)
	{
		typedef decltype(tuple) tuple_type;
		apply_tuple_impl< N - 1 >::apply_tuple(
			std::forward< decltype(func) >(func),
			std::forward< tuple_type >(tuple),
			std::get< N - 1 >(std::forward< tuple_type >(tuple)),
			std::forward< Args >(args)...);
	}
};

template< typename ...ArgsF, typename ...ArgsT >
void apply_tuple(
	const std::function< void(ArgsF...) > && func,
	const std::tuple< ArgsT... > && tuple)
{
	apply_tuple_impl< sizeof...(ArgsF)>::apply_tuple(
		std::forward< decltype(func) >(func),
		std::forward< decltype(tuple) >(tuple));
}

} //end namespace wgt

#endif //TUPLE_HELPER_HPP