#include "memory_plugin_context_creator.hpp"
#include "plugin_memory_allocator.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include <typeinfo>

namespace wgt
{
//==============================================================================
InterfacePtr MemoryPluginContextCreator::createContext(const wchar_t* contextId)
{
	return std::make_shared<InterfaceHolder<IMemoryAllocator>>(new PluginMemoryAllocator(contextId), false);
}

//==============================================================================
const char* MemoryPluginContextCreator::getType() const
{
	return typeid(IMemoryAllocator).name();
}

//==============================================================================
bool MemoryPluginContextCreator::ownsSubContext() const
{
	return true;
}
} // end namespace wgt
