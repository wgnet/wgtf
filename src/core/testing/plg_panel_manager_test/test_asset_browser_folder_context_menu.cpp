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
#include "core_qt_common/i_qt_framework.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_serialization/i_file_system.hpp"

namespace wgt
{
namespace
{
std::string getAssetFromAction(const IAction* action)
{
	if (action == nullptr)
	{
		return "";
	}

	auto path = action->getData().value<std::string>();
	return path;
}
}

TestAssetBrowserFolderContextMenu20::TestAssetBrowserFolderContextMenu20()
{
}

void TestAssetBrowserFolderContextMenu20::registerActions(IUIFramework& uiFramework, Actions& actions)
{
	// Load the action data
	uiFramework.loadActionData(":/plg_panel_manager_test/test_asset_browser_folder_actions.xml",
	                           IUIFramework::ResourceType::File);

	// Create actions and add them to the UI Framework
	using namespace std::placeholders;

	actions.emplace_back(
	uiFramework.createAction("CopyFolderPath", std::bind(&TestAssetBrowserFolderContextMenu20::copyPath, this, _1),
	                         std::bind(&TestAssetBrowserFolderContextMenu20::canCopyPath, this, _1)));
	actions.emplace_back(
	uiFramework.createAction("CopyAssetPath", std::bind(&TestAssetBrowserFolderContextMenu20::copyPath, this, _1),
	                         std::bind(&TestAssetBrowserFolderContextMenu20::canCopyPath, this, _1)));
	actions.emplace_back(uiFramework.createAction("showInGraphicShell",
	                                              std::bind(&TestAssetBrowserFolderContextMenu20::show, this, _1),
	                                              std::bind(&TestAssetBrowserFolderContextMenu20::canShow, this, _1)));
	actions.emplace_back(
	uiFramework.createAction("openInDefaultApp", std::bind(&TestAssetBrowserFolderContextMenu20::open, this, _1)));
}

bool TestAssetBrowserFolderContextMenu20::canCopyPath(const IAction* action) const
{
	return !getAssetFromAction(action).empty();
}

void TestAssetBrowserFolderContextMenu20::copyPath(IAction* action)
{
	auto selectedAssetData = getAssetFromAction(action);
	if (!selectedAssetData.empty())
	{
		get< IQtFramework >()->copyTextToClipboard(selectedAssetData.c_str());
	}
}

bool TestAssetBrowserFolderContextMenu20::canShow(const IAction* action) const
{
	auto selectedAssetData = getAssetFromAction(action);
	return selectedAssetData != "" && get<IFileSystem >()->exists(selectedAssetData.c_str());
}

void TestAssetBrowserFolderContextMenu20::show(IAction* action)
{
	auto pathIn = getAssetFromAction(action);
	get< IQtFramework >()->openInGraphicalShell(pathIn.c_str());
}

void TestAssetBrowserFolderContextMenu20::open(IAction* action)
{
	auto pathIn = getAssetFromAction(action);
	get< IQtFramework >()->openInDefaultApp(pathIn.c_str());
}
}