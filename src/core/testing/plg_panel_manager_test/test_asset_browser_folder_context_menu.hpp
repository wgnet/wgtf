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
#include "core_dependency_system/depends.hpp"

class QString;
class QStringList;

namespace wgt
{
class IQtFramework;
class IFileSystem;

class TestAssetBrowserFolderContextMenu20 : public IMenuHandler
	, Depends< IQtFramework, IFileSystem >
{
public:
	TestAssetBrowserFolderContextMenu20();

private:
	virtual void registerActions(IUIFramework& uiFramework, Actions& actions) override;

	bool canCopyPath(const IAction* action) const;
	void copyPath(IAction* action);
	bool canShow(const IAction* action) const;
	void show(IAction* action);
	void open(IAction* action);
};
}

#endif // TEST_ASSET_BROWSER_FOLDER_CONTEXT_MENU_H_