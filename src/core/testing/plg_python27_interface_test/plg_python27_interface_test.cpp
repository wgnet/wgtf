#include "pch.hpp"
#include "main_application.hpp"

#include "core_generic_plugin/generic_plugin.hpp"


namespace wgt
{
/**
* A plugin which queries the IPythonScriptingEngine and runs unit tests on its interface
*
* @ingroup plugins 
* @note Requires Plugins:
*       - @ref coreplugins
*       - Python27Plugin
*/
class Python27TestPlugin
	: public PluginMain
{
public:
	Python27TestPlugin( IComponentContext & contextManager )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		contextManager.registerInterface( new MainApplication( contextManager ) );
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
	}
};

PLG_CALLBACK_FUNC( Python27TestPlugin )
} // end namespace wgt
