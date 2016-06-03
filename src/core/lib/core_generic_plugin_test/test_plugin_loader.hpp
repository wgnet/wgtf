#ifndef TEST_PLUGIN_LOADER_HPP
#define TEST_PLUGIN_LOADER_HPP

#include "core_generic_plugin_manager/generic_plugin_manager.hpp"


#include <string>
#include <vector>


namespace wgt
{
/**
 *	Plugin loader to be used by unit tests.
 */
class TestPluginLoader
{
public:
	typedef std::vector< std::wstring > PluginList;
	TestPluginLoader();
	virtual ~TestPluginLoader();

	/**
	 *	Call from the constructor of derived classes.
	 *	@param plugins list of plugins to load.
	 */
	void load( const PluginList & pluginList );

	GenericPluginManager pluginManager_;
	PluginList plugins_;
};
} // end namespace wgt
#endif // TEST_PLUGIN_LOADER_HPP
