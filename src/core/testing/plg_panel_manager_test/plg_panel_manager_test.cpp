#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include "core_serialization/i_file_system.hpp"
#include "core_data_model/asset_browser/asset_browser_model20.hpp"
#include "test_asset_browser_folder_context_menu.hpp"
#include "core_data_model/file_system/file_system_model.hpp"

// TODO: merge asset and panel manager
#include "interfaces/asset_manager/i_asset_manager.hpp"
#include "interfaces/panel_manager/i_panel_manager.hpp"

namespace wgt
{
class AssetManager : public Implements<IAssetManager>
{
public:
	AssetManager()
	{
	}
	virtual ~AssetManager()
	{
	}

	void initialise(IFileSystem& fileSystem)
	{
		assetModel_.reset(new FileSystemModel(fileSystem, "../../"));
	}

	virtual AbstractItemModel* assetModel() const override
	{
		return assetModel_.get();
	}

private:
	std::unique_ptr<FileSystemModel> assetModel_;
};
/**
* A plugin which queries the IPanelManager to create an asset browser
*
* @ingroup plugins
* @image html plg_panel_manager_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - PanelManagerPlugin
*/
class TestPanelManagerPlugin : public PluginMain
{
public:
	//==========================================================================
	TestPanelManagerPlugin(IComponentContext& contextManager) : assetManager_(nullptr)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		assetManager_ = new AssetManager();
		types_.push_back(contextManager.registerInterface(assetManager_));
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		auto fileSystem = contextManager.queryInterface<IFileSystem>();
		auto panelManager = contextManager.queryInterface<IPanelManager>();
		assert(uiApplication != nullptr);
		assert(fileSystem != nullptr);
		assert(definitionManager != nullptr);
		assert(panelManager != nullptr);
		if (!fileSystem || !definitionManager || !uiApplication || !panelManager)
			return;

		assetManager_->initialise(*fileSystem);

		std::vector<std::string> assetPaths;
		std::vector<std::string> customFilters;
		assetPaths.emplace_back("../../");
		{
			std::vector<std::string> nameFilters;
			nameFilters.push_back("All Files (*)");
			nameFilters.push_back("DLL Files (*.dll)");
			nameFilters.push_back("PDB Files (*.pdb)");

			auto browserModel = new AssetBrowser20::AssetBrowserModel();
			browserModel->setAssetModel(assetManager_->assetModel());
			browserModel->setNameFilters(nameFilters);
			dataModel_ = ObjectHandleT<AssetBrowser20::IAssetBrowserModel>(
			std::unique_ptr<AssetBrowser20::IAssetBrowserModel>(browserModel));
			assetBrowserView_ = panelManager->createAssetBrowser(dataModel_);
			assetBrowserFolderContextMenuHandler_ =
			CreateMenuHandler<TestAssetBrowserFolderContextMenu20>(contextManager);
		}
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		assetBrowserFolderContextMenuHandler_.reset();
		auto uiApplication = contextManager.queryInterface<IUIApplication>();

		if (uiApplication)
		{
			if (assetBrowserView_.valid())
			{
				auto view = assetBrowserView_.get();
				uiApplication->removeView(*view);
				view = nullptr;
			}
		}

		if (dataModel_ != nullptr)
		{
			dataModel_ = nullptr;
		}

		return true;
	}

private:
	std::vector<IInterface*> types_;
	AssetManager* assetManager_;
	ObjectHandleT<AssetBrowser20::IAssetBrowserModel> dataModel_;
	wg_future<std::unique_ptr<IView>> assetBrowserView_;
	MenuHandlerPtr assetBrowserFolderContextMenuHandler_;
};

PLG_CALLBACK_FUNC(TestPanelManagerPlugin)
} // end namespace wgt
