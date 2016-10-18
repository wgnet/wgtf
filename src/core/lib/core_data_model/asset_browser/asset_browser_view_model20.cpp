//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_view_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_view_model20.hpp"
#include "base_asset_object_item20.hpp"
#include "i_asset_browser_model20.hpp"
#include "i_asset_browser_event_model20.hpp"
#include "asset_browser_breadcrumbs_model20.hpp"

#include "core_data_model/variant_list.hpp"
#include "core_data_model/value_change_notifier.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/selection_handler.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/i_file_system.hpp"

#include <sstream>
#include <stdio.h>
#include <vector>

namespace wgt
{
namespace AssetBrowser20
{
static const size_t NO_SELECTION = SIZE_MAX;

struct AssetBrowserViewModel::AssetBrowserViewModelImplementation
{
	AssetBrowserViewModelImplementation(
	IDefinitionManager& definitionManager,
	ObjectHandleT<IAssetBrowserModel> data,
	ObjectHandleT<IAssetBrowserEventModel> events)
	    : definitionManager_(definitionManager)
	    , currentSelectedAssetIndex_(-1)
	    , currentFolderHistoryIndex_(NO_SELECTION)
	    , selectedTreeItem_(nullptr)
	    , data_(std::move(data))
	    , events_(std::move(events))
	    , breadcrumbsModel_(nullptr)
	{
		breadcrumbsModel_ = std::unique_ptr<AssetBrowserBreadcrumbsModel>(new AssetBrowserBreadcrumbsModel(definitionManager));
	}

	~AssetBrowserViewModelImplementation()
	{
	}

	IDefinitionManager& definitionManager_;
	int currentSelectedAssetIndex_;
	size_t currentFolderHistoryIndex_;
	AbstractItem* selectedTreeItem_;

	ObjectHandleT<IAssetBrowserModel> data_;
	ObjectHandleT<IAssetBrowserEventModel> events_;
	std::unique_ptr<AssetBrowserBreadcrumbsModel> breadcrumbsModel_;
};

AssetBrowserViewModel::AssetBrowserViewModel(
IDefinitionManager& definitionManager,
ObjectHandleT<IAssetBrowserModel> data,
ObjectHandleT<IAssetBrowserEventModel> events)
    :
    impl_(new AssetBrowserViewModelImplementation(definitionManager, std::move(data),
                                                  std::move(events)))
{
	if (impl_->events_.get())
	{
		impl_->events_->connectFilterChanged([&](const Variant& filter) { updateFolderContentsFilter(filter); });
	}
}

ObjectHandle AssetBrowserViewModel::data() const
{
	return impl_->data_;
}

ObjectHandle AssetBrowserViewModel::events() const
{
	return impl_->events_;
}

void AssetBrowserViewModel::onItemSelected(std::string assetPath)
{
	auto item = impl_->data_->getAssetAtPath(assetPath.c_str());
	if (item == nullptr)
	{
		return;
	}
	impl_->data_.get()->populateFolderContents(item);

	impl_->breadcrumbsModel_->generateBreadcrumbs(item, impl_->data_->getFolderTreeModel());
}

IBreadcrumbsModel* AssetBrowserViewModel::getBreadcrumbsModel() const
{
	return impl_->breadcrumbsModel_.get();
}

const int& AssetBrowserViewModel::currentSelectedAssetIndex() const
{
	return impl_->currentSelectedAssetIndex_;
}

void AssetBrowserViewModel::currentSelectedAssetIndex(const int& index)
{
	impl_->currentSelectedAssetIndex_ = index;
}

IAssetObjectItem* AssetBrowserViewModel::getSelectedAssetData() const
{
	//TODO: This will need to support multi-selection. Right now it is a single item
	// selection, but the QML is the same way.
	auto dataModel = impl_->data_.get();
	if (dataModel != nullptr)
	{
		return dataModel->getFolderContentsAtIndex(impl_->currentSelectedAssetIndex_);
	}

	return nullptr;
}

bool AssetBrowserViewModel::refreshData() const
{
	return true;
}

void AssetBrowserViewModel::updateFolderContentsFilter(const Variant& filter)
{
	std::string newFilter = "";
	if (filter.typeIs<std::string>())
	{
		bool isOk = filter.tryCast(newFilter);
		if (isOk)
		{
			// Set the new filter for folder contents
			impl_->data_->setFolderContentsFilter(newFilter);

			// Refresh is required to apply the new filter
			refreshData();
		}
	}
}
} // end namespace AssetBrowser20
} // end namespace wgt
