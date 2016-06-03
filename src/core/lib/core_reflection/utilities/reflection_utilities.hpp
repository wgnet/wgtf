#ifndef REFLECTION_UTILITIES_HPP
#define REFLECTION_UTILITIES_HPP

#include "core_variant/variant.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
class IDefinitionManager;

namespace ReflectionUtilities
{

bool isPolyStruct( const PropertyAccessor & pa );
bool isStruct( const PropertyAccessor & pa );


// =============================================================================
template< typename T >
Variant copy( T & value )
{
	Variant result( value, true );
	if( result.isVoid() )
	{
		result = ObjectHandle( value );
	}

	return result;
}


// =============================================================================
template< typename T >
Variant reference( T & value )
{
	Variant result( value, true );
	if( result.isVoid() )
	{
		result = ObjectHandle( &value );
	}

	return result;
}


// =============================================================================
template< typename T >
Variant reference( T *& value )
{
	Variant result( value, true );
	if( result.isVoid() )
	{
		result = ObjectHandle( value );
	}

	return result;
}


// =============================================================================
template<>
Variant copy< Variant >( Variant & value );


// =============================================================================
template<>
Variant copy< const Variant >( const Variant & value );


// =============================================================================
template<>
Variant reference< Variant >( Variant & value );


// =============================================================================
template<>
Variant reference< const Variant >( const Variant & value );


// =============================================================================
template< typename T >
bool extract( const Variant & variant, T & value, const IDefinitionManager & defManager )
{
	if (variant.isVoid())
	{
		return false;
	}

	if (variant.tryCast( value ))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast( handle ))
	{
		auto valuePtr = reflectedCast< T >( handle.data(), handle.type(), defManager );
		if (valuePtr)
		{
			value = *valuePtr;
			return true;
		}
	}

	return false;
}


// =============================================================================
template< typename T >
bool extract(const Variant & variant, T *& value, const IDefinitionManager & defManager)
{
	if (variant.isVoid())
	{
		value = nullptr;
		return true;
	}

	if (variant.tryCast( value ))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast( handle ))
	{
		value = reflectedCast< T >( handle.data(), handle.type(), defManager );
		return true;
	}

	return false;
}


// =============================================================================
template< typename T >
bool extract(const Variant & variant, ObjectHandleT< T > & value, const IDefinitionManager & defManager)
{
	if (variant.isVoid())
	{
		value = nullptr;
		return true;
	}

	if (variant.tryCast( value ))
	{
		return true;
	}

	ObjectHandle handle;
	if (variant.tryCast( handle ))
	{
		value = reflectedCast< T >( handle, defManager );
		return true;
	}

	return false;
}


}
} // end namespace wgt
#endif //REFLECTION_UTILITIES_HPP
