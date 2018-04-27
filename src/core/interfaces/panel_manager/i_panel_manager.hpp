#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

#include "core_common/wg_future.hpp"

namespace wgt
{
class IComponentContext;
class IView;

namespace AssetBrowser20
{
class IAssetBrowserModel;
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
		const std::string& title,
		ObjectHandleT<AssetBrowser20::IAssetBrowserModel> dataModel) = 0;
};
} // end namespace wgt
#endif // I_ASSET_BROWSER_MANAGER_HPP
