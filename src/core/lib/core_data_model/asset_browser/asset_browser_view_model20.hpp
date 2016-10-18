//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_view_model_old.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_VIEW_MODEL20_H_
#define ASSET_BROWSER_VIEW_MODEL20_H_

#pragma once

#include "i_asset_browser_view_model20.hpp"

namespace wgt
{
namespace AssetBrowser20
{
class IAssetBrowserModel;
class IAssetBrowserEventModel;

class AssetBrowserViewModel : public IAssetBrowserViewModel
{
public:
	AssetBrowserViewModel(
	IDefinitionManager& definitionManager_,
	ObjectHandleT<IAssetBrowserModel> data,
	ObjectHandleT<IAssetBrowserEventModel> events);

	virtual ObjectHandle data() const override;

	virtual ObjectHandle events() const override;

	virtual IBreadcrumbsModel* getBreadcrumbsModel() const override;

	virtual const int& currentSelectedAssetIndex() const override;

	virtual void currentSelectedAssetIndex(const int& index) override;

	virtual IAssetObjectItem* getSelectedAssetData() const override;

	virtual bool refreshData() const override;
	virtual void onItemSelected(std::string assetPath) override;

private:
	void onUseSelectedAsset(const IAssetObjectItem& selectedAsset);
	void updateFolderContentsFilter(const Variant& filter);

	struct AssetBrowserViewModelImplementation;
	std::unique_ptr<AssetBrowserViewModelImplementation> impl_;
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif // ASSET_BROWSER_VIEW_MODEL20_H_
