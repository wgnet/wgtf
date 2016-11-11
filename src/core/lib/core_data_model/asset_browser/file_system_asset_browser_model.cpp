
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "file_system_asset_browser_model.hpp"

#include "core_data_model/asset_browser/asset_list_model.hpp"
#include "core_data_model/asset_browser/base_asset_object_item.hpp"
#include "core_data_model/asset_browser/folder_tree_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/value_change_notifier.hpp"
#include "core_data_model/simple_active_filters_model.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_logging/logging.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_reflection/type_class_definition.hpp"

#include <list>

namespace wgt
{
static const int NO_SELECTION = -1;

struct FileSystemAssetBrowserModel::FileSystemAssetBrowserModelImplementation
{
	FileSystemAssetBrowserModelImplementation(FileSystemAssetBrowserModel& self, IFileSystem& fileSystem,
	                                          IDefinitionManager& definitionManager,
	                                          IAssetPresentationProvider& presentationProvider)
	    : self_(self), folders_(nullptr), activeFiltersModel_(nullptr), definitionManager_(definitionManager),
	      presentationProvider_(presentationProvider), fileSystem_(fileSystem), folderContentsFilter_(""),
	      contentFilterIndexNotifier_(NO_SELECTION), currentCustomFilterIndex_(-1), iconSize_(64)
	{
	}

	void addFolderItem(const IFileInfoPtr& fileInfo)
	{
		if (self_.fileHasFilteredExtension(fileInfo))
		{
			auto item = new BaseAssetObjectItem(fileInfo, nullptr, nullptr, &presentationProvider_);
			folderContents_.push_back(item);
		}
	}

	IAssetObjectItem* getFolderContentsAtIndex(const int& index)
	{
		if (index < 0 || index >= (int)folderContents_.size())
		{
			return nullptr;
		}

		return &folderContents_[index];
	}

	FileSystemAssetBrowserModel& self_;
	AssetListModel folderContents_;
	VariantList customContentFilters_;
	std::shared_ptr<ITreeModel> folders_;
	std::unique_ptr<IActiveFiltersModel> activeFiltersModel_;

	IDefinitionManager& definitionManager_;
	IAssetPresentationProvider& presentationProvider_;
	IFileSystem& fileSystem_;
	AssetPaths assetPaths_;
	std::string folderContentsFilter_;

