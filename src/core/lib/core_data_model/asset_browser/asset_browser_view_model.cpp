//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_view_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_view_model.hpp"
#include "base_asset_object_item.hpp"
#include "i_asset_browser_model.hpp"
#include "i_asset_browser_event_model.hpp"
#include "asset_browser_breadcrumbs_model.hpp"

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
static const size_t NO_SELECTION = SIZE_MAX;

struct AssetBrowserViewModel::AssetBrowserViewModelImplementation
{
	AssetBrowserViewModelImplementation(IDefinitionManager& definitionManager, ObjectHandleT<IAssetBrowserModel> data,
	                                    ObjectHandleT<IAssetBrowserEventModel> events)
	    : definitionManager_(definitionManager), currentSelectedAssetIndex_(-1),
	      currentFolderHistoryIndex_(NO_SELECTION), selectedTreeItem_(nullptr), data_(std::move(data)),
	      events_(std::move(events)), breadcrumbsModel_(nullptr)
	{
		connections_ += folderSelectionHandler_.signalPostSelectionChanged.connect(
		std::bind(&AssetBrowserViewModel::AssetBrowserViewModelImplementation::onPostFolderDataChanged, this));
		connections_ += folderContentSelectionHandler_.signalPostSelectionChanged.connect(
		std::bind(&AssetBrowserViewModel::AssetBrowserViewModelImplementation::onPostFolderContentDataChanged, this));

		breadcrumbsModel_ =
		std::unique_ptr<AssetBrowserBreadcrumbsModel>(new AssetBrowserBreadcrumbsModel(definitionManager));
	}

	~AssetBrowserViewModelImplementation()
	{
	}

	void onPostFolderDataChanged()
	{
		std::vector<IItem*> items = folderSelectionHandler_.getSelectedItems();
		if (items.empty())
		{
			return;
		}

		assert(items.size() == 1);

		selectedTreeItem_ = items[0];
		data_.get()->populateFolderContents(selectedTreeItem_);

		breadcrumbsModel_->generateBreadcrumbs(selectedTreeItem_, data_->getFolderTreeModel());
	}

	void onPostFolderContentDataChanged()
	{
		std::vector<int> indices = folderSelectionHandler_.getSelectedRows();
		if (indices.empty())
		{
			return;
		}

		assert(indices.size() == 1);

		currentSelectedAssetIndex_ = indices[0];
	}

	IDefinitionManager& definitionManager_;
	int currentSelectedAssetIndex_;
	size_t currentFolderHistoryIndex_;
	IItem* selectedTreeItem_;

	ObjectHandleT<IAssetBrowserModel> data_;
	ObjectHandleT<IAssetBrowserEventModel> events_;
	std::unique_ptr<AssetBrowserBreadcrumbsModel> breadcrumbsModel_;

	SelectionHandler folderSelectionHandler_;
	SelectionHandler folderContentSelectionHandler_;

	ConnectionHolder connections_;
};

AssetBrowserViewModel::AssetBrowserViewModel(IDefinitionManager& definitionManager,
                                             ObjectHandleT<IAssetBrowserModel> data,
                                             ObjectHandleT<IAssetBrowserEventModel> events)
    : impl_(new AssetBrowserViewModelImplementation(definitionManager, std::move(data), std::move(events)))
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
	// TODO: This will need to support multi-selection. Right now it is a single item
	// selection, but the QML is the same way.
	auto dataModel = impl_->data_.get();
	if (dataModel != nullptr)
	{
		return dataModel->getFolderContentsAtIndex(impl_->currentSelectedAssetIndex_);
	}

	return nullptr;
}

const char* AssetBrowserViewModel::getSelectedTreeItemName()
{
	if (impl_->selectedTreeItem_ != nullptr)
	{
		return impl_->selectedTreeItem_->getDisplayText(0);
	}

	return nullptr;
}

bool AssetBrowserViewModel::refreshData() const
{
	if (impl_->selectedTreeItem_ != nullptr)
	{
		impl_->data_->populateFolderContents(impl_->selectedTreeItem_);
	}

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

ISelectionHandler* AssetBrowserViewModel::getFolderSelectionHandler() const
{
	return &impl_->folderSelectionHandler_;
}

ISelectionHandler* AssetBrowserViewModel::getFolderContentSelectionHandler() const
{
	return &impl_->folderContentSelectionHandler_;
}
} // end namespace wgt
