#ifndef I_ASSET_OBJECT_ITEM_HPP
#define I_ASSET_OBJECT_ITEM_HPP

#include "core_data_model/i_item.hpp"

namespace wgt
{
class IAssetPresentationProvider;

/**
 IAssetObjectItem
 Represents an IItem used by the Asset Browser tree and list models. Contains special functionality for retrieving
 properties of an asset for display. May be overwritten by developers to provide custom functionality for 
 retrieving thumbnails, status icons, and additional data on assets unique to one's studio.
 */
class IAssetObjectItem : public IItem
{
public:
	IAssetObjectItem() {}
	virtual ~IAssetObjectItem() {}
	
	// IItem Methods
	virtual const char* getDisplayText( int column ) const override = 0;
	virtual ThumbnailData getThumbnail( int column ) const override = 0;
	virtual Variant getData( int column, size_t roleId ) const override = 0;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override = 0;
	
	// IAssetObjectItem Methods for Tree Model Usage
	virtual const IItem* getParent() const = 0;
	virtual IItem* operator[]( size_t index ) const = 0;
	virtual size_t indexOf( const IItem* item ) const = 0;
	virtual bool empty() const = 0;
	virtual size_t size() const = 0;

	// Retrieves the binary block for a status overlay in the asset browser list. Allows developers to use multiple 
	// status icons by combining them. If not combining icons, read in the icon you wish to use and pass its 
	// data as the return value.
	virtual ThumbnailData getStatusIconData() const = 0;

	// Retrieves common properties associated with all asset items in the asset browser
	virtual const char* getAssetName() const = 0;	// Name of the asset (may include extension, but not required)
	virtual uint16_t getAssetType() const = 0;		// The type of asset by numeric; may map to an enum
	virtual const char* getFullPath() const = 0;	// Path of the asset in the resources directory or package
	virtual uint64_t getSize() const = 0;			// Size of asset's file, not dimensions!
	virtual uint64_t getCreatedTime() const = 0;	// Time asset was created (depends on operating system)
	virtual uint64_t getModifiedTime() const = 0;	// Time asset was last modified
	virtual uint64_t getAccessedTime() const = 0;	// Time asset was last accessed/opened
	virtual bool isDirectory() const = 0;			// This asset represents a directory (depends on resource structure)
	virtual bool isReadOnly() const = 0;			// This asset is flagged as read-only
	virtual bool isCompressed() const = 0;			// This asset is a compressed file (zip, tar, resource pak, etc.)
};
} // end namespace wgt
#endif // I_ASSET_OBJECT_ITEM_HPP
