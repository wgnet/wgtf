#ifndef I_ITEM_ROLE_HPP
#define I_ITEM_ROLE_HPP

#include "wg_types/hash_utilities.hpp"

#define ITEMROLE( ROLE )														\
namespace ItemRole {															\
	namespace {																	\
		const char * ROLE##Name = #ROLE;										\
		const unsigned int ROLE##Id = ItemRole::compute( #ROLE );				\
	}																			\
}

namespace wgt
{
namespace ItemRole
{
	inline unsigned int compute( const char * roleName )
	{
		return static_cast<unsigned int>(HashUtilities::compute( roleName ));
	}
}

// DEPRECATED
// Add new role types here
#define ITEM_ROLES																\
	/* Tree Adapter */															\
	X( ChildModel, childModel )													\
	X( ColumnModel, columnModel )												\
	X( HasChildren, hasChildren )												\
	X( Expanded, expanded )														\
	X( ParentIndex, parentIndex )												\
	X( Selected, selected )														\
	X( IndexPath, indexPath )													\
	/* Component Extension */													\
	X( Component, component )													\
	X( ValueType, valueType )													\
	X( KeyType, keyType )														\
	X( IsEnum, isEnum )															\
	X( IsThumbnail, isThumbnail )												\
	X( IsSlider, isSlider )														\
	X( IsColor, isColor )														\
	X( IsUrl, isUrl )															\
	/* Value Extension */														\
	X( Value, value )															\
	X( Key, key )																\
	X( MinValue, minValue )														\
	X( MaxValue, maxValue )														\
	X( StepSize, stepSize )														\
	X( Decimals, decimals )														\
	X( EnumModel, enumModel )													\
	X( Definition, definition )													\
	X( Object, object )                                                         \
	X( RootObject, rootObject )                                                 \
	X( DefinitionModel, definitionModel )										\
	X( UrlIsAssetBrowser, urlIsAssetBrowser )									\
	X( UrlDialogTitle, urlDialogTitle )											\
	X( UrlDialogDefaultFolder, urlDialogDefaultFolder )							\
	X( UrlDialogNameFilters, urlDialogNameFilters )								\
	X( UrlDialogSelectedNameFilter, urlDialogSelectedNameFilter )				\
	X( UrlDialogModality, urlDialogModality )									\
	/* Asset Item Extension */													\
	X( StatusIcon, statusIcon )													\
	X( TypeIcon, typeIcon )														\
	X( Size, size )																\
	X( CreatedTime, createdTime )												\
	X( ModifiedTime, modifiedTime )												\
	X( AccessedTime, accessedTime )												\
	X( IsDirectory, isDirectory )												\
	X( IsReadOnly, isReadOnly )													\
	X( IsCompressed, isCompressed )												\
	/* Thumbnail Extension */													\
	X( Thumbnail, thumbnail )													\
	X( headerText, headerText )													\
	X( footerText, footerText )													\

#define X( ROLE, ROLENAME ) 													\
	class ROLE##Role															\
	{																			\
	public:																		\
		static const char * roleName_;											\
		static unsigned int roleId_;											\
	};
	ITEM_ROLES
#undef X
} // end namespace wgt
#endif //I_ITEM_ROLE_HPP
