// Declaration of PluginMain
#include "core_generic_plugin/generic_plugin.hpp"
 
// Declaration of the Panel
#include "timeline_panel.hpp"
 
// Declaration of the type system
#include "core_variant/variant.hpp"
 
 
#include <memory>
 
 
namespace wgt
{
/**
* A plugin which creates a panel with a timeline component filled with sample data
*
* @ingroup plugins
* @image html plg_timeline_panel.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class TimelinePanelPlugin
	: public PluginMain
{
public:
	TimelinePanelPlugin( IComponentContext & componentContext )
	{
	}
 
	bool PostLoad( IComponentContext & componentContext ) override
	{
		return true;
	}
 
	void Initialise( IComponentContext & componentContext ) override
	{
		// Needs to be after plg_reflection Initialise to construct
		// Variant( ObjectHandle )
		// Create the panel
		timelinePanel_.reset( new TimelinePanel( componentContext ) );
		timelinePanel_->addPanel();
	}
 
	bool Finalise( IComponentContext & componentContext ) override
	{
		timelinePanel_->removePanel();
		timelinePanel_.reset();
		return true;
	}
 
	void Unload( IComponentContext & componentContext ) override
	{
	}
 
private:
	std::unique_ptr< TimelinePanel > timelinePanel_;
};
 
 
PLG_CALLBACK_FUNC( TimelinePanelPlugin )
} // end namespace wgt
