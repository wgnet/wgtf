#include "file_system_model.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_serialization/i_file_system.hpp"

#include <string>
#include <unordered_map>
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

namespace wgt
{
ITEMROLE(isDirectory)
ITEMROLE(isHidden)
ITEMROLE(fullPath)
ITEMROLE(directory)
ITEMROLE(absolutePath)
ITEMROLE(readOnly)
ITEMROLE(compressed)
ITEMROLE(modified)
ITEMROLE(extension)
ITEMROLE(size)
ITEMROLE(thumbnail)
ITEMROLE(collapsedFolderIcon)
ITEMROLE(expandedFolderIcon)


namespace FileSystemModelDetails
{
static const std::string s_RolesArr[] = 
{ 
	ItemRole::thumbnailName,
	ItemRole::isDirectoryName, 
	ItemRole::fullPathName,
	ItemRole::absolutePathName,
	ItemRole::isHiddenName,
	ItemRole::directoryName,
	ItemRole::readOnlyName,
	ItemRole::compressedName,
	ItemRole::modifiedName,
	ItemRole::sizeName,
	ItemRole::collapsedFolderIconName,
	ItemRole::expandedFolderIconName,
	ItemRole::decorationName,
	IndexPathRole::roleName_ 
};
static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
                                                 &s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);

} // end namespace FileSystemModelDetails

namespace
{
struct FileItem;
typedef std::vector<std::unique_ptr<FileItem>> FileItems;

struct ModelSignals
{
	Signal<AbstractTreeModel::DataSignature> preItemDataChanged_;
	Signal<AbstractTreeModel::DataSignature> postItemDataChanged_;
};

struct FileItem : public AbstractTreeItem
{
	enum EventType
	{
		PRE_DATA_CHANGED,
		POST_DATA_CHANGED
	};

	struct ItemSignals
	{
		Signal<DataSignature> preDataChanged_;
		Signal<DataSignature> postDataChanged_;
	};

	FileItem(IFileInfoPtr& fileInfo, const FileItem* parent)
		: fileInfo_(fileInfo)
		, parent_(parent)
	{
	}

	Variant getData(int column, ItemRole::Id roleId) const override
	{
		static SharedString s_Folder("folder");

		if (roleId == ItemRole::displayId)
		{
			if (parent_ == nullptr)
			{
				static SharedString s_Root("root");
				return s_Root;
			}
			return fileInfo_->name();
		}

		if (roleId == ItemRole::decorationId)
		{
			if (fileInfo_->isDirectory())
			{
				return s_Folder;
			}
			return fileInfo_->extension();
		}

		if (roleId == ItemRole::collapsedFolderIconId)
		{
			return s_Folder;
		}

		if (roleId == ItemRole::expandedFolderIconId)
		{
			static SharedString s_FolderOpen("folder_open");
			return s_FolderOpen;
		}

		if (roleId == IndexPathRole::roleId_)
		{
			return fileInfo_->fullPath();
		}

		if (roleId == ItemRole::thumbnailId)
		{
			DependsLocal<IUIFramework> depends;
			auto uiFramework = depends.get<IUIFramework>();
			if (uiFramework == nullptr)
			{
				return Variant();
			}
			auto thumbnailFile = fileInfo_->fullPath();
			if (!uiFramework->hasThumbnail(thumbnailFile.str().c_str()))
			{
				return Variant();
			}
			return thumbnailFile;
		}

		if (roleId == ItemRole::isDirectoryId)
		{
			return fileInfo_->isDirectory();
		}

		if (roleId == ItemRole::isHiddenId)
		{
			return fileInfo_->isHidden();
		}

		if (roleId == ItemRole::fullPathId)
		{
			return fileInfo_->fullPath();
		}

		if (roleId == ItemRole::absolutePathId)
		{
			return fileInfo_->absolutePath();
		}

		if (roleId == ItemRole::directoryId)
		{
			return fileInfo_->isDirectory() ? 
				fileInfo_->fullPath() : 
				FilePath::getFolder(fileInfo_->fullPath().str());
		}

		if (roleId == ItemRole::readOnlyId)
		{
			return fileInfo_->isReadOnly();
		}

		if (roleId == ItemRole::compressedId)
		{
			return (fileInfo_->attributes() & FileAttributes::Compressed) != 0;
		}

		if (roleId == ItemRole::modifiedId)
		{
			return fileInfo_->modified();
		}

		if (roleId == ItemRole::sizeId)
		{
			return fileInfo_->size();
		}

		return Variant();
	}

