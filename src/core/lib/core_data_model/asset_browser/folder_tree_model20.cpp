#include "folder_tree_model20.hpp"
#include "base_asset_object_item20.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_logging/logging.hpp"
#include "core_data_model/i_item_role.hpp"

#include <vector>

namespace wgt
{
typedef std::unique_ptr<AbstractItem> IItem_uptr;
namespace AssetBrowser20
{
struct FolderTreeModel::Implementation
{
	Implementation(FolderTreeModel& main, IFileSystem& fileSystem);
	~Implementation();

	void generateData(const IAssetObjectItem* parent, const std::string& path);

	FolderTreeModel& main_;
	IFileSystem& fileSystem_;
	IAssetBrowserModel* model_;
	std::vector<IItem_uptr> roots_;
};

FolderTreeModel::Implementation::Implementation(FolderTreeModel& main, IFileSystem& fileSystem)
    : main_(main)
    , fileSystem_(fileSystem)
    , model_(nullptr)
{
}

FolderTreeModel::Implementation::~Implementation()
{
}

void FolderTreeModel::Implementation::generateData(const IAssetObjectItem* parent, const std::string& path)
{
	auto info = fileSystem_.getFileInfo(path.c_str());
	if ((info->attributes() != FileAttributes::None) && !info->isDots() && !info->isHidden())
	{
		roots_.emplace_back(new BaseAssetObjectItem(info, nullptr, &fileSystem_, nullptr));
	}
}

FolderTreeModel::FolderTreeModel(IAssetBrowserModel& model, IFileSystem& fileSystem)
    : impl_(new Implementation(*this, fileSystem))
{
	init(&model);
}

FolderTreeModel::FolderTreeModel(const FolderTreeModel& rhs)
    : impl_(new Implementation(*this, rhs.impl_->fileSystem_))

{
	init(rhs.model());
}

void FolderTreeModel::init(IAssetBrowserModel* model)
{
	impl_->model_ = model;
	setAssetPaths(model->assetPaths());
}

IAssetBrowserModel* FolderTreeModel::model() const
{
	return impl_->model_;
}

wgt::AbstractItem* FolderTreeModel::item(const ItemIndex& index) const
{
	auto temp = static_cast<const IAssetObjectItem*>(index.parent_);
	return temp ? (*temp)[index.row_] : static_cast<IAssetObjectItem*>(impl_->roots_[index.row_].get());
}

wgt::AbstractTreeModel::ItemIndex FolderTreeModel::index(const AbstractItem* item) const
{
	assert(item != nullptr);
	auto fileItem = static_cast<const IAssetObjectItem*>(item);
	auto parentItem = fileItem->getParent();

	if (parentItem != nullptr)
	{
		return ItemIndex(static_cast<int>(parentItem->indexOf(fileItem)), parentItem);
	}

	auto found = std::find_if(impl_->roots_.begin(), impl_->roots_.end(),
	                          [&](const IItem_uptr& i) { return i.get() == item; });

	return ItemIndex(static_cast<int>(found - impl_->roots_.begin()), parentItem);
}

int FolderTreeModel::rowCount(const AbstractItem* item) const
{
	auto fileItem = static_cast<const IAssetObjectItem*>(item);
	if (fileItem == nullptr)
	{
		return static_cast<int>(impl_->roots_.size());
	}
	const auto directory = fileItem->getFullPath();
	int count = 0;
	impl_->fileSystem_.enumerate(directory, [&](IFileInfoPtr&& fileInfo)
	                             {
		                             // Skip dots and hidden files
		                             if (fileInfo->isDots() || fileInfo->isHidden())
		                             {
			                             return true;
		                             }

		                             ++count;
		                             return true;
		                         });
	return count;
}

int FolderTreeModel::columnCount() const
{
	return 1;
}

std::vector<std::string> FolderTreeModel::roles() const
{
	std::vector<std::string> roles;
	roles.push_back(ItemRole::indexPathName);
	roles.push_back(ItemRole::thumbnailName);
	roles.push_back(ItemRole::typeIconName);
	roles.push_back(ItemRole::assetSizeName);
	roles.push_back(ItemRole::createdTimeName);
	roles.push_back(ItemRole::modifiedTimeName);
	roles.push_back(ItemRole::accessedTimeName);
	roles.push_back(ItemRole::isDirectoryName);
	roles.push_back(ItemRole::isReadOnlyName);
	roles.push_back(ItemRole::isCompressedName);
	roles.push_back(ItemRole::itemIdName);

	return roles;
}

void FolderTreeModel::setAssetPaths(const std::vector<std::string>& paths)
{
	for (auto& path : paths)
	{
		impl_->generateData(nullptr, path);
	}

	if (!impl_->roots_.empty())
	{
		auto assetObjectItem = static_cast<BaseAssetObjectItem*>(impl_->roots_[0].get());
		assert(assetObjectItem != nullptr);
		impl_->model_->populateFolderContents(assetObjectItem);
	}
}

FolderTreeModel::~FolderTreeModel()
{
}

FolderTreeModel& FolderTreeModel::operator=(const FolderTreeModel& rhs)
{
	if (this != &rhs)
	{
		impl_.reset(new Implementation(*this, rhs.impl_->fileSystem_));
	}

	return *this;
}

const char* uriMimeType = "text/uri-list";
MimeData FolderTreeModel::mimeData(std::vector<AbstractItemModel::ItemIndex>& indices)
{
	std::string uriList;
	for (auto& index : indices)
	{
		if (index.isValid())
		{
			IAssetObjectItem* assetItem =
			static_cast<IAssetObjectItem*>(item(ItemIndex(index.row_, index.parent_)));
			const char* fullPath = assetItem->getFullPath();
			uriList += "file:///";
			uriList += fullPath;
			uriList += "\r\n";
		}
	}

	MimeData result;
	if (uriList.size() > 0)
	{
		result[uriMimeType] = std::vector<char>(uriList.begin(), uriList.end());
	}
	return result;
}

std::vector<std::string> FolderTreeModel::mimeTypes() const
{
	std::vector<std::string> result;
	result.push_back(uriMimeType);
	return result;
}
}
} // end namespace wgt
