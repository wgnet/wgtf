#include "test_asset_presentation_provider.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include "core_serialization/i_file_system.hpp"
#include "core_data_model/asset_browser/file_system_asset_browser_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_event_model.hpp"
#include "core_data_model/asset_browser/file_system_asset_browser_model20.hpp"
#include "core_data_model/asset_browser/i_asset_browser_event_model20.hpp"
#include "interfaces/panel_manager/i_panel_manager.hpp"
#include "test_asset_browser_folder_context_menu.hpp"

namespace wgt
{
/**
* A plugin which queries the IPanelManager to create an asset browser
*
* @ingroup plugins
* @image html plg_panel_manager_test.png 
* @note Requires Plugins:
*       - @ref coreplugins
*       - PanelManagerPlugin
*/
class TestPanelManagerPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TestPanelManagerPlugin(IComponentContext & contextManager) {}

	void Initialise(IComponentContext& contextManager) override
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		auto fileSystem = contextManager.queryInterface<IFileSystem>();
		auto panelManager = contextManager.queryInterface<IPanelManager>();
		assert(uiApplication != nullptr);
		assert(fileSystem != nullptr);
		assert(definitionManager != nullptr);
		assert(panelManager != nullptr);
		if(!fileSystem || !definitionManager || !uiApplication || !panelManager)
			return;

		std::vector<std::string> assetPaths;
		std::vector<std::string> customFilters;
		assetPaths.emplace_back("../../");
		{
			presentationProviderOld_.generateData();
			auto browserModelOld = std::unique_ptr<IAssetBrowserModel>(
			new FileSystemAssetBrowserModel(assetPaths, customFilters, *fileSystem,
			                                *definitionManager, presentationProviderOld_));

			auto dataDef = definitionManager->getDefinition<IAssetBrowserModel>();
			dataModelOld_ = ObjectHandleT<IAssetBrowserModel>(std::move(browserModelOld), dataDef);
			assetBrowserViewOld_ = panelManager->createAssetBrowser(dataModelOld_);
			assetBrowserFolderContextMenuHandlerOld_ = CreateMenuHandler<TestAssetBrowserFolderContextMenu>(contextManager);
		}

		{
			presentationProvider_.generateData();
			auto browserModel = std::unique_ptr<AssetBrowser20::IAssetBrowserModel>(
			new AssetBrowser20::FileSystemAssetBrowserModel(assetPaths, customFilters, *fileSystem,
			                                                *definitionManager, presentationProvider_));

			auto dataDef = definitionManager->getDefinition<AssetBrowser20::IAssetBrowserModel>();
			dataModel_ = ObjectHandleT<AssetBrowser20::IAssetBrowserModel>(std::move(browserModel), dataDef);
			assetBrowserView_ = panelManager->createAssetBrowser20(dataModel_);
			assetBrowserFolderContextMenuHandler_ = CreateMenuHandler<TestAssetBrowserFolderContextMenu20>(contextManager);
		}
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		assetBrowserFolderContextMenuHandlerOld_.reset();
		assetBrowserFolderContextMenuHandler_.reset();
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication)
		{
			if (assetBrowserViewOld_.valid())
			{
				auto view = assetBrowserViewOld_.get();
				uiApplication->removeView( *view );
				view = nullptr;
			}
			if (assetBrowserView_.valid())
			{
				auto view = assetBrowserView_.get();
				uiApplication->removeView(*view);
				view = nullptr;
			}
		}
		if (dataModelOld_ != nullptr)
		{
			dataModelOld_->finalise();
			dataModelOld_ = nullptr;
		}
		if (dataModel_ != nullptr)
		{
			dataModel_->finalise();
			dataModel_ = nullptr;
		}
		return true;
	}

private:
	TestAssetPresentationProvider presentationProviderOld_;
	ObjectHandleT<IAssetBrowserModel> dataModelOld_;
	wg_future<std::unique_ptr<IView>> assetBrowserViewOld_;
	MenuHandlerPtr assetBrowserFolderContextMenuHandlerOld_;
	TestAssetPresentationProvider20 presentationProvider_;
	ObjectHandleT<AssetBrowser20::IAssetBrowserModel> dataModel_;
	wg_future<std::unique_ptr<IView>> assetBrowserView_;
	MenuHandlerPtr assetBrowserFolderContextMenuHandler_;
};

PLG_CALLBACK_FUNC(TestPanelManagerPlugin)
} // end namespace wgt
