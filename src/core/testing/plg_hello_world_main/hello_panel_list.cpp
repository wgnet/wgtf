#include "hello_panel_list.hpp"
#include "core_logging/logging.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "hello_panel_list_exposed.hpp"

namespace wgt
{
bool HelloPanelList::addPanel()
{
	if (this->get<IDefinitionManager>() == nullptr)
	{
		return false;
	}

	helloPanelListExposed_ = ManagedObject<HelloPanelListExposed>::make();

	auto viewCreator = this->get<IViewCreator>();
	if (viewCreator == nullptr)
	{
		return false;
	}

	helloView_ = viewCreator->createView("WGHello/HelloPanelList.qml", helloPanelListExposed_.getHandleT());

	return helloView_.valid();
}

void HelloPanelList::removePanel()
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

    helloPanelListExposed_ = nullptr;
}

} // end namespace wgt