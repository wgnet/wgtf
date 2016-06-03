#ifndef GENERIC_PLUGIN_MANAGER_HPP
#define GENERIC_PLUGIN_MANAGER_HPP

#include "core_common/platform_dll.hpp"
#include "core_generic_plugin/interfaces/i_memory_allocator.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>

namespace wgt
{
class IPluginContextManager;

class GenericPluginManager
{
public:
	typedef std::vector< HMODULE > PluginList;
	typedef std::vector< std::wstring > PluginNameList;

    GenericPluginManager(bool applyDebugPostfix = true);
	virtual ~GenericPluginManager();

	void loadPlugins( const PluginNameList& plugins );

	void unloadPlugins( const PluginNameList& plugins );
	void unloadPlugins( const PluginList& plugins );

	IPluginContextManager & getContextManager() const;

	template< class T >
	T * queryInterface()
	{
		return reinterpret_cast< T * >( queryInterface(
			typeid( T ).name() ) );
	}

private:
	void * queryInterface( const char * name ) const;

	typedef std::function< bool (HMODULE) > NotifyFunction;
	void notifyPlugins( const PluginList& plugins, NotifyFunction func );

	HMODULE loadPlugin( const std:: wstring& filename );
	bool unloadPlugin( HMODULE hPlugin );
	void unloadContext( HMODULE hPlugin );

	typedef std::vector< std::pair<std::wstring, HMODULE> > PluginMap;
	PluginMap::iterator findPlugin(HMODULE hPlugin);

	std::wstring processPluginFilename(const std::wstring& filename);

	PluginMap plugins_; // don't use std::map since we need to keep original modules' loading order

	std::map< std::wstring, IMemoryAllocator * >	memoryContext_;
	std::unique_ptr< IPluginContextManager >		contextManager_;
    bool applyDebugPostfix;
};
} // end namespace wgt
#endif //GENERIC_PLUGIN_MANAGER_HPP
