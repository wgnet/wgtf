#include "color_picker.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"
#include "color_picker_context.hpp"
#include "core_reflection/property_accessor.hpp"

namespace wgt
{
ColorPicker::ColorPicker( IComponentContext & context )
	: Depends( context )
{
	auto definitionManager = this->get< IDefinitionManager >();
	colorPickerContext_ = definitionManager->create< ColorPickerContext >();
}


bool ColorPicker::addPanel()
{
	auto viewCreator = get< IViewCreator >();
	if (viewCreator)
	{
		viewCreator->createView(
			"WGColorPicker/WGColorPickerPanel.qml", colorPickerContext_, colorView_ );
	}
	return true;
}
 
 
void ColorPicker::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (colorView_ != nullptr)
	{
		uiApplication->removeView( *colorView_ );
	}
}
} // end namespace wgt
