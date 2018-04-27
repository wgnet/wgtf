
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
#include "core_dependency_system/i_interface.hpp"

#include <memory>

namespace wgt
{
namespace AssetBrowser20
{
class AssetBrowserModel
	: public Implements< IAssetBrowserModel >
{
public:
	AssetBrowserModel();
	virtual ~AssetBrowserModel();

	virtual AbstractItemModel* getAssetModel() const override;
	virtual const std::vector<std::string>& getNameFilters() const override;
	virtual int getIconSize() const override;
	virtual Variant assetPreview(std::string assetPath) override;

	virtual void programSelectItemByPath(const char* path) override;
	virtual const char* getProgramSelectedItemPath() const override;
	virtual void getProgramSelectedItemSignal(Signal<void(Variant&)>** result) const override;

	void setAssetModel(AbstractItemModel* assetModel) override;
	void setNameFilters(const std::vector<std::string>& nameFilters) override;
	void setIconSize(int iconSize) override;
	void setAssetPreviewProvider(std::shared_ptr<IAssetPreviewProvider> assetPreviewProvider) override;

private:
	struct Impl;
	std::unique_ptr<Impl> pImpl_;
};
}
} // end namespace wgt
#endif // ASSET_BROWSER_MODEL20_H_
