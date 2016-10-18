#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

#include "core_common/wg_future.hpp"

namespace wgt
{
class IComponentContext;
class IView;
class IAssetBrowserModel;
class IAssetBrowserEventModel;
namespace AssetBrowser20
{
class IAssetBrowserModel;
class IAssetBrowserEventModel;
}

class IPanelManager
{
public:
	IPanelManager()
	{
	}
	virtual ~IPanelManager()
	{
	}

	virtual wg_future<std::unique_ptr<IView>> createAssetBrowser(
	ObjectHandleT<IAssetBrowserModel> dataModel,
	std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr) = 0;

	virtual wg_future<std::unique_ptr<IView>> createAssetBrowser20(
	ObjectHandleT<AssetBrowser20::IAssetBrowserModel> dataModel,
	std::unique_ptr<AssetBrowser20::IAssetBrowserEventModel> eventModel = nullptr) = 0;
};
} // end namespace wgt
#endif // I_ASSET_BROWSER_MANAGER_HPP
