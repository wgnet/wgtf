
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_dialog_model.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_DIALOG_MODEL_H
#define ASSET_BROWSER_DIALOG_MODEL_H

#pragma once

#include "core_common/signal.hpp"
#include "core_data_model/dialog/dialog_model.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{

class AbstractItemModel;

namespace AssetBrowser20
{
class IAssetBrowserModel;

class AssetBrowserDialogModel : public DialogModel, Depends<IAssetBrowserModel>
{
	DECLARE_REFLECTED;

public:
	using AssetSignature = void (const std::string& assetPath);
	using AssetCallback = std::function<AssetSignature>;
	using NameFilters = std::vector<std::string>;

	AssetBrowserDialogModel();

	Connection			connectAssetAccepted(AssetCallback callback);
	void				assetAccepted(const std::string& assetPath);
	AbstractItemModel*	assetModel() const;
	const char*			getNameFilter() const;
	void				setNameFilter(const char* nameFilter);
	const NameFilters&	getNameFilters() const;
	void				setNameFilters(NameFilters nameFilters);
	const char*			getSelectedAsset() const;
	void				setSelectedAsset(const char* selectedAsset);

private:
	Signal<AssetSignature> assetAccepted_;
	bool startVisible_ = true;
	std::string nameFilter_ = "*.*";
	std::vector<std::string> nameFilters_ = { "*.*" };
};

} // namespace AssetBrowser20
} // namespace wgt
#endif // ASSET_BROWSER_DIALOG_MODEL_H