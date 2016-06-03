#include "core_generic_plugin/generic_plugin.hpp"
#include "object_manager.hpp"
#include "object_manager_creator.hpp"

namespace wgt
{
class ObjectPlugin : public PluginMain
{
public:
	ObjectPlugin( IComponentContext & context )
	{

	}

	bool PostLoad( IComponentContext & context ) override
	{
		context.registerInterface( new ObjectManagerCreator( context ) );
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
