#ifndef I_ASSET_BROWSER_MODEL20_HPP
#define I_ASSET_BROWSER_MODEL20_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_common/signal.hpp"

#include <string>
#include <vector>
#include <functional>

namespace wgt
{
class AbstractItemModel;

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

	//-------------------------------------
	// Signal Connections
	//-------------------------------------
	typedef void AssetSignature(const std::string& assetPath);
	typedef std::function<AssetSignature> AssetCallback;

	Connection connectAssetAccepted(AssetCallback callback)
	{
		return assetAccepted_.connect(callback);
	}

	// TODO: @b_harding Investigate why friend class in DECLARE_REFLECTED isn't enough to enable EXPOSE_METHOD in OSX
	void assetAccepted(std::string assetPath)
	{
		assetAccepted_(assetPath);
	}

private:
	Signal<AssetSignature> assetAccepted_;
};
} // end namespace AssetBrowser20
} // end namespace wgt
#endif // I_ASSET_BROWSER_MODEL20_HPP
