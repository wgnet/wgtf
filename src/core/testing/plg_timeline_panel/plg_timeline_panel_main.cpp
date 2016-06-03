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
 * TimelinePanelPlugin
 *
 * This is the main application's plugin. It creates the TimelinePanel and
 * registers it with the UIFramework. So that it will be added to the main dialog
 */
class TimelinePanelPlugin
    : public PluginMain
{
public:
    TimelinePanelPlugin( IComponentContext & componentContext )
    {
    }
 
    // Plugin creates resources
    bool PostLoad( IComponentContext & componentContext ) override
    {
        // Static variable that must be set for every plugin
        Variant::setMetaTypeManager(
            componentContext.queryInterface< IMetaTypeManager >() );
 
        // Create the panel
        timelinePanel_.reset( new TimelinePanel( componentContext ) );
        return true;
    }
 
    // Registration of services this plugin provides
    // IComponentContext provides access to services provided by other plugins
    void Initialise( IComponentContext & componentContext ) override
    {
        timelinePanel_->addPanel();
    }
 
    // De-registration of services this plugin provides
    bool Finalise( IComponentContext & componentContext ) override
    {
        timelinePanel_->removePanel();
        return true;
    }
 
    // Plugin deletes resources
    void Unload( IComponentContext & componentContext ) override
    {
        timelinePanel_.reset();
    }
 
private:
    std::unique_ptr< TimelinePanel > timelinePanel_;
};
 
 
PLG_CALLBACK_FUNC( TimelinePanelPlugin )
} // end namespace wgt
