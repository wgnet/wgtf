#ifndef REFLECTED_OBJECT_HPP
#define REFLECTED_OBJECT_HPP

#include <cassert>
#include <memory>
#include <type_traits>
#include <typeinfo>

namespace wgt
{
class IClassDefinition;

template< typename T >
class TypeClassDefinition;

template< typename T >
const char * getClassIdentifier()
{
	return typeid( T ).name();
}

#define DECLARE_REFLECTED \
	template< typename T >\
	friend class TypeClassDefinition;
} // end namespace wgt
#endif // REFLECTED_OBJECT_HPP
