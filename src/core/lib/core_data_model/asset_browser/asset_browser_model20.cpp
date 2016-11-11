
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_model20.hpp"

namespace wgt
{
namespace AssetBrowser20
{
AssetBrowserModel::AssetBrowserModel() : assetModel_(nullptr), iconSize_(64)
{
}

AssetBrowserModel::~AssetBrowserModel()
{
}

AbstractItemModel* AssetBrowserModel::getAssetModel() const
{
	return assetModel_;
}

const std::vector<std::string>& AssetBrowserModel::getNameFilters() const
{
	return nameFilters_;
}

int AssetBrowserModel::getIconSize() const
{
	return iconSize_;
}

void AssetBrowserModel::setAssetModel(AbstractItemModel* assetModel)
{
	assetModel_ = assetModel;
}

void AssetBrowserModel::setNameFilters(const std::vector<std::string>& nameFilters)
{
	nameFilters_ = nameFilters;
}

void AssetBrowserModel::setIconSize(int iconSize)
{
	iconSize_ = iconSize;
}
}
} // end namespace wgt
