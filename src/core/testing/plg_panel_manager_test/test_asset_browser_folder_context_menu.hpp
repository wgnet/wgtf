//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  test_asset_browser_folder_context_menu.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef TEST_ASSET_BROWSER_FOLDER_CONTEXT_MENU_H_
#define TEST_ASSET_BROWSER_FOLDER_CONTEXT_MENU_H_

#pragma once

#include "core_ui_framework/i_menu_handler.hpp"

namespace wgt
{
class TestAssetBrowserFolderContextMenu : public IMenuHandler
{
public:
	TestAssetBrowserFolderContextMenu(IComponentContext& contextManager);

private:
	virtual void registerActions(IUIFramework& uiFramework, Actions& actions) override;

	bool canCopyPath(const IAction* action) const;
	void copyPath(IAction* action);
};

class TestAssetBrowserFolderContextMenu20 : public IMenuHandler
{
public:
	TestAssetBrowserFolderContextMenu20(IComponentContext& contextManager);

private:
	virtual void registerActions(IUIFramework& uiFramework, Actions& actions) override;

	bool canCopyPath(const IAction* action) const;
	void copyPath(IAction* action);
};
}

#endif // TEST_ASSET_BROWSER_FOLDER_CONTEXT_MENU_H_