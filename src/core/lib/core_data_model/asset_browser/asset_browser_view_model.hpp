//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_view_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_VIEW_MODEL_H_
#define ASSET_BROWSER_VIEW_MODEL_H_

#pragma once

#include "i_asset_browser_view_model.hpp"

namespace wgt
{
class IAssetBrowserModel;
class IAssetBrowserEventModel;
class IAssetBrowserContextMenuModel;

class AssetBrowserViewModel : public IAssetBrowserViewModel
{
public:
	AssetBrowserViewModel(
		IDefinitionManager& definitionManager_,
		ObjectHandleT<IAssetBrowserModel> data,
		ObjectHandleT<IAssetBrowserEventModel> events );

	virtual ObjectHandle data() const override;

	virtual ObjectHandle events() const override;

	virtual IBreadcrumbsModel * getBreadcrumbsModel() const override;

	virtual const int & currentSelectedAssetIndex() const override;

	virtual void currentSelectedAssetIndex( const int & index ) override;

	virtual IAssetObjectItem * getSelectedAssetData() const override;

	virtual bool refreshData() const override;

	virtual ISelectionHandler * getFolderSelectionHandler() const override;
	virtual ISelectionHandler * getFolderContentSelectionHandler() const override;

	virtual const char * getSelectedTreeItemName() override;

private:
	void onUseSelectedAsset( const IAssetObjectItem& selectedAsset );
	void updateFolderContentsFilter( const Variant& filter );

	struct AssetBrowserViewModelImplementation;
	std::unique_ptr<AssetBrowserViewModelImplementation> impl_;
};
} // end namespace wgt
#endif // ASSET_BROWSER_VIEW_MODEL_H_
