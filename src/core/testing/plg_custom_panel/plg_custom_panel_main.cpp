// Declaration of PluginMain
#include "core_generic_plugin/generic_plugin.hpp"
 
// Declaration of the Panel
#include "custom_panel.hpp"
 
// Declaration of the type system
#include "core_variant/variant.hpp"
 
 
#include <memory>
 
 
namespace wgt
{
/**
 * CustomPanelMainPlugin
 *
 * This is the main application's plugin. It creates the CustomPanel and
 * registers it with the UIFramework. So that it will be added to the main dialog
 */
class CustomPanelPlugin
    : public PluginMain
{
public:
    CustomPanelPlugin( IComponentContext & componentContext )
    {
    }
 
    // Plugin creates resources
    bool PostLoad( IComponentContext & componentContext ) override
    {
        // Static variable that must be set for every plugin
        Variant::setMetaTypeManager(
            componentContext.queryInterface< IMetaTypeManager >() );
 
        // Create the panel
        customPanel_.reset( new CustomPanel( componentContext ) );
        return true;
    }
 
    // Registration of services this plugin provides
    // IComponentContext provides access to services provided by other plugins
    void Initialise( IComponentContext & componentContext ) override
    {
        customPanel_->addPanel();
    }
 
    // De-registration of services this plugin provides
    bool Finalise( IComponentContext & componentContext ) override
    {
        customPanel_->removePanel();
        return true;
    }
 
    // Plugin deletes resources
    void Unload( IComponentContext & componentContext ) override
    {
        customPanel_.reset();
    }
 
private:
    std::unique_ptr< CustomPanel > customPanel_;
};
 
 
PLG_CALLBACK_FUNC( CustomPanelPlugin )
} // end namespace wgt
