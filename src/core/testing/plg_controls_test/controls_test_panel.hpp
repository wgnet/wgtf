#pragma once
#ifndef _CONTROLS_TEST_PANEL_HPP
#define _CONTROLS_TEST_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>

namespace wgt
{
class ControlsTestPanel
	: Depends< IViewCreator, IUIApplication, IDefinitionManager >
{
public:
	ControlsTestPanel( IComponentContext & context );

	bool addPanel();
	void removePanel();

private:
	wg_future<std::unique_ptr< IView >> controlsView_;
	ObjectHandle controlData_;
};
} // end namespace wgt
#endif // _CONTROLS_TEST_PANEL_HPP
