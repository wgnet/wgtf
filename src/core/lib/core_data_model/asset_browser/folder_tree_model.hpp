#ifndef FOLDER_TREE_MODEL_HPP
#define FOLDER_TREE_MODEL_HPP

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "i_asset_browser_model.hpp"

#include <memory>

namespace wgt
{
class IFileSystem;

//------------------------------------------------------------------------------
// FolderTreeModel
//
// Represents the tree model with all of the items for browsing assets based
// on the registered IFileSystem for folder retrieval.
//------------------------------------------------------------------------------
class FolderTreeModel: public ITreeModel
{
public:
	FolderTreeModel( IAssetBrowserModel & model, IFileSystem& fileSystem );

	FolderTreeModel( const FolderTreeModel& rhs );
	virtual ~FolderTreeModel();

	void init( IAssetBrowserModel* model );
	IAssetBrowserModel* model() const;

	FolderTreeModel& operator=( const FolderTreeModel& rhs );

	virtual IItem* item( size_t index, const IItem* parent ) const override;
	virtual ItemIndex index( const IItem* item ) const override;
	virtual bool empty( const IItem* parent ) const override;
	virtual size_t size( const IItem* parent ) const override;
	virtual int columnCount() const override;

private:
	void setAssetPaths( const std::vector<std::string>& paths );

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // FOLDER_TREE_MODEL_HPP
