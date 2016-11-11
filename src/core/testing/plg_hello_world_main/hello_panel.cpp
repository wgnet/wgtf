#include "hello_panel.hpp"
#include "metadata/hello_panel.mpp"
#include "core_logging/logging.hpp"

namespace wgt
{
HelloPanel::HelloPanel(IComponentContext& context) : Depends(context)
{
}

bool HelloPanel::addPanel()
{
	if (this->get<IDefinitionManager>() == nullptr)
	{
		return false;
	}

	IDefinitionManager& definitionManager = *this->get<IDefinitionManager>();
	REGISTER_DEFINITION(HelloPanelExposed);
	REGISTER_DEFINITION(HelloPanelExposedSource);
	auto helloPanelExposed = definitionManager.create<HelloPanelExposedSource>();

	auto viewCreator = this->get<IViewCreator>();
	if (viewCreator == nullptr)
	{
		return false;
	}

	helloView_ = viewCreator->createView("WGHello/HelloPanel.qml", helloPanelExposed);

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
}

} // end namespace wgt