	ValueChangeNotifier<int> contentFilterIndexNotifier_;
	int currentCustomFilterIndex_;
	int iconSize_;
};

FileSystemAssetBrowserModel::FileSystemAssetBrowserModel(const AssetPaths& assetPaths,
                                                         const CustomContentFilters& customContentFilters,
                                                         IFileSystem& fileSystem, IDefinitionManager& definitionManager,
                                                         IAssetPresentationProvider& presentationProvider)
    : impl_(new FileSystemAssetBrowserModelImplementation(*this, fileSystem, definitionManager, presentationProvider))
{
	for (auto& path : assetPaths)
	{
		addAssetPath(path);
	}

	for (auto& filter : customContentFilters)
	{
		addCustomContentFilter(filter);
	}

	// Create the FolderTreeModel now that we've added our asset paths
	impl_->folders_.reset(new FolderTreeModel(*this, impl_->fileSystem_));
}

FileSystemAssetBrowserModel::~FileSystemAssetBrowserModel()
{
	finalise();
}

void FileSystemAssetBrowserModel::finalise()
{
	impl_ = nullptr;
}

void FileSystemAssetBrowserModel::addAssetPath(const std::string& path)
{
	if (std::find(impl_->assetPaths_.begin(), impl_->assetPaths_.end(), path) == impl_->assetPaths_.end())
	{
		if (!impl_->fileSystem_.exists(path.c_str()))
		{
			NGT_ERROR_MSG("TestAssetBrowserModel::addAssetPath: "
			              "asset folder path does not exist: %s\n",
			              path.c_str());
			return;
		}

		impl_->assetPaths_.push_back(path);
	}
}

void FileSystemAssetBrowserModel::addCustomContentFilter(const std::string& filter)
{
	impl_->customContentFilters_.push_back(filter.c_str());
}

void FileSystemAssetBrowserModel::initialise(IComponentContext& contextManager, IDefinitionManager& definitionManager)
{
	auto uiFramework = contextManager.queryInterface<IUIFramework>();

	impl_->activeFiltersModel_ = std::unique_ptr<IActiveFiltersModel>(
	new SimpleActiveFiltersModel("AssetBrowserFilter", definitionManager, *uiFramework));
}

const AssetPaths& FileSystemAssetBrowserModel::assetPaths() const
{
	return impl_->assetPaths_;
}

void FileSystemAssetBrowserModel::populateFolderContents(const IItem* item)
{
	impl_->folderContents_.clear();
	if (item)
	{
		auto folderItem = dynamic_cast<const BaseAssetObjectItem*>(item);
		if (folderItem)
		{
			std::vector<std::string> paths;
			auto fileInfo = folderItem->getFileInfo();
			paths.push_back(fileInfo->fullPath());
			addFolderItems(paths);
		}
	}
}

bool FileSystemAssetBrowserModel::fileHasFilteredExtension(const IFileInfoPtr& fileInfo)
{
	std::string fileExtensionFilter;
	getSelectedCustomFilterText(fileExtensionFilter);

	if (fileExtensionFilter.length() < 1 || fileExtensionFilter.compare("*.*") == 0)
	{
		// No filter being applied.
		// Note: Qt ComboBox does not support selecting an empty string value. *.* is hardcoded until a better
		//       solution is made available.
		return true;
	}

	return (std::strcmp(fileInfo->extension(), fileExtensionFilter.c_str()) == 0);
}

IAssetObjectItem* FileSystemAssetBrowserModel::getFolderContentsAtIndex(const int& index) const
{
	return impl_->getFolderContentsAtIndex(index);
}

void FileSystemAssetBrowserModel::getSelectedCustomFilterText(std::string& value) const
{
	// Note: Since it is likely this particular feature will be rolled into a more robust filtering
	//       system later, it will not be plopped into the impl_ to make it easier to remove later.
	int index = impl_->currentCustomFilterIndex_;
	if (index < 0 || index >= (int)impl_->customContentFilters_.size())
	{
		return;
	}

	auto& variant = impl_->customContentFilters_[index];
	if (variant.typeIs<const char*>() || variant.typeIs<std::string>())
	{
		variant.tryCast(value);
	}
}

IListModel* FileSystemAssetBrowserModel::getFolderContents() const
{
	return &impl_->folderContents_;
}

ITreeModel* FileSystemAssetBrowserModel::getFolderTreeModel() const
{
	return impl_->folders_.get();
}

IListModel* FileSystemAssetBrowserModel::getCustomContentFilters() const
{
	return &impl_->customContentFilters_;
}

const int& FileSystemAssetBrowserModel::currentCustomContentFilter() const
{
	return impl_->currentCustomFilterIndex_;
}

void FileSystemAssetBrowserModel::currentCustomContentFilter(const int& index)
{
	impl_->currentCustomFilterIndex_ = index;
	impl_->contentFilterIndexNotifier_.value(index);
}

IActiveFiltersModel* FileSystemAssetBrowserModel::getActiveFiltersModel() const
{
	return impl_->activeFiltersModel_.get();
}

void FileSystemAssetBrowserModel::setFolderContentsFilter(const std::string filter)
{
	impl_->folderContentsFilter_ = filter;
}

const int& FileSystemAssetBrowserModel::getIconSize() const
{
	return impl_->iconSize_;
}

void FileSystemAssetBrowserModel::setIconSize(const int& size)
{
	impl_->iconSize_ = size;
}

IValueChangeNotifier* FileSystemAssetBrowserModel::customContentFilterIndexNotifier() const
{
	return &impl_->contentFilterIndexNotifier_;
}

void FileSystemAssetBrowserModel::addFolderItems(const AssetPaths& paths)
{
	IFileSystem& fs = impl_->fileSystem_;

	std::list<std::string> directories;

	for (auto& path : paths)
	{
		if (!fs.exists(path.c_str()))
		{
			NGT_WARNING_MSG("FileSystemAssetBrowserModel::addFolderItems: "
			                "asset folder path does not exist: %s\n",
			                path.c_str());
			continue;
		}

		directories.push_back(path);
	}

	while (!directories.empty())
	{
		const std::string& dir = directories.front();

		fs.enumerate(dir.c_str(), [&](IFileInfoPtr&& info) {
			if (!info->isDirectory())
			{
				impl_->addFolderItem(info);
			}
			// TODO: For search/filtering we should add all resources on a separate thread
			// We don't want to block the main UI thread
			// For now do not add sub-folders to avoid performance issues
			// else if (!info.isDots())
			//{
			//	directories.push_back( info.fullPath );
			//}
			return true;
		});

		directories.pop_front();
	}
}

Variant FileSystemAssetBrowserModel::findAssetWithPath(std::string path)
{
	// We received a request to find an item from the QML. Use the path to search the folder tree model.
	auto asset = getAssetAtPath(path.c_str());
	if (asset != nullptr)
	{
		return Variant(reinterpret_cast<intptr_t>(asset));
	}

	return Variant();
}

IAssetObjectItem* FileSystemAssetBrowserModel::getAssetAtPath(const char* path, IAssetObjectItem* parent) const
{
	auto assetTree = dynamic_cast<FolderTreeModel*>(impl_->folders_.get());
	if (assetTree == nullptr)
	{
		return nullptr;
	}

	IAssetObjectItem* treeItem = nullptr;

	size_t count = assetTree->size(parent);
	for (size_t i = 0; i < count; ++i)
	{
		treeItem = dynamic_cast<IAssetObjectItem*>(assetTree->item(i, parent));
		if (treeItem != nullptr && strcmp(treeItem->getFullPath(), path) == 0)
		{
			// Match found!
			return treeItem;
		}

		// No match. Use this tree item as the next search step.
		auto result = getAssetAtPath(path, treeItem);
		if (result)
		{
			return result;
		}
	}

	return nullptr;
}
} // end namespace wgt
