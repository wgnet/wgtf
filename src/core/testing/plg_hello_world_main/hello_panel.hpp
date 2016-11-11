#pragma once
#ifndef _HELLO_PANEL_HPP
#define _HELLO_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include <memory>

namespace wgt
{
class HelloPanel : Depends<IUIFramework, IUIApplication, IDefinitionManager, IViewCreator>
{
public:
	HelloPanel(IComponentContext& context);

	bool addPanel();
	void removePanel();

private:
	wg_future<std::unique_ptr<IView>> helloView_;
};
} // end namespace wgt

#endif // _HELLO_PANEL_HPP