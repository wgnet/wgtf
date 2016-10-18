#include "base_asset_object_item20.hpp"
#include "core_data_model/asset_browser/i_asset_presentation_provider20.hpp"

namespace wgt
{
namespace AssetBrowser20
{
struct BaseAssetObjectItem::Implementation
{
	typedef std::vector<BaseAssetObjectItem> BaseAssetObjectItems;

	Implementation(BaseAssetObjectItem& self, const IFileInfoPtr& fileInfo, const AbstractItem* parent,
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
	const AbstractItem* parent_;
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

BaseAssetObjectItem::BaseAssetObjectItem(const IFileInfoPtr& fileInfo, const AbstractItem* parent,
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

Variant BaseAssetObjectItem::getData(int row, int column, ItemRole::Id roleId) const /* override */
{
	if (roleId == ItemRole::displayId
	    || roleId == ItemRole::valueId)
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
	else if (roleId == ItemRole::indexPathId)
	{
		return getFullPath();
	}
	else if (roleId == ItemRole::thumbnailId)
	{
		//TODO: make image work for Qt's views
		//if (impl_->presentationProvider_ != nullptr)
		//{
		//	return impl_->presentationProvider_->getThumbnail(this);
		//}

		return nullptr;
	}
	else if (roleId == ItemRole::typeIconId)
	{
		return getTypeIconResourceString();
	}
	else if (roleId == ItemRole::assetSizeId)
	{
		return getSize();
	}
	else if (roleId == ItemRole::createdTimeId)
	{
		return getCreatedTime();
	}
	else if (roleId == ItemRole::modifiedTimeId)
	{
		return getModifiedTime();
	}
	else if (roleId == ItemRole::accessedTimeId)
	{
		return getAccessedTime();
	}
	else if (roleId == ItemRole::isDirectoryId)
	{
		return isDirectory();
	}
	else if (roleId == ItemRole::isReadOnlyId)
	{
		return isReadOnly();
	}
	else if (roleId == ItemRole::isCompressedId)
	{
		return isCompressed();
	}
	else if (roleId == ItemRole::decorationId)
	{
		return getStatusIconData();
	}
	else if (roleId == ItemRole::itemIdId)
	{
		return intptr_t(this);
	}

	return AbstractItem::getData(row, column, roleId);
}

bool BaseAssetObjectItem::setData(int row, int column, ItemRole::Id roleId, const Variant& data)
{
	return AbstractItem::setData(row, column, roleId, data);
}

const IAssetObjectItem* BaseAssetObjectItem::getParent() const
{
	return static_cast<const IAssetObjectItem*>(impl_->parent_);
}

IAssetObjectItem* BaseAssetObjectItem::operator[](size_t index) const
{
	if (impl_->getChildren().size() > index)
	{
		return &impl_->getChildren()[index];
	}

	return nullptr;
}

size_t BaseAssetObjectItem::indexOf(const IAssetObjectItem* item) const
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

std::shared_ptr<BinaryBlock> BaseAssetObjectItem::getStatusIconData() const
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
} // end namespace AssetBrowser20
} // end namespace wgt
