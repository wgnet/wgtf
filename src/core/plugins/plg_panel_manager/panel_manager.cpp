#include "panel_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_data_model/asset_browser/i_asset_browser_model.hpp"
#include "core_data_model/asset_browser/asset_browser_view_model.hpp"
#include "core_data_model/asset_browser/asset_browser_event_model.hpp"

#include "core_data_model/asset_browser/file_system_asset_browser_model.hpp"

#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QObject>

namespace wgt
{
PanelManager::PanelManager( IComponentContext & contextManager )
	: contextManager_( contextManager )
	, Depends( contextManager )
{
}

PanelManager::~PanelManager()
{	
	for ( auto type : types_ )
	{
		contextManager_.deregisterInterface(type);
	}
}

wg_future<std::unique_ptr< IView >> PanelManager::createAssetBrowser(
	ObjectHandleT<IAssetBrowserModel> dataModel,
	std::unique_ptr<IAssetBrowserEventModel> eventModel )
{
	if ( !eventModel )
	{
		eventModel.reset(new AssetBrowserEventModel());
	}

	auto uiApplication = contextManager_.queryInterface< IUIApplication >();
	auto uiFramework = contextManager_.queryInterface<IUIFramework>();
	auto definitionManager = contextManager_.queryInterface<IDefinitionManager>();
	assert( uiApplication != nullptr );
	assert(uiFramework != nullptr);
	assert(definitionManager != nullptr);
	
	auto viewDef = definitionManager->getDefinition<IAssetBrowserViewModel>();
	auto dataDef = definitionManager->getDefinition<IAssetBrowserModel>();
	auto eventDef = definitionManager->getDefinition<IAssetBrowserEventModel>();
	if ( viewDef && dataDef && eventDef )
	{
		dataModel->initialise( contextManager_, *definitionManager );
		types_.emplace_back(contextManager_.registerInterface(eventModel.get(), false));
		auto assetBrowserEventModel = ObjectHandleT<IAssetBrowserEventModel>(std::move(eventModel), eventDef);
		auto viewModel = std::unique_ptr<IAssetBrowserViewModel>(new AssetBrowserViewModel(*definitionManager, dataModel, assetBrowserEventModel));
		
		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			return viewCreator->createView("plg_panel_manager/asset_browser_panel.qml", ObjectHandle(std::move(viewModel), viewDef) );
		}
	}
    return std::future<std::unique_ptr<IView>>();
}
} // end namespace wgt
