//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  test_asset_browser_folder_context_menu.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "test_asset_browser_folder_context_menu.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/asset_browser/i_asset_object_item.hpp"
#include "QtWidgets/QApplication"
#include "QtGui/QClipboard"

namespace wgt
{
	namespace
	{
		IAssetObjectItem* getAssetFromAction(const IAction* action)
		{
			if ( action == nullptr )
			{
				return nullptr;
			}

			auto assetItem = reinterpret_cast<IItem*>( action->getData().value<intptr_t>() );
			return dynamic_cast<IAssetObjectItem*>( assetItem );
		}
	}

	TestAssetBrowserFolderContextMenu::TestAssetBrowserFolderContextMenu(IComponentContext & contextManager)
		: IMenuHandler(contextManager)
	{
	}

	void TestAssetBrowserFolderContextMenu::registerActions(IUIFramework& uiFramework, Actions& actions)
	{
		// Load the action data
		uiFramework.loadActionData(":/plg_panel_manager_test/test_asset_browser_folder_actions.xml", IUIFramework::ResourceType::File);

		// Create actions and add them to the UI Framework
		using namespace std::placeholders;

		actions.emplace_back(uiFramework.createAction("CopyFolderPath",
			std::bind(&TestAssetBrowserFolderContextMenu::copyPath, this, _1),
			std::bind(&TestAssetBrowserFolderContextMenu::canCopyPath, this, _1)));
	}

	bool TestAssetBrowserFolderContextMenu::canCopyPath(const IAction* action) const
	{
		return getAssetFromAction(action) != nullptr;
	}

	void TestAssetBrowserFolderContextMenu::copyPath(IAction* action)
	{
		auto selectedAssetData = getAssetFromAction(action);
		if ( selectedAssetData )
		{
			QClipboard *clipboard = QApplication::clipboard();
			if(clipboard)
			{
				clipboard->setText(selectedAssetData->getFullPath());
			}
		}
	}
}