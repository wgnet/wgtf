#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

#include "core_common/wg_future.hpp"

namespace wgt
{
class IAssetBrowserModel;
class IAssetBrowserEventModel;
class IComponentContext;
class IView;

class IPanelManager
{
public:

	IPanelManager() {}
	virtual ~IPanelManager() {}

	virtual wg_future<std::unique_ptr< IView >> createAssetBrowser(
		ObjectHandleT<IAssetBrowserModel> dataModel,
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr ) = 0;
};
} // end namespace wgt
#endif // I_ASSET_BROWSER_MANAGER_HPP
