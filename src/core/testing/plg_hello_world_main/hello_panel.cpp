#include "hello_panel.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "hello_panel_exposed.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
bool HelloPanel::addPanel()
{
	if (this->get<IDefinitionManager>() == nullptr)
	{
		return false;
	}

	helloPanelExposed_ = ManagedObject<HelloPanelExposedSource>::make();

	auto viewCreator = this->get<IViewCreator>();
	if (viewCreator == nullptr)
	{
		return false;
	}

	helloView_ = viewCreator->createView("WGHello/HelloPanel.qml", helloPanelExposed_.getHandleT());
	return helloView_.valid();
}

void HelloPanel::removePanel()
{
	auto uiApplication = this->get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (helloView_.valid())
	{
		auto view = helloView_.get();
		uiApplication->removeView(*view);
		view = nullptr;
	}

    helloPanelExposed_ = nullptr;
}

} // end namespace wgt