#include "timeline_panel.hpp"
#include "core_reflection/object_handle.hpp"
 
 
namespace wgt
{
TimelinePanel::TimelinePanel( IComponentContext & context )
    : Depends( context )
{
}
 
 
bool TimelinePanel::addPanel()
{
	auto viewCreator = this->get< IViewCreator >();
	if (viewCreator == nullptr)
	{
		return false;
	}
	ObjectHandle handle;
    viewCreator->createView(
        "PlgTimelinePanel/TimelinePanel.qml", handle, timelineView_ );
    return true;
}
 
 
void TimelinePanel::removePanel()
{
    auto uiApplication = this->get< IUIApplication >();
    if (uiApplication == nullptr)
    {
        return;
    }
    uiApplication->removeView( *timelineView_ );
}
} // end namespace wgt
