#include "folder_tree_model.hpp"
#include "base_asset_object_item.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_logging/logging.hpp"
#include "core_data_model/i_item_role.hpp"

#include <vector>

namespace wgt
{
typedef std::unique_ptr<IItem> IItem_uptr;

struct FolderTreeModel::Implementation
{
	Implementation(FolderTreeModel& main, IFileSystem& fileSystem);
	~Implementation();

	std::vector<IAssetObjectItem*> getSection(const IAssetObjectItem* parent);
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

void FolderTreeModel::setAssetPaths(const std::vector<std::string>& paths)
{
	for (auto& path : paths)
	{
		impl_->generateData(nullptr, path);
	}

	if (!impl_->roots_.empty())
		impl_->model_->populateFolderContents(impl_->roots_[0].get());
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

IItem* FolderTreeModel::item(size_t index, const IItem* parent) const
{
	auto temp = static_cast<const IAssetObjectItem*>(parent);
	return temp ? (*temp)[index] : const_cast<IItem*>(impl_->roots_[index].get());
}

ITreeModel::ItemIndex FolderTreeModel::index(const IItem* item) const
{
	if (!item)
		return ItemIndex(0, nullptr);

	auto temp = static_cast<const IAssetObjectItem*>(item);
	temp = static_cast<const IAssetObjectItem*>(temp->getParent());
	if (temp)
		return ItemIndex(temp->indexOf(item), temp);

	auto found = std::find_if(impl_->roots_.begin(), impl_->roots_.end(),
	                          [&](const IItem_uptr& i) { return i.get() == item; });

	return ItemIndex(found - impl_->roots_.begin(), temp);
}

bool FolderTreeModel::empty(const IItem* parent) const
{
	auto temp = static_cast<const IAssetObjectItem*>(parent);
	return temp ? temp->empty() : impl_->roots_.empty();
}

size_t FolderTreeModel::size(const IItem* parent) const
{
	auto temp = static_cast<const IAssetObjectItem*>(parent);
	return temp ? temp->size() : impl_->roots_.size();
}

int FolderTreeModel::columnCount() const
{
	return 1;
}
} // end namespace wgt
