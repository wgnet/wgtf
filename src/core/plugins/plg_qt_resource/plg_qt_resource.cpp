#include "core_generic_plugin/generic_plugin.hpp"

#include "qt_resource_system.hpp"

#include <vector>

namespace wgt
{

/**
* Registers the Qt IResourceSystem for packing resources into binaries using QRC.
*
* @ingroup plugins
* @ingroup coreplugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class QtPluginResource
	: public PluginMain
{
public:
	QtPluginResource( IComponentContext & contextManager )
	{
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		types_.push_back(
			contextManager.registerInterface( new QtResourceSystem() ));

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

private:
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( QtPluginResource )
} // end namespace wgt
