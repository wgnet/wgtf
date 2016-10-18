//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_event_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_event_model20.hpp"
#include "base_asset_object_item20.hpp"
#include "core_variant/variant.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
namespace AssetBrowser20
{
template <class Type>
Type* get(const Variant& selectedAsset)
{
	// Downcasting via static_cast should be allowed so long as the selected asset derives
	// from IAssetObjectItem, which is an IItem. The Asset Browser API won't work without
	// IAssetObjectItem, so this is a safe bet.
	auto listItem = reinterpret_cast<AbstractItem*>(selectedAsset.value<intptr_t>());
	return static_cast<Type*>(listItem);
}

void AssetBrowserEventModel::assetSelectionChanged(const Variant& selectedAsset)
{
	auto asset = get<IAssetObjectItem>(selectedAsset);
	if (asset)
		signalAssetSelectionChanged(*asset);
}

void AssetBrowserEventModel::breadcrumbSelected(const Variant& breadcrumb)
{
	signalBreadcrumbSelected(breadcrumb);
}

void AssetBrowserEventModel::folderSelectionChanged(const Variant& folderSelection)
{
	signalFolderSelectionChanged(folderSelection);
}

void AssetBrowserEventModel::useSelectedAsset(const Variant& selectedAsset)
{
	auto asset = get<IAssetObjectItem>(selectedAsset);
	if (asset)
		signalUseSelectedAsset(*asset);
}

void AssetBrowserEventModel::connectAssetSelectionChanged(AssetCallback callback)
{
	signalAssetSelectionChanged.connect(callback);
}

void AssetBrowserEventModel::connectBreadcrumbSelected(VariantCallback callback)
{
	signalBreadcrumbSelected.connect(callback);
}

void AssetBrowserEventModel::connectFolderSelectionChanged(VariantCallback callback)
{
	signalFolderSelectionChanged.connect(callback);
}

void AssetBrowserEventModel::connectUseSelectedAsset(AssetCallback callback)
{
	signalUseSelectedAsset.connect(callback);
}
}
} // end namespace wgt
