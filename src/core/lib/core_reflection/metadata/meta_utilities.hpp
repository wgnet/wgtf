#ifndef META_UTILITIES_HPP
#define META_UTILITIES_HPP

#include "../utilities/reflection_utilities.hpp"
#include "../property_accessor.hpp"
#include "../interfaces/i_base_property.hpp"
#include "../reflection_dll.hpp"
#include "meta_base.hpp"

namespace wgt
{
class IBaseProperty;

//==============================================================================
REFLECTION_DLL MetaHandle findFirstMetaData( const TypeId & typeId, const MetaHandle & metaData, const IDefinitionManager & definitionManager );
REFLECTION_DLL MetaHandle findFirstMetaData( const TypeId & typeId, const PropertyAccessor & accessor, const IDefinitionManager & definitionManager );
REFLECTION_DLL MetaHandle findFirstMetaData( const TypeId & typeId, const IBaseProperty & pProperty, const IDefinitionManager & definitionManager );
REFLECTION_DLL MetaHandle findFirstMetaData( const TypeId & typeId, const IClassDefinition & definition, const IDefinitionManager & definitionManager );


//==============================================================================
template <class T>
T* MetaData(const MetaHandle& metaData, const IDefinitionManager& definitionManager)
{
	auto root = reflectedRoot( metaData, definitionManager );
	auto target = reflectedCast< T >( root, definitionManager );
	return target.get();
}

//==============================================================================
template< class T >
const T * findFirstMetaData( const MetaHandle & metaData, const IDefinitionManager & definitionManager )
{
	auto meta = findFirstMetaData( TypeId::getType< T >(), metaData, definitionManager );
	return MetaData< T >( meta, definitionManager );
}


//==============================================================================
template< class T >
const T * findFirstMetaData( const PropertyAccessor & accessor, const IDefinitionManager & definitionManager )
{
	auto meta = findFirstMetaData( TypeId::getType< T >(), accessor, definitionManager );
	return MetaData< T >( meta, definitionManager );
}


//==============================================================================
template< class T >
const T * findFirstMetaData( const IBaseProperty & pProperty, const IDefinitionManager & definitionManager )
{
	auto meta = findFirstMetaData( TypeId::getType< T >(), pProperty, definitionManager );
	return MetaData< T >( meta, definitionManager );
}


//==============================================================================
template< class T >
const T * findFirstMetaData( const IClassDefinition & definition, const IDefinitionManager & definitionManager )
{
	auto meta = findFirstMetaData( TypeId::getType< T >(), definition, definitionManager );
	return MetaData< T >( meta, definitionManager );
}
} // end namespace wgt
#endif //META_UTILITIES_HPP
