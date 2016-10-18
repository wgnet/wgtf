#ifndef FOLDER_TREE_MODEL20_HPP
#define FOLDER_TREE_MODEL20_HPP

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "i_asset_browser_model20.hpp"

#include <memory>
#include "../abstract_item_model.hpp"

namespace wgt
{
class IFileSystem;

namespace AssetBrowser20
{
//------------------------------------------------------------------------------
// FolderTreeModel
//
// Represents the tree model with all of the items for browsing assets based
// on the registered IFileSystem for folder retrieval.
//------------------------------------------------------------------------------
class FolderTreeModel : public AbstractTreeModel
{
public:
	FolderTreeModel(IAssetBrowserModel& model, IFileSystem& fileSystem);

	FolderTreeModel(const FolderTreeModel& rhs);
	virtual ~FolderTreeModel();

	virtual std::vector<std::string> mimeTypes() const override;
	virtual MimeData mimeData(std::vector<AbstractItemModel::ItemIndex>& indexList) override;

	void init(IAssetBrowserModel* model);
	IAssetBrowserModel* model() const;

	FolderTreeModel& operator=(const FolderTreeModel& rhs);

	virtual AbstractItem* item(const ItemIndex& index) const override;

	virtual ItemIndex index(const AbstractItem* item) const override;

	virtual int rowCount(const AbstractItem* item) const override;

	virtual int columnCount() const override;
	virtual std::vector<std::string> roles() const override;

private:
	void setAssetPaths(const std::vector<std::string>& paths);

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif // FOLDER_TREE_MODEL20_HPP
