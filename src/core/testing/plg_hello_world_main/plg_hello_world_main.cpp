
// Declaration of PluginMain
#include "core_generic_plugin/generic_plugin.hpp"
 
// Declaration of the Panel
#include "hello_panel.hpp"
#include "hello_panel_list.hpp"

// Interface specifically for the Hello World Interface example
#include "../interfaces_test/hello_interface.hpp"
 
#include <memory>
 
namespace wgt
{
 
/**
* This plugin creates panels used for the Hello World tutorial and
* registers it with the UIFramework to be added to the main application.
*
* @ingroup plugins
* @image html plg_hello_world_main.png 
* @note Requires Plugins:
*       - @ref coreplugins
*       - HelloInterfacePlugin
*/
class HelloPanelPlugin
    : public PluginMain
{
public:
    HelloPanelPlugin( IComponentContext & componentContext )
    {
    }
 
    bool PostLoad( IComponentContext & componentContext ) override
    {
        // Create the panel
        helloPanel_.reset( new HelloPanel( componentContext ) );
        helloPanelList_.reset( new HelloPanelList( componentContext ) );
        return true;
    }
 
    void Initialise( IComponentContext & componentContext ) override
    {
        helloPanel_->addPanel();
        helloPanelList_->addPanel();

        // Load the hello world interface plugin by querying the HelloInterface with version 1
        auto plugin = componentContext.queryInterface< INTERFACE_VERSION( HelloInterface, 0, 1 ) >();
        plugin->display();
    }
 
    bool Finalise( IComponentContext & componentContext ) override
    {
        helloPanel_->removePanel();
        helloPanelList_->removePanel();

        // Remove the hello world interface plugin by querying the HelloInterface with version 1
        auto plugin = componentContext.queryInterface< INTERFACE_VERSION( HelloInterface, 0, 1 ) >();
        plugin->remove();

        return true;
    }
 
    void Unload( IComponentContext & componentContext ) override
    {
        helloPanel_.reset();
        helloPanelList_.reset();
    }
 
private:
    std::unique_ptr< HelloPanel > helloPanel_;
    std::unique_ptr< HelloPanelList > helloPanelList_;
};
 
PLG_CALLBACK_FUNC( HelloPanelPlugin )

} // end namespace wgt