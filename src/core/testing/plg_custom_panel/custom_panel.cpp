#include "custom_panel.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
CustomPanel::CustomPanel( IComponentContext & context )
	: Depends( context )
{
}
 
 
bool CustomPanel::addPanel()
{
	auto viewCreator = this->get< IViewCreator >();
	
	if (viewCreator == nullptr)
	{
		return false;
	}

	viewCreator->createView(
		"plg_custom_panel/custom_panel.qml",
		ObjectHandle(), customView_ );
	return true;
}
 
 
void CustomPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (customView_ != nullptr)
	{
		uiApplication->removeView( *customView_ );
		customView_ = nullptr;
	}
}
} // end namespace wgt
