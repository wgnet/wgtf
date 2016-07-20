
#include "core_generic_plugin/generic_plugin.hpp"
#include "hello_interface_impl.hpp"
#include <memory>

namespace wgt
{

/**
* This plugin creates an interface for a panel which can be imported into a project via the
* ComponentContext. It will be responsible for creating a new instance of the
* HelloWorldInterface and registering it so that the plugin system can invoke
* functionality at startup and shutdown.
*
* @ingroup plugins
* @image html plg_hello_world_interface.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class HelloInterfacePlugin
    : public PluginMain
{
public:

    HelloInterfacePlugin( IComponentContext & componentContext )
    {
    }

    bool PostLoad( IComponentContext & componentContext ) override
    {
        // Create the panel
        types_.push_back( componentContext.registerInterface( new HelloInterfaceImpl( componentContext ) ) );
        return true;
    }

    void Initialise( IComponentContext & componentContext ) override
    {
    }

    bool Finalise( IComponentContext & componentContext ) override
    {
        return true;
    }

    void Unload( IComponentContext & componentContext ) override
    {
        for ( auto type: types_ )
        {
            componentContext.deregisterInterface( type );
        }
    }

private:

    std::vector< IInterface * > types_;
};
 
PLG_CALLBACK_FUNC( HelloInterfacePlugin )

}