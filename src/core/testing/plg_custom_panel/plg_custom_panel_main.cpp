// Declaration of PluginMain
#include "core_generic_plugin/generic_plugin.hpp"
 
// Declaration of the Panel
#include "custom_panel.hpp"
 
 
#include <memory>
 
 
namespace wgt
{
/**
* A plugin which creates a panel to test various components and subpanels
*
* @ingroup plugins
* @image html plg_custom_panel.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class CustomPanelPlugin
    : public PluginMain
{
public:
    CustomPanelPlugin( IComponentContext & componentContext )
    {
    }
 
    bool PostLoad( IComponentContext & componentContext ) override
    {
        // Create the panel
        customPanel_.reset( new CustomPanel( componentContext ) );
        return true;
    }
 
    void Initialise( IComponentContext & componentContext ) override
    {
        customPanel_->addPanel();
    }
 
    bool Finalise( IComponentContext & componentContext ) override
    {
        customPanel_->removePanel();
        return true;
    }
 
    void Unload( IComponentContext & componentContext ) override
    {
        customPanel_.reset();
    }
 
private:
    std::unique_ptr< CustomPanel > customPanel_;
};
 
 
PLG_CALLBACK_FUNC( CustomPanelPlugin )
} // end namespace wgt
