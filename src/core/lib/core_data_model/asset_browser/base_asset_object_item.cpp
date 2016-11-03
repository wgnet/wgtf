#include "base_asset_object_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_data_model/asset_browser/i_asset_presentation_provider.hpp"
#include <vector>

namespace wgt
{
struct BaseAssetObjectItem::Implementation
{
	typedef std::vector<BaseAssetObjectItem> BaseAssetObjectItems;

	Implementation(BaseAssetObjectItem& self, const IFileInfoPtr& fileInfo, const IItem* parent,
	               IFileSystem* fileSystem, IAssetPresentationProvider* presentationProvider)
	    : self_(self)
	    , fileInfo_(fileInfo)
	    , parent_(parent)
	    , fileSystem_(fileSystem)
	    , presentationProvider_(presentationProvider)
	    , displayText_("")
	{
	}

	BaseAssetObjectItems& getChildren() const
	{
		// Not currently thread safe, only one thread can initialize the children
		if (fileSystem_ != nullptr && fileInfo_->isDirectory() && children_.empty())
		{
			fileSystem_->enumerate(fileInfo_->fullPath(), [&](IFileInfoPtr&& info)
			                       {
				                       if (!info->isDots() && !(info->attributes() & FileAttributes::Hidden))
					                       children_.emplace_back(info, &self_, fileSystem_, presentationProvider_);
				                       return true;
				                   });
		}
		return children_;
	}

	BaseAssetObjectItem& self_;
	IFileInfoPtr fileInfo_;
	const IItem* parent_;
	IFileSystem* fileSystem_;
	IAssetPresentationProvider* presentationProvider_;
	mutable BaseAssetObjectItems children_;
	std::string displayText_;
};

BaseAssetObjectItem::BaseAssetObjectItem(const BaseAssetObjectItem& rhs)
    : impl_(new Implementation(*this,
                               rhs.impl_->fileInfo_,
                               rhs.impl_->parent_,
                               rhs.impl_->fileSystem_,
                               rhs.impl_->presentationProvider_))
{
}

BaseAssetObjectItem::BaseAssetObjectItem(const IFileInfoPtr& fileInfo, const IItem* parent,
                                         IFileSystem* fileSystem, IAssetPresentationProvider* assetPresentationProvider)
    : impl_(new Implementation(*this, fileInfo, parent, fileSystem, assetPresentationProvider))
{
}

BaseAssetObjectItem::~BaseAssetObjectItem()
{
}

BaseAssetObjectItem& BaseAssetObjectItem::operator=(const BaseAssetObjectItem& rhs)
{
	if (this != &rhs)
	{
		impl_.reset(new Implementation(*this, rhs.impl_->fileInfo_, rhs.impl_->parent_,
		                               rhs.impl_->fileSystem_, rhs.impl_->presentationProvider_));
	}

	return *this;
}

const IItem* BaseAssetObjectItem::getParent() const
{
	return impl_->parent_;
}

IItem* BaseAssetObjectItem::operator[](size_t index) const
{
	if (impl_->getChildren().size() > index)
	{
		return &impl_->getChildren()[index];
	}

	return nullptr;
}

size_t BaseAssetObjectItem::indexOf(const IItem* item) const
{
	auto& children = impl_->getChildren();
	return static_cast<const BaseAssetObjectItem*>(item) - &*begin(children);
}

bool BaseAssetObjectItem::empty() const
{
	return impl_->getChildren().empty();
}

size_t BaseAssetObjectItem::size() const
{
	return impl_->getChildren().size();
}

const char* BaseAssetObjectItem::getDisplayText(int column) const
{
	if (!isDirectory())
	{
		return getAssetName();
	}

	impl_->displayText_ = impl_->fileInfo_->fullPath();
	auto lastSeparator = impl_->displayText_.find_last_of("/\\");
	if (impl_->displayText_.length() > 1 && lastSeparator != std::string::npos)
	{
		if (lastSeparator == (impl_->displayText_.length() - 1))
			impl_->displayText_.erase(--impl_->displayText_.end());

		impl_->displayText_ = impl_->displayText_.substr(impl_->displayText_.find_last_of("/\\") + 1).c_str();
		return impl_->displayText_.c_str();
	}

	return "";
}

ThumbnailData BaseAssetObjectItem::getThumbnail(int column) const
{
	if (impl_->presentationProvider_ != nullptr)
	{
		return impl_->presentationProvider_->getThumbnail(this);
	}

	return nullptr;
}

ThumbnailData BaseAssetObjectItem::getStatusIconData() const
{
	if (impl_->presentationProvider_ != nullptr)
	{
		return impl_->presentationProvider_->getStatusIconData(this);
	}

	return nullptr;
}

const char* BaseAssetObjectItem::getTypeIconResourceString() const
{
	if (impl_->presentationProvider_ != nullptr)
	{
		return impl_->presentationProvider_->getTypeIconResourceString(this);
	}

	return nullptr;
}

Variant BaseAssetObjectItem::getData(int column, ItemRole::Id roleId) const
{
	if (column != 0)
	{
		return Variant();
	}

	if (roleId == ValueRole::roleId_)
	{
		return getDisplayText(0);
	}
	else if (roleId == IndexPathRole::roleId_)
	{
		return getFullPath();
	}
	else if (roleId == ThumbnailRole::roleId_)
	{
		return getThumbnail(0);
	}
	else if (roleId == TypeIconRole::roleId_)
	{
		return getTypeIconResourceString();
	}
	else if (roleId == SizeRole::roleId_)
	{
		return getSize();
	}
	else if (roleId == CreatedTimeRole::roleId_)
	{
		return getCreatedTime();
	}
	else if (roleId == ModifiedTimeRole::roleId_)
	{
		return getModifiedTime();
	}
	else if (roleId == AccessedTimeRole::roleId_)
	{
		return getAccessedTime();
	}
	else if (roleId == IsDirectoryRole::roleId_)
	{
		return isDirectory();
	}
	else if (roleId == IsReadOnlyRole::roleId_)
	{
		return isReadOnly();
	}
	else if (roleId == IsCompressedRole::roleId_)
	{
		return isCompressed();
	}
	else if (roleId == ItemRole::itemIdId)
	{
		return intptr_t(this);
	}

	return Variant();
}

bool BaseAssetObjectItem::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	return false;
}

