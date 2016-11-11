#include "core_generic_plugin/generic_plugin.hpp"

#include "core_python27/defined_instance.hpp"
#include "core_python27/listener_hooks.hpp"
#include "core_python27/scripting_engine.hpp"
#include "core_python27/script_object_definition_registry.hpp"
#include "core_python27/type_converters/converter_queue.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"

namespace wgt
{
/**
* A plugin which controls initialization and finalization of Python
* and registers the IPythonScriptingEngine Python interface to be used by other plugins
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class Python27Plugin : public PluginMain
{
public:
	Python27Plugin(IComponentContext& contextManager)
	    : interpreter_(contextManager), definitionRegistry_(contextManager), typeConverterQueue_(contextManager),
	      hookListener_(new ReflectedPython::HookListener())
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		const bool transferOwnership = false;
		interfaces_.push(contextManager.registerInterface(&interpreter_, transferOwnership));
		interfaces_.push(contextManager.registerInterface(&definitionRegistry_, transferOwnership));
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		// Initialize listener hooks
		const auto pDefinitionManager = contextManager.queryInterface<IDefinitionManager>();
		if (pDefinitionManager == nullptr)
		{
			NGT_ERROR_MSG("Could not get IDefinitionManager\n");
			return;
		}
		pDefinitionManager->registerPropertyAccessorListener(
		std::static_pointer_cast<PropertyAccessorListener>(hookListener_));
		g_pHookContext = &contextManager;
		g_listener = hookListener_;

		interpreter_.init();
		definitionRegistry_.init();
		typeConverterQueue_.init();
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		typeConverterQueue_.fini();
		definitionRegistry_.fini();
		interpreter_.fini();

		// Finalize listener hooks
		// All reflected Python objects should have been removed by this point
		const auto pDefinitionManager = contextManager.queryInterface<IDefinitionManager>();
		if (pDefinitionManager == nullptr)
		{
			NGT_ERROR_MSG("Could not get IDefinitionManager\n");
			return false;
		}
		pDefinitionManager->deregisterPropertyAccessorListener(
		std::static_pointer_cast<PropertyAccessorListener>(hookListener_));
		g_listener.reset();
		g_pHookContext = nullptr;

		return true;
	}

	void Unload(IComponentContext& contextManager)
	{
		while (!interfaces_.empty())
		{
			contextManager.deregisterInterface(interfaces_.top());
			interfaces_.pop();
		}
	}

private:
	std::stack<IInterface*> interfaces_;
	Python27ScriptingEngine interpreter_;
	ReflectedPython::ScriptObjectDefinitionRegistry definitionRegistry_;
	PythonType::ConverterQueue typeConverterQueue_;
	std::shared_ptr<ReflectedPython::HookListener> hookListener_;
};

PLG_CALLBACK_FUNC(Python27Plugin)
} // end namespace wgt
