#include "timeline_panel.hpp"
#include "core_reflection/object_handle.hpp"
 
 
namespace wgt
{
TimelinePanel::TimelinePanel( IComponentContext & context )
    : Depends( context )
{
	timelineModel_.addComponent( "Component 1" );
	timelineModel_.addTextBox( "Condition 1", "if (life == 0)");
	auto health = timelineModel_.addFrameSlider( "Health", "#22EE22", "health" );
	timelineModel_.addKeyFrame( 0, "0", "constant", health );
	auto baseModel = timelineModel_.addFrameSlider( "Model Property", "#3333BB", "baseModel" );
	timelineModel_.addKeyFrame( 0, "hide", "constant", baseModel );
	auto wreckModel = timelineModel_.addFrameSlider( "Model Property", "#3333BB", "wreckModel" );
	timelineModel_.addKeyFrame( 0.1, "show", "constant", wreckModel );
	auto particleSpawn = timelineModel_.addFrameSlider( "Particles", "#BB33BB", "particleSpawn" );
	timelineModel_.addKeyFrame( 0.1, "true", "constant", particleSpawn );
	timelineModel_.addBarSlider( "Sound", 0.1, 2.2, "#3FA9F5", "file", "explosion.fsb" );
	auto volume = timelineModel_.addFrameSlider( "Volume", "#3FA9F5", "volume" );
	timelineModel_.addKeyFrame( 0.1, "0", "linear", volume );
	timelineModel_.addKeyFrame( 0.9, "100", "linear", volume );
	timelineModel_.addKeyFrame( 1.6, "100", "linear", volume );
	timelineModel_.addKeyFrame( 2.2, "0", "linear", volume );
	timelineModel_.addTextBox( "Condition 2", "if (componentLife == 0 && onFire)");
	health = timelineModel_.addFrameSlider( "Health", "#22EE22", "health" );
	timelineModel_.addKeyFrame( 0, "0", "constant", health );
	baseModel = timelineModel_.addFrameSlider( "Model Property", "#3333BB", "baseModel" );
	timelineModel_.addKeyFrame( 0, "hide", "constant", baseModel );
	auto objectWreckModel = timelineModel_.addFrameSlider( "Model Property", "#3333BB", "objectWreckModel" );
	timelineModel_.addKeyFrame( 0.1, "show", "constant", objectWreckModel );
	auto componentModel = timelineModel_.addFrameSlider( "Model Property", "#3333BB", "componentModel" );
	timelineModel_.addKeyFrame( 0.1, "show", "constant", componentModel );
	auto impulse = timelineModel_.addFrameSlider( "Physics", "#BB3333", "impulse" );
	timelineModel_.addKeyFrame( 0.1, "(0,0,200)", "constant", impulse );
	particleSpawn = timelineModel_.addFrameSlider( "Particles", "#BB33BB", "particleSpawn" );
	timelineModel_.addKeyFrame( 0.1, "true", "constant", particleSpawn );
	timelineModel_.addBarSlider( "Sound", 0.1, 3, "#3FA9F5", "file", "cookoff.fsb" );
	volume = timelineModel_.addFrameSlider( "Volume", "#3FA9F5", "volume" );
	timelineModel_.addKeyFrame( 0.1, "100", "boolean", volume );
	timelineModel_.addKeyFrame( 3.0, "0", "boolean", volume );

	timelineModel_.addComponent( "Component 1" );
	timelineModel_.addTextBox( "Condition 1", "if (componentLife == 0 && speed >= 10)");
	auto speed = timelineModel_.addFrameSlider( "Physics", "#BB3333", "speed" );
	timelineModel_.addKeyFrame( 0, "0", "constant", speed );
	componentModel = timelineModel_.addFrameSlider( "Model Property", "#3333BB", "componentModel" );
	timelineModel_.addKeyFrame( 0, "hide", "constant", componentModel );
	auto wreckedComponentModel = timelineModel_.addFrameSlider( "Model Property", "#3333BB", "wreckedComponentModel" );
	timelineModel_.addKeyFrame( 0.1, "show", "constant", wreckedComponentModel );
	timelineModel_.addBarSlider( "Sound", 0.1, 1.5, "#3FA9F5", "file", "componentBreak.fsb" );
	volume = timelineModel_.addFrameSlider( "Volume", "#3FA9F5", "volume" );
	timelineModel_.addKeyFrame( 0.1, "0", "linear", volume );
	timelineModel_.addKeyFrame( 0.5, "100", "linear", volume );
	timelineModel_.addKeyFrame( 1.5, "0", "linear", volume );
}
 
 
bool TimelinePanel::addPanel()
{
	auto viewCreator = this->get< IViewCreator >();
	if (viewCreator == nullptr)
	{
		return false;
	}
	timelineView_ = viewCreator->createView(
        "PlgTimelinePanel/TimelinePanel.qml", ObjectHandleT< AbstractListModel >( &timelineModel_ ));
    return true;
}
 
 
void TimelinePanel::removePanel()
{
    auto uiApplication = this->get< IUIApplication >();
    if (uiApplication == nullptr)
    {
        return;
    }
    if(timelineView_.valid())
    {
        auto view = timelineView_.get();
        uiApplication->removeView(*view);
        view = nullptr;
    }
}
} // end namespace wgt
