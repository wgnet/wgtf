#include "meta_utilities.hpp"

namespace wgt
{
MetaHandle findFirstMetaData( const TypeId & typeId, const MetaHandle & metaData, const IDefinitionManager & definitionManager )
{
	auto targetDefinition = definitionManager.getDefinition( typeId.getName() );
	assert( targetDefinition != nullptr );

	auto meta = metaData;
	while ( meta != nullptr )
	{
		auto root = reflectedRoot( meta, definitionManager );
		auto rootDefinition = definitionManager.getDefinition( root.type().getName() );
		assert( rootDefinition != nullptr );
		if (rootDefinition->canBeCastTo( *targetDefinition ))
		{
			return meta;
		}
		meta = meta->next();
	}
	return nullptr;
}


MetaHandle findFirstMetaData( const TypeId & typeId, const PropertyAccessor & accessor, const IDefinitionManager & definitionManager )
{
	auto metaData = accessor.getMetaData();
	auto targetMetaData = findFirstMetaData( typeId, metaData, definitionManager );
	if (targetMetaData != nullptr)
	{
		return targetMetaData;
	}
	PropertyAccessor parentAccessor = accessor.getParent();
	if (!parentAccessor.canGetValue() ||
		parentAccessor.getValue().typeIs< Collection >() == false)
	{
		return nullptr;
	}
	return findFirstMetaData( typeId, parentAccessor, definitionManager );
}


MetaHandle findFirstMetaData( const TypeId & typeId, const IBaseProperty & pProperty, const IDefinitionManager & definitionManager )
{
	auto metaData = pProperty.getMetaData();
	return findFirstMetaData( typeId, metaData, definitionManager );
}


MetaHandle findFirstMetaData( const TypeId & typeId, const IClassDefinition & definition, const IDefinitionManager & definitionManager )
{
	auto metaData = definition.getMetaData();
	return findFirstMetaData( typeId, metaData, definitionManager );
}
} // end namespace wgt
