#ifndef FUNCTION_TRAITS_HPP
#define FUNCTION_TRAITS_HPP

namespace wgt
{

/**
/* These are a bunch of utility function traits for functions, member functions 
 * and lambdas.
 * I've only added the ones I've needed, if there are other types you need, feel
 * free to add to this file.
 */
template<typename... Ts> struct make_void { typedef void type; };
template<typename... Ts> using void_t = typename make_void<Ts...>::type;

template< typename T, typename = void >
struct function_traits;

template< typename R, typename ...Args>
struct function_traits<R(*)(Args...)>
{
	using return_type = R;
	using class_type = void;
	using args_type = std::tuple< Args... >;
};

template< typename R, typename C, typename ...Args>
struct function_traits<R( C::*)(Args...) const>
{
	using return_type = R;
	using class_type = C;
	using args_type = std::tuple< Args... >;
};


//This picks up lambdas and decays them to const member functions
template< typename T >
struct function_traits< T, void_t< decltype( &T::operator() ) > >
	: public function_traits< decltype( &T::operator() ) >
{
};

} //end namespace wgt

#endif //FUNCTION_TRAITS_HPP