#ifndef TEST_PLUGIN_LOADER_HPP
#define TEST_PLUGIN_LOADER_HPP

#include "core_unit_test/test_global_context.hpp"
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
	typedef std::vector<std::wstring> PluginList;
	TestPluginLoader();
	virtual ~TestPluginLoader();

private:
	InterfacePtr interface_ = nullptr;
	GenericPluginManager& pluginManager_;
	PluginList plugins_;
};
} // end namespace wgt
#endif // TEST_PLUGIN_LOADER_HPP