	bool setData(int column, ItemRole::Id roleId, const Variant& data) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void invalidate(IFileInfoPtr info)
	{
		auto leftAttributes = fileInfo_->attributes();
		auto rightAttributes = info->attributes();
		bool readOnlyChanged = (leftAttributes & FileAttributes::ReadOnly) != (rightAttributes & FileAttributes::ReadOnly);
		bool hiddenChanged = (leftAttributes & FileAttributes::Hidden) != (rightAttributes & FileAttributes::Hidden);
		bool leftCompressed = (leftAttributes & FileAttributes::Compressed) != 0;
		bool rightCompressed = (rightAttributes & FileAttributes::Compressed) != 0;
		bool compressedChanged = leftCompressed != rightCompressed;

		fileInfo_ = info;

		if (readOnlyChanged)
		{
			fireDataChanged(PRE_DATA_CHANGED, *this, 0, ItemRole::readOnlyId, fileInfo_->isReadOnly());
			fireDataChanged(POST_DATA_CHANGED, *this, 0, ItemRole::readOnlyId, fileInfo_->isReadOnly());
		}

		if (hiddenChanged)
		{
			fireDataChanged(PRE_DATA_CHANGED, *this, 0, ItemRole::isHiddenId, fileInfo_->isHidden());
			fireDataChanged(POST_DATA_CHANGED, *this, 0, ItemRole::isHiddenId, fileInfo_->isHidden());
		}

		if (compressedChanged)
		{
			fireDataChanged(PRE_DATA_CHANGED, *this, 0, ItemRole::compressedId, leftCompressed);
			fireDataChanged(POST_DATA_CHANGED, *this, 0, ItemRole::compressedId, rightCompressed);
		}
	}


	ItemSignals & getItemSignals()
	{
		if (signals_ == nullptr)
		{
			signals_ = std::make_unique< ItemSignals >();
		}
		return *signals_;
	}

	virtual Connection connectPreDataChanged(DataCallback callback) override
	{
		return getItemSignals().preDataChanged_.connect(callback);
	}

	virtual Connection connectPostDataChanged(DataCallback callback) override
	{
		return getItemSignals().postDataChanged_.connect(callback);
	}

	virtual const ModelSignals & getModelSignals() const
	{
		TF_ASSERT(parent_);
		return parent_->getModelSignals();
	}


	static void fireDataChanged(
		EventType type, const FileItem & item, 
		int column, ItemRole::Id role, const Variant& value )
	{
		int row = 0;
		if (item.parent_ != nullptr)
		{
			auto findIt = std::find_if(item.parent_->children_->begin(), item.parent_->children_->end(), [&item]( const std::unique_ptr< FileItem > & ptr)
			{
				return &item == ptr.get();
			});
			if (findIt != item.parent_->children_->end())
			{
				row = ( int ) std::distance(item.parent_->children_->begin(), findIt);
			}
		}
		AbstractTreeModel::ItemIndex index(row, item.parent_);

		auto && signals = item.getModelSignals();
		if (type == PRE_DATA_CHANGED )
		{
			signals.preItemDataChanged_(index, column, role, value);
			if (item.signals_)
			{
				item.signals_->preDataChanged_(column, role, value);
			}
		}
		else if (type == POST_DATA_CHANGED)
		{
			signals.postItemDataChanged_(index, column, role, value);
			if (item.signals_)
			{
				item.signals_->postDataChanged_(column, role, value);
			}
		}
	}

	bool matchesPath(const char* path)
	{
		return strcmp(path, fileInfo_->fullPath()->c_str()) == 0;
	}

	IFileInfoPtr fileInfo_;
	const FileItem* parent_;
	std::unique_ptr<FileItems> children_;
	std::unique_ptr< ItemSignals > signals_;
};
}

//==============================================================================
struct RootFileItem
	: public FileItem
{
	RootFileItem( const ModelSignals & signals, IFileInfoPtr && fileInfo )
		: FileItem( fileInfo, nullptr )
		, signals_(signals)
	{
	}

	const ModelSignals & getModelSignals() const override
	{
		return signals_;
	}

	const ModelSignals & signals_;
};

//==============================================================================
struct FileSystemModel::Impl : DependsLocal<IUIFramework>
{
	Impl(IFileSystem& fileSystem, const char* rootDirectory) : fileSystem_(fileSystem), rootDirectory_(rootDirectory)
	{
		using namespace std::placeholders;
		IFileSystem::PathChangedCallback changeCallback =
			std::bind(&FileSystemModel::Impl::fileChanged, this, _1, _2);
		fileSystem.listenForChanges(changeCallback);

		//register icons
		auto uiFramework = get<IUIFramework>();
		TF_ASSERT(uiFramework != nullptr);
		uiFramework->loadIconData(":/WGControls/wg_file_system_icons.xml", IUIFramework::ResourceType::File);
	}

