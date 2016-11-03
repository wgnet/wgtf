#ifndef BASE_ASSET_OBJECT_ITEM20_HPP
#define BASE_ASSET_OBJECT_ITEM20_HPP

#include "i_asset_object_item20.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_data_model/common_data_roles.hpp"
namespace wgt
{
ITEMROLE(indexPath)
ITEMROLE(thumbnail)
ITEMROLE(typeIcon)
ITEMROLE(assetSize)
ITEMROLE(createdTime)
ITEMROLE(modifiedTime)
ITEMROLE(accessedTime)
ITEMROLE(isDirectory)
ITEMROLE(isReadOnly)
ITEMROLE(isCompressed)
ITEMROLE(statusIcon)
namespace AssetBrowser20
{
class BaseAssetObjectItem : public IAssetObjectItem
{
public:
	BaseAssetObjectItem(const IFileInfoPtr& fileInfo, const AbstractItem* parent,
	                    IFileSystem* fileSystem, IAssetPresentationProvider* presentationProvider);
	BaseAssetObjectItem(const BaseAssetObjectItem& rhs);
	virtual ~BaseAssetObjectItem();

	BaseAssetObjectItem& operator=(const BaseAssetObjectItem& rhs);

	virtual Variant getData(int row, int column, ItemRole::Id roleId) const override;
	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override;

	// IAssetObjectItemOld Implementation
	virtual const IAssetObjectItem* getParent() const override;
	virtual IAssetObjectItem* operator[](size_t index) const override;
	virtual size_t indexOf(const IAssetObjectItem* item) const override;
	virtual bool empty() const override;
	virtual size_t size() const override;

	// File Information
	// TODO: Remove dependency on the file system on low level models in the asset browser.
	// JIRA: NGT-1248
	IFileInfoPtr getFileInfo() const;

	virtual std::shared_ptr<BinaryBlock> getStatusIconData() const override;

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
} // end namespace AssetBrowser20

} // end namespace wgt
#endif // BASE_ASSET_OBJECT_ITEM20_HPP
