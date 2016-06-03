#ifndef GENERIC_PLUGIN_HPP
#define GENERIC_PLUGIN_HPP

#include "interfaces/i_component_context.hpp"
#include <cassert>

#ifdef _DEBUG
	#define PLG_CALLBACK PluginCallback_d
#else
	#define PLG_CALLBACK PluginCallback
#endif

namespace wgt
{
enum GenericPluginLoadState
{
	Create,
	PostLoad,
	Initialise,
	Finalise,
	Unload,
	Destroy,
	PluginDestroyEnd
};

#define PLG_CALLBACK_FUNC( PluginType ) \
	PluginMain * createPlugin( IComponentContext & contextManager )\
	{\
		auto pluginMain = new PluginType( contextManager );\
		pluginMain->init( #PluginType );\
		return pluginMain;\
	}

class PluginMain
{
public:
	PluginMain();
	virtual ~PluginMain() {}

	virtual bool PostLoad( IComponentContext & /*contextManager*/ ) { return true; }
	virtual void Initialise( IComponentContext & /*contextManager*/ ) {}
	virtual bool Finalise( IComponentContext & /*contextManager*/ ) { return true; }
	virtual void Unload( IComponentContext & /*contextManager*/ ) {}

	void init( const char * name );

private:
	const char * name_;
	
public:
	static void setContext( IComponentContext * context );
	static IComponentContext * getContext();

private:
	static IComponentContext * s_Context_;
	static bool s_ContextInitialized_;
};
} // end namespace wgt
#endif //GENERIC_PLUGIN_HPP
