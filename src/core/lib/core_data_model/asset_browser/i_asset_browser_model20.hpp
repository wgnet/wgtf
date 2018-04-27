#ifndef I_ASSET_BROWSER_MODEL20_HPP
#define I_ASSET_BROWSER_MODEL20_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_common/signal.hpp"
#include "core_variant/variant.hpp"

#include <string>
#include <vector>
#include <functional>

namespace wgt
{
class AbstractItemModel;
class Variant;
class IAssetPreviewProvider;

namespace AssetBrowser20
{
//------------------------------------------------------------------------------
// IAssetBrowserModel
//
// Represents the data model for the asset browser panel. Developers must
// implement their own versions depending on the type of system their tools
// use to access assets be it a loose file or resource pack file system.
//------------------------------------------------------------------------------
class IAssetBrowserModel
{
	DECLARE_REFLECTED

public:
	//-------------------------------------
	// Lifecycle
	//-------------------------------------
	virtual ~IAssetBrowserModel()
	{
	}

	//-------------------------------------
	// QML Exposed Methods/Properties
	//-------------------------------------
	virtual AbstractItemModel* getAssetModel() const = 0;
	virtual const std::vector<std::string>& getNameFilters() const = 0;
	virtual int getIconSize() const = 0;
	virtual Variant assetPreview(std::string assetPath) = 0;

	/** Allows selecting an item from the browser programatically */
	virtual void programSelectItemByPath(const char* path) = 0;
	virtual const char* getProgramSelectedItemPath() const = 0;
	virtual void getProgramSelectedItemSignal(Signal<void(Variant&)>** result) const = 0;

	virtual void setAssetModel(AbstractItemModel* assetModel) = 0;
	virtual void setNameFilters(const std::vector<std::string>& nameFilters) = 0;
	virtual void setIconSize(int iconSize) = 0;
	virtual void setAssetPreviewProvider(std::shared_ptr<IAssetPreviewProvider> assetPreviewProvider) = 0;

	//-------------------------------------
	// Signal Connections
	//-------------------------------------
	typedef void AssetSignature(const std::string& assetPath);
	typedef std::function<AssetSignature> AssetCallback;


	Connection connectAssetAccepted(AssetCallback callback)
	{
		return assetAccepted_.connect(callback);
	}

	Connection connectCurrentFolderChanged(AssetCallback callback)
	{
		return currentFolderChanged_.connect(callback);
	}

	// TODO: @b_harding Investigate why friend class in DECLARE_REFLECTED isn't enough to enable EXPOSE_METHOD in OSX
	void assetAccepted(std::string assetPath)
	{
		assetAccepted_(assetPath);
	}
	void currentFolderChanged(std::string folderPath)
	{
		currentFolderChanged_(folderPath);
	}

private:
	Signal<AssetSignature> assetAccepted_;
	Signal<AssetSignature> currentFolderChanged_;
};
} // end namespace AssetBrowser20
} // end namespace wgt
#endif // I_ASSET_BROWSER_MODEL20_HPP
