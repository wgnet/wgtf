#ifndef ASSET_BROWSER_VIEW_HPP
#define ASSET_BROWSER_VIEW_HPP

#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "interfaces/panel_manager/i_panel_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class PanelManager : public Implements<IPanelManager>, public Depends<IViewCreator>
{
public:
	PanelManager(IComponentContext& contextManager);
	virtual ~PanelManager();

	wg_future<std::unique_ptr<IView>> createAssetBrowser(
	ObjectHandleT<AssetBrowser20::IAssetBrowserModel> assetModel) override;

private:
	IComponentContext& contextManager_;
	std::vector<IInterface*> types_;
};
} // end namespace wgt
#endif // ASSET_BROWSER_VIEW_HPP
