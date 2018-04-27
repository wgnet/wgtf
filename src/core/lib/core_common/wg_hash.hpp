#ifndef WG_HASH_HPP
#define WG_HASH_HPP

#include <type_traits>
#include <stdint.h>
#include <string>

namespace wgt
{

template< typename T >
struct hash {};

template< typename T, typename = void >
struct is_hashable
	: std::false_type {};

template< typename T>
struct is_hashable < T, std::void_t< decltype(std::declval< hash< T > >().operator()(std::declval< const T & >())) > >
	: std::true_type {};

#define REMAP_STD_HASH( type )\
template <>\
struct hash<type> : public std::unary_function<type, size_t>\
{\
	public:\
		size_t operator()(const type & v) const\
	{\
		std::hash<type> hash_fn; \
		return hash_fn(v); \
	}\
};\

REMAP_STD_HASH(uint32_t)
REMAP_STD_HASH(int32_t)
REMAP_STD_HASH(int64_t)
REMAP_STD_HASH(double)
REMAP_STD_HASH(uint64_t)
REMAP_STD_HASH(std::string)

}

#define DEFINE_CUST_HASHER( type, mem_func, hasher )\
namespace wgt{\
template <>\
struct hash<type> : public std::unary_function<type, size_t>\
{\
public:\
	size_t operator()(const type & v) const\
	{\
		wgt::hash<hasher> hash_fn;\
		return hash_fn(v.mem_func());\
	}\
};\
}

#define DEFINE_CUST_HASH( type, mem_func )\
DEFINE_CUST_HASHER( type, mem_func, uint64_t )


#define EXTERN_DECLARE_CUST_HASH( type )\
namespace wgt\
{\
extern template struct hash<type>;\
}\

#endif //WG_HASH_HPP