	const FileItems& getItems(const FileItem* parentItem)
	{
		auto& items = const_cast<std::unique_ptr<FileItems>&>(parentItem == nullptr ? rootItems_ : parentItem->children_);
		if (items != nullptr)
		{
			return *items;
		}

		items.reset(new FileItems());
		if (parentItem == nullptr)
		{
			auto fileInfo = fileSystem_.getFileInfo(rootDirectory_.c_str());

			auto item = new RootFileItem( signals_, std::move(fileInfo));
			items->emplace_back(item);
		}
		else if (parentItem->fileInfo_->isDirectory())
		{
			const auto directory = parentItem->fileInfo_->fullPath()->c_str();
			fileSystem_.enumerate(directory, [&](IFileInfoPtr&& fileInfo) {
				// Skip dots
				if (fileInfo->isDots())
				{
					return true;
				}
				
				int row = int(items->size());
				auto item = new FileItem(std::move(fileInfo), parentItem);
				items->emplace_back(item);
				return true;
			});
		}
		return *items;
	}

	void fileChanged(const char* path, const IFileInfoPtr info)
	{
		if (auto item = findItem(path))
		{
			item->invalidate(info);
			if (item->parent_ != nullptr)
			{
				auto parentPath = item->parent_->fileInfo_->fullPath()->c_str();
				auto parentInfo = fileSystem_.getFileInfo(parentPath);
				fileChanged(parentPath, parentInfo);
			}
		}
	}

	FileItem* findItem(const char* path)
	{
		if (!rootItems_)
		{
			return nullptr;
		}

		return findItem(path, *rootItems_);
	}

	FileItem* findItem(const char* path, FileItems& items)
	{
		for (size_t i = 0; i < items.size(); ++i)
		{
			auto& item = items[i];

			if (!item)
			{
				continue;
			}

			if (item->matchesPath(path))
			{
				return item.get();
			}

			auto& children = items[i]->children_;

			if (!children)
			{
				continue;
			}

			if (FileItem* item = findItem(path, *children))
			{
				return item;
			}
		}

		return nullptr;
	}

	IFileSystem& fileSystem_;
	std::string rootDirectory_;
	mutable std::unique_ptr<FileItems> rootItems_;
	Signal<VoidSignature> preModelReset_;
	Signal<VoidSignature> postModelReset_;
	ModelSignals signals_;
	std::mutex connectionsMutex_;
	std::vector<Connection> connections_;
};

const char* FileSystemModel::s_mimeFilePath = "application/file-path";
const char  FileSystemModel::s_mimeFilePathDelimiter = '\t';

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
	TF_ASSERT(item != nullptr);
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

//------------------------------------------------------------------------------
void FileSystemModel::iterateRoles(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : FileSystemModelDetails::s_RolesVec)
	{
		iterFunc(role.c_str());
	}
}

//------------------------------------------------------------------------------
std::vector<std::string> FileSystemModel::roles() const
{
	return FileSystemModelDetails::s_RolesVec;
}

//------------------------------------------------------------------------------
MimeData FileSystemModel::mimeData(std::vector<AbstractItemModel::ItemIndex>& indices)
{
	MimeData res;
	for (const auto& index : indices)
	{
		auto fileItem = this->item(ItemIndex(index.row_, index.parent_));
		if (fileItem)
		{
			SharedString s;
			const bool wasString = fileItem->getData(index.row_, index.column_, ItemRole::fullPathId).tryCast(s);
			TF_ASSERT(wasString);
			const std::string & name = s.str();
			auto& data = res[s_mimeFilePath];
			// Write strings with \t as separator.
			data.insert(data.end(), name.c_str(), name.c_str() + name.size());
			data.push_back(s_mimeFilePathDelimiter);
		}
	}
	return res;
}

//------------------------------------------------------------------------------
void FileSystemModel::iterateMimeTypes(const std::function<void(const char*)>& iterFunc) const
{
	iterFunc(s_mimeFilePath);
}

//------------------------------------------------------------------------------
std::vector<std::string> FileSystemModel::mimeTypes() const
{
	return { s_mimeFilePath };
}

//------------------------------------------------------------------------------
void FileSystemModel::revert()
{
	impl_->preModelReset_();
	impl_->rootItems_.reset();
	impl_->postModelReset_();
}

Connection FileSystemModel::connectPreItemDataChanged(DataCallback callback)
{
	return impl_->signals_.preItemDataChanged_.connect(callback);
}

Connection FileSystemModel::connectPostItemDataChanged(DataCallback callback)
{
	return impl_->signals_.postItemDataChanged_.connect(callback);
}

//------------------------------------------------------------------------------
Connection FileSystemModel::connectPreModelReset(VoidCallback callback)
{
	return impl_->preModelReset_.connect(callback);
}

//------------------------------------------------------------------------------
Connection FileSystemModel::connectPostModelReset(VoidCallback callback)
{
	return impl_->postModelReset_.connect(callback);
}
} // end namespace wgt
