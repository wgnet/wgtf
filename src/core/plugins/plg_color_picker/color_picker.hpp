#pragma once
#ifndef _COLOR_PICKER_HPP
#define _COLOR_PICKER_HPP
 
#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_handle.hpp"

#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include <memory>

namespace wgt
{
class ColorPicker
	: Depends< IUIFramework, IUIApplication, IDefinitionManager, IViewCreator >

{
	typedef Depends< IUIFramework, IUIApplication, IDefinitionManager > DepsBase;

public:
	ColorPicker( IComponentContext & context );

	bool addPanel();
	void removePanel();

private:
	std::unique_ptr< IView > colorView_;
	ObjectHandle colorPickerContext_;
};
 
 
} // end namespace wgt
#endif // _COLOR_PICKER_HPP
