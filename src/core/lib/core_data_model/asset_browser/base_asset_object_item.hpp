#ifndef BASE_ASSET_OBJECT_ITEM_HPP
#define BASE_ASSET_OBJECT_ITEM_HPP

#include "i_asset_object_item.hpp"
#include "core_serialization/i_file_system.hpp"

namespace wgt
{
class BaseAssetObjectItem : public IAssetObjectItem
{
public:
	BaseAssetObjectItem( const IFileInfoPtr & fileInfo, const IItem * parent, 
		IFileSystem * fileSystem, IAssetPresentationProvider * presentationProvider );
	BaseAssetObjectItem( const BaseAssetObjectItem & rhs );
	virtual ~BaseAssetObjectItem();

	BaseAssetObjectItem& operator=( const BaseAssetObjectItem & rhs );

	// File Information
	// TODO: Remove dependency on the file system on low level models in the asset browser.
	// JIRA: NGT-1248
	IFileInfoPtr getFileInfo() const;

	// IItem Implementation
	virtual const char* getDisplayText( int column ) const override;
	virtual ThumbnailData getThumbnail( int column ) const override;
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override;

	// IAssetObjectItem Implementation
	virtual const IItem* getParent() const override;
	virtual IItem* operator[]( size_t index ) const override;
	virtual size_t indexOf( const IItem* item ) const override;
	virtual bool empty() const override;
	virtual size_t size() const override;

	virtual ThumbnailData getStatusIconData() const override;

	virtual const char* getAssetName() const override;
	virtual uint16_t getAssetType() const override;
	virtual const char* getFullPath() const override;
	virtual uint64_t getSize() const override;
	virtual uint64_t getCreatedTime() const override;
	virtual uint64_t getModifiedTime() const override;
	virtual uint64_t getAccessedTime() const override;
	virtual bool isDirectory() const override;
	virtual bool isReadOnly() const override;
	virtual bool isCompressed() const override;

	// Custom Functions for Basic Asset Data Using FileInfo
	virtual const char* getTypeIconResourceString() const;

private:

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // BASE_ASSET_OBJECT_ITEM_HPP