IFileInfoPtr BaseAssetObjectItem::getFileInfo() const
{
	return impl_->fileInfo_;
}

const char* BaseAssetObjectItem::getAssetName() const
{
	return impl_->fileInfo_->name();
}

uint16_t BaseAssetObjectItem::getAssetType() const
{
	// Developers can and should override this method to return an enum value that is shared with their
	// plugin. Allows access without needing to dynamically cast to a derived type when passing around
	// the IAssetObjectItem base.
	//
	// BaseAssetObjectItem will maintain the extension (type info) in the asset's name.
	return 0;
}

const char* BaseAssetObjectItem::getFullPath() const
{
	return impl_->fileInfo_->fullPath();
}

uint64_t BaseAssetObjectItem::getSize() const
{
	return impl_->fileInfo_->size();
}

uint64_t BaseAssetObjectItem::getCreatedTime() const
{
	return impl_->fileInfo_->created();
}

uint64_t BaseAssetObjectItem::getModifiedTime() const
{
	return impl_->fileInfo_->modified();
}

uint64_t BaseAssetObjectItem::getAccessedTime() const
{
	return impl_->fileInfo_->accessed();
}

bool BaseAssetObjectItem::isDirectory() const
{
	return impl_->fileInfo_->isDirectory();
}

bool BaseAssetObjectItem::isReadOnly() const
{
	return impl_->fileInfo_->isReadOnly();
}

bool BaseAssetObjectItem::isCompressed() const
{
	return (impl_->fileInfo_->attributes() & FileAttributes::Compressed) == FileAttributes::Compressed;
}
} // end namespace wgt
