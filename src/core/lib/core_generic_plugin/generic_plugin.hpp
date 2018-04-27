#ifndef GENERIC_PLUGIN_HPP
#define GENERIC_PLUGIN_HPP

#include "interfaces/i_component_context.hpp"
#include "common_include/qrc_loader.hpp"
#include "core_dependency_system/context_callback_helper.hpp"
#include <cassert>
#include <utility>

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

struct PluginCreator
{
	template <typename T>
	static typename std::enable_if<!std::is_default_constructible<T>::value, T*>::type createPlugin(
	IComponentContext& contextManager)
	{
		return new T(contextManager);
	}

	template <typename T>
	static typename std::enable_if<std::is_default_constructible<T>::value, T*>::type createPlugin(IComponentContext& )
	{
		return new T();
	}
};

#define PLG_CALLBACK_FUNC(PluginType)                                              \
	PluginMain* createPlugin(IComponentContext& contextManager)                    \
	{                                                                              \
		auto pluginMain = PluginCreator::createPlugin<PluginType>(contextManager); \
		pluginMain->init(#PluginType);                                             \
		return pluginMain;                                                         \
	}

class PluginMain
	: public ContextCallBackHelper
{
public:
	PluginMain();
	virtual ~PluginMain()
	{
	}

	/**
	* Called after all plugins have been loaded. Interface registration should occur here.
	* @param IComponentContext provides access to services provided by other plugins
	*/
	virtual bool PostLoad(IComponentContext& /*contextManager*/)
	{
		return true;
	}

	/**
	* Called after all plugins have registered their interfaces. Interface querying can occur here,
	* but performing initialisation this way is deprecated, as it does not support plug-in hotloading.
	* Use ContextCallBackHelper::registerCallback() in your constructor instead and see the documentation
	* for ContextCallBackHelper for a more detailed explanation and code examples.
	* @param IComponentContext provides access to services provided by other plugins
	*/
	virtual void Initialise(IComponentContext& /*contextManager*/)
	{
	}

	/**
	* Cleanup stage during which all interfaces can still be accessed
	* @param IComponentContext provides access to services provided by other plugins
	*/
	virtual bool Finalise(IComponentContext& /*contextManager*/)
	{
		return true;
	}

	/**
	* Called before any plugins have been unloaded. Interface deregistration should occur here
	* @param IComponentContext provides access to services provided by other plugins
	*/
	virtual void Unload(IComponentContext& /*contextManager*/)
	{
	}

	void init(const char* name);

private:
	const char* name_;
};
} // end namespace wgt
#endif // GENERIC_PLUGIN_HPP
