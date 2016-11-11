
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model_old.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_MODEL20_H_
#define ASSET_BROWSER_MODEL20_H_

#pragma once

#include "core_data_model/asset_browser/i_asset_browser_model20.hpp"

#include <memory>

namespace wgt
{
namespace AssetBrowser20
{
class AssetBrowserModel : public IAssetBrowserModel
{
public:
	AssetBrowserModel();
	virtual ~AssetBrowserModel();

	virtual AbstractItemModel* getAssetModel() const override;
	virtual const std::vector<std::string>& getNameFilters() const override;
	virtual int getIconSize() const override;

	void setAssetModel(AbstractItemModel* assetModel);
	void setNameFilters(const std::vector<std::string>& nameFilters);
	void setIconSize(int iconSize);

private:
	AbstractItemModel* assetModel_;
	std::vector<std::string> nameFilters_;
	int iconSize_;
};
}
} // end namespace wgt
#endif // ASSET_BROWSER_MODEL20_H_
