#include "pch.hpp"

#include "test_global_context.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_generic_plugin_manager/default_context_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include "core_generic_plugin_manager/plugin_static_initializer.hpp"

namespace wgt
{
GenericPluginManager* getPluginManager()
{
	static std::unique_ptr<GenericPluginManager> s_manager;
	if (!s_manager)
	{
		s_manager.reset(new GenericPluginManager());
		s_manager->getContextManager().createContext(L"Unit Test Context", L"");
	}
	return s_manager.get();
}

//------------------------------------------------------------------------------
void registerStatic(
	const std::shared_ptr< IStaticInitalizer::ExecFunc > & initFunc,
	const std::shared_ptr< IStaticInitalizer::FiniFunc > & finiFunc)
{
	auto context = getGlobalContext();
	if (context)
	{
		static IStaticInitalizer* staticInitializer
			= context->queryInterface<IStaticInitalizer>();
		staticInitializer->registerStaticInitializer(initFunc, finiFunc);
		staticInitializer->initStatics( *context );
	}
}


//------------------------------------------------------------------------------
void deregisterInterface(IInterface* i)
{
	IComponentContext* globalContext = getGlobalContext();
	TF_ASSERT(globalContext);
	globalContext->deregisterInterface(i);
}
}