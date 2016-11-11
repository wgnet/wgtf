#include "file_system_model.hpp"
#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_serialization/i_file_system.hpp"

#include <string>

namespace wgt
{
ITEMROLE(isDirectory)
ITEMROLE(fullPath)

namespace
{
class FileItem;
typedef std::vector<std::unique_ptr<FileItem>> FileItems;

class FileItem : public AbstractTreeItem
{
public:
	FileItem(const IFileInfoPtr& fileInfo, const FileItem* parent) : fileInfo_(fileInfo), parent_(parent)
	{
	}

	Variant getData(int column, ItemRole::Id roleId) const override
	{
		if (roleId == ItemRole::displayId)
		{
			return fileInfo_->name();
		}

		if (roleId == IndexPathRole::roleId_)
		{
			return fileInfo_->fullPath();
		}

		if (roleId == ItemRole::isDirectoryId)
		{
			return fileInfo_->isDirectory();
		}

		if (roleId == ItemRole::fullPathId)
		{
			return fileInfo_->fullPath();
		}

		return Variant();
	}

	bool setData(int column, ItemRole::Id roleId, const Variant& data) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	const IFileInfoPtr fileInfo_;
	const FileItem* parent_;
	std::unique_ptr<FileItems> children_;
};
}

struct FileSystemModel::Impl
{
	Impl(IFileSystem& fileSystem, const char* rootDirectory) : fileSystem_(fileSystem), rootDirectory_(rootDirectory)
	{
	}

	const FileItems& getItems(const FileItem* parentItem)
	{
		auto& items =
		const_cast<std::unique_ptr<FileItems>&>(parentItem == nullptr ? rootItems_ : parentItem->children_);
		if (items != nullptr)
		{
			return *items;
		}

		items.reset(new FileItems());
		if (parentItem == nullptr || parentItem->fileInfo_->isDirectory())
		{
			const auto directory = parentItem == nullptr ? rootDirectory_ : parentItem->fileInfo_->fullPath();
			fileSystem_.enumerate(directory.c_str(), [&](IFileInfoPtr&& fileInfo) {
				// Skip dots and hidden files
				if (fileInfo->isDots() || fileInfo->isHidden())
				{
					return true;
				}

				items->emplace_back(new FileItem(std::move(fileInfo), parentItem));
				return true;
			});
		}
		return *items;
	}

	IFileSystem& fileSystem_;
	std::string rootDirectory_;
	mutable std::unique_ptr<FileItems> rootItems_;
};

FileSystemModel::FileSystemModel(IFileSystem& fileSystem, const char* rootDirectory)
    : impl_(new Impl(fileSystem, rootDirectory))
{
}

FileSystemModel::~FileSystemModel()
{
}

AbstractItem* FileSystemModel::item(const ItemIndex& index) const
{
	auto parentItem = static_cast<const FileItem*>(index.parent_);
	auto& items = impl_->getItems(parentItem);
	if (static_cast<int>(items.size()) <= index.row_)
	{
		return nullptr;
	}
	return items[index.row_].get();
}

AbstractTreeModel::ItemIndex FileSystemModel::index(const AbstractItem* item) const
{
	assert(item != nullptr);
	auto fileItem = static_cast<const FileItem*>(item);
	auto parentItem = fileItem->parent_;
	auto& items = impl_->getItems(parentItem);

	auto findIt = std::find_if(items.begin(), items.end(),
	                           [&](const std::unique_ptr<FileItem>& value) { return value.get() == fileItem; });

	if (findIt != items.end())
	{
		return ItemIndex(static_cast<int>(std::distance(items.begin(), findIt)), parentItem);
	}

	return ItemIndex();
}

int FileSystemModel::rowCount(const AbstractItem* item) const
{
	auto fileItem = static_cast<const FileItem*>(item);
	auto& items = impl_->getItems(fileItem);
	return static_cast<int>(items.size());
}

int FileSystemModel::columnCount() const
{
	return 1;
}

std::vector<std::string> FileSystemModel::roles() const
{
	auto roleNames = AbstractTreeModel::roles();
	roleNames.push_back(ItemRole::isDirectoryName);
	roleNames.push_back(ItemRole::fullPathName);
	return roleNames;
}
} // end namespace wgt
