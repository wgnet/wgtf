#pragma once
#ifndef _CUSTOM_UI_THEMES_HPP
#define _CUSTOM_UI_THEMES_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_action.hpp"

#include <memory>

namespace wgt
{
class CustomUIThemesDialog : Depends<IUIFramework, IUIApplication, IViewCreator>
{
public:
	bool createDialog();
	void destroyDialog();

private:
	std::vector<std::unique_ptr<IAction>> actions_;
	void actionShowDialog(const IAction* action);
	void setTheme(const IAction* action, Palette::Theme theme);
	bool getIsCurrentTheme(const IAction* action, Palette::Theme theme) const;
};

#endif // _CUSTOM_UI_THEMES_HPP
}
