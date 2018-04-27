
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_model20.hpp"
#include "i_asset_preview_provider.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_object/managed_object.hpp"
#include <unordered_map>
#include "image_preview.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
namespace AssetBrowser20_Local
{
	class ImageFilePreviewProvider : public IAssetPreviewProvider
	{
	public:
		ImageFilePreviewProvider()
			:imagePreview_(std::make_unique<ImagePreview>())
		{
		}

		virtual ObjectHandle getPreview(const char* filePath) override
		{
			if (!supportPreview(filePath))
			{
				return nullptr;
			}
			imagePreview_->setAssetPath(filePath);
			return imagePreview_.getHandle();
		}

	private:
		bool supportPreview(const char* filePath) const
		{
			DependsLocal<IUIFramework> depends;
			auto uiFramework = depends.get<IUIFramework>();
			if (uiFramework == nullptr)
			{
				return false;
			}
			return uiFramework->hasThumbnail(filePath);
		}

		ManagedObject<ImagePreview> imagePreview_;
	};
}

namespace AssetBrowser20
{

struct AssetBrowserModel::Impl
{
	Impl()
		: assetPreviewProvider_()
		, assetModel_(nullptr)
		, iconSize_(64)
	{
	}

	std::weak_ptr<IAssetPreviewProvider> assetPreviewProvider_;
	AbstractItemModel* assetModel_;
	int iconSize_;
	std::string programSelectedItemPath_;
	Signal<void(Variant&)> programSelectedItemChanged_;
	std::vector<std::string> nameFilters_;
	AssetBrowser20_Local::ImageFilePreviewProvider defaultFilePreviewProvider_;
};

AssetBrowserModel::AssetBrowserModel() 
	: pImpl_(new AssetBrowserModel::Impl)
{
}

AssetBrowserModel::~AssetBrowserModel()
{
}

AbstractItemModel* AssetBrowserModel::getAssetModel() const
{
	return pImpl_->assetModel_;
}

const std::vector<std::string>& AssetBrowserModel::getNameFilters() const
{
	return pImpl_->nameFilters_;
}

int AssetBrowserModel::getIconSize() const
{
	return pImpl_->iconSize_;
}

void AssetBrowserModel::programSelectItemByPath(const char* path)
{
	pImpl_->programSelectedItemPath_ = path ? path : "";
	pImpl_->programSelectedItemChanged_(Variant(pImpl_->programSelectedItemPath_.c_str()));
}

const char* AssetBrowserModel::getProgramSelectedItemPath() const
{
	return pImpl_->programSelectedItemPath_.c_str();
}

void AssetBrowserModel::getProgramSelectedItemSignal(Signal<void(Variant&)>** result) const
{
	*result = const_cast<Signal<void(Variant&)>*>(&pImpl_->programSelectedItemChanged_);
}

Variant AssetBrowserModel::assetPreview(std::string assetPath)
{
	ObjectHandle objectHandle = nullptr;

	if (pImpl_->assetPreviewProvider_.lock() != nullptr)
	{
		objectHandle = pImpl_->assetPreviewProvider_.lock()->getPreview(assetPath.c_str());
	}
	else
	{
		objectHandle = pImpl_->defaultFilePreviewProvider_.getPreview(assetPath.c_str());
	}

	if (!objectHandle.isValid())
	{
		return Variant();
	}

	return objectHandle;
}

void AssetBrowserModel::setAssetModel(AbstractItemModel* assetModel)
{
	pImpl_->assetModel_ = assetModel;
}

void AssetBrowserModel::setNameFilters(const std::vector<std::string>& nameFilters)
{
	pImpl_->nameFilters_ = nameFilters;
}

void AssetBrowserModel::setIconSize(int iconSize)
{
	pImpl_->iconSize_ = iconSize;
}

void AssetBrowserModel::setAssetPreviewProvider(std::shared_ptr<IAssetPreviewProvider> assetPreviewProvider)
{
	pImpl_->assetPreviewProvider_ = assetPreviewProvider;
}
}
} // end namespace wgt
