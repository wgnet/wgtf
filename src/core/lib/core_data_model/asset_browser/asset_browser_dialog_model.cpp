
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_dialog_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_dialog_model.hpp"
#include "i_asset_browser_model20.hpp"
#include "metadata/asset_browser_dialog_model.mpp"

namespace wgt
{
namespace AssetBrowser20
{

AssetBrowserDialogModel::AssetBrowserDialogModel()
{
	setURL("WGControls/WGAssetBrowserDialogFrame.qml");
}

Connection AssetBrowserDialogModel::connectAssetAccepted(AssetCallback callback)
{
	return assetAccepted_.connect(callback);
}

void AssetBrowserDialogModel::assetAccepted(const std::string& assetPath)
{
	assetAccepted_(assetPath);
}

AbstractItemModel* AssetBrowserDialogModel::assetModel() const
{
	auto assetBrowserModel = get<IAssetBrowserModel>();
	return assetBrowserModel ? assetBrowserModel->getAssetModel() : nullptr;
}

const char* AssetBrowserDialogModel::getNameFilter() const
{
	return nameFilter_.c_str();
}

void AssetBrowserDialogModel::setNameFilter(const char* nameFilter)
{
	nameFilter_ = nameFilter;
}

const char* AssetBrowserDialogModel::getSelectedAsset() const
{
	auto assetBrowserModel = get<IAssetBrowserModel>();
	return assetBrowserModel->getProgramSelectedItemPath();
}

void AssetBrowserDialogModel::setSelectedAsset(const char* selectedAsset)
{
	auto assetBrowserModel = get<IAssetBrowserModel>();
	assetBrowserModel->programSelectItemByPath(selectedAsset);
}

const AssetBrowserDialogModel::NameFilters&	AssetBrowserDialogModel::getNameFilters() const
{
	return nameFilters_;
}

void AssetBrowserDialogModel::setNameFilters(NameFilters nameFilters)
{
	nameFilters_ = std::move(nameFilters);
}

} // namespace AssetBrowser20
} // namespace wgt