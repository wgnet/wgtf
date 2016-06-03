#include "memory_plugin_context_creator.hpp"
#include "plugin_memory_allocator.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <typeinfo>

namespace wgt
{
//==============================================================================
IInterface * MemoryPluginContextCreator::createContext(
	const wchar_t * contextId )
{
	return new InterfaceHolder< PluginMemoryAllocator >(
		new PluginMemoryAllocator( contextId ), false );
}


//==============================================================================
const char * MemoryPluginContextCreator::getType() const
{
	return typeid( IMemoryAllocator ).name();
}


//==============================================================================
bool MemoryPluginContextCreator::ownsSubContext() const
{
	return true;
}
} // end namespace wgt
