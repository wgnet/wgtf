#pragma once
#ifndef _QML_PREFERENCES_PANEL_HPP
#define _QML_PREFERENCES_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_dependency_system/depends.hpp"

#include <memory>

namespace wgt
{
class QmlPreferencesPanel : Depends<IUIFramework, IUIApplication, IViewCreator>
{
public:
	bool addPanel();
	void removePanel();

private:
	wg_future<std::unique_ptr<IView>> qmlPreferencesView_;
};

#endif // _QML_PREFERENCES_PANEL_HPP
}
