#pragma once

#include <memory>

#include "common_include/i_static_initializer.hpp"
#include "core_common/assert.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"

namespace wgt
{
class IInterface;
using InterfacePtr = std::shared_ptr<IInterface>;
class IDefinitionManager;

GenericPluginManager* getPluginManager();
void registerStatic(
	const std::shared_ptr< IStaticInitalizer::ExecFunc > & fn,
	const std::shared_ptr< IStaticInitalizer::FiniFunc > & finiFunc);
void deregisterInterface(IInterface* i);

inline IComponentContext* getGlobalContext()
{
	return getPluginManager()->getContextManager().getGlobalContext();
}

template <typename T>
InterfacePtr registerInterface(T* i)
{
	IComponentContext* globalContext = getGlobalContext();
	TF_ASSERT(globalContext);
	return globalContext->registerInterface<T>(i, false);
}

}