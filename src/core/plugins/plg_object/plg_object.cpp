#include "core_generic_plugin/generic_plugin.hpp"
#include "object_manager.hpp"
#include "object_manager_creator.hpp"

namespace wgt
{
/**
* A plugin which implements a context object manager. The interface is empty and currently unused
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins

*/
class ObjectPlugin : public PluginMain
{
public:
	ObjectPlugin( IComponentContext & context )
	{

	}

	bool PostLoad( IComponentContext & context ) override
	{
		context.registerInterface(new ObjectManagerCreator(context));
		return true; 
	}
	
	void Initialise( IComponentContext & context ) override
	{

	}

	bool Finalise( IComponentContext & context ) override
	{
		return true; 
	}

	void Unload( IComponentContext & context ) override
	{
	}
};

PLG_CALLBACK_FUNC( ObjectPlugin )
} // end namespace wgt
