#include "panel_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_data_model/asset_browser/i_asset_browser_model20.hpp"

namespace wgt
{
PanelManager::PanelManager(IComponentContext& contextManager) : contextManager_(contextManager)
{
}

PanelManager::~PanelManager()
{
	for (auto type : types_)
	{
		contextManager_.deregisterInterface(type.get());
	}
}

wg_future<std::unique_ptr<IView>> PanelManager::createAssetBrowser(
	const std::string& title,
	ObjectHandleT<AssetBrowser20::IAssetBrowserModel> dataModel)
{
	auto viewCreator = get<IViewCreator>();
	if (viewCreator)
	{
		return viewCreator->createView("plg_panel_manager/asset_browser_panel20.qml", dataModel, [title](IView& view) 
		{
			view.title(title.c_str());
		});
	}
	return std::future<std::unique_ptr<IView>>();
}
} // end namespace wgt
