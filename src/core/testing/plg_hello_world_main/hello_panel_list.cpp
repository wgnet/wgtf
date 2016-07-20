#include "hello_panel_list.hpp"
#include "core_logging/logging.hpp"
#include "metadata/hello_panel_list.mpp"

namespace wgt
{

HelloPanelList::HelloPanelList( IComponentContext & context )
    : Depends( context )
{
}
 
bool HelloPanelList::addPanel()
{
    if (this->get< IDefinitionManager >() == nullptr)
    {
        return false;
    }

    IDefinitionManager& definitionManager = *this->get< IDefinitionManager >();
    REGISTER_DEFINITION( HelloPanelListExposed );
    auto helloPanelListExposed = definitionManager.create< HelloPanelListExposed >();

    auto viewCreator = this->get< IViewCreator >();
    if (viewCreator == nullptr)
    {
        return false;
    }

    helloView_ = viewCreator->createView(
        "WGHello/HelloPanelList.qml",
        helloPanelListExposed );

    return helloView_.valid();
}

void HelloPanelList::removePanel()
{
    auto uiApplication = this->get< IUIApplication >();
    if (uiApplication == nullptr)
    {
        return;
    }

    if (helloView_.valid())
    {
        auto view = helloView_.get();
        uiApplication->removeView( *view );
        view = nullptr;
    }
}

} // end namespace wgt