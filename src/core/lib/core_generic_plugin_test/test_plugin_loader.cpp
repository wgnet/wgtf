#include "test_plugin_loader.hpp"

#include "memory_plugin_context_creator.hpp"

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include <cassert>

namespace wgt
{
TestPluginLoader::TestPluginLoader()
{
	IPluginContextManager& contextManager = pluginManager_.getContextManager();
	contextManager.getGlobalContext()->registerInterface(new MemoryPluginContextCreator);
}

void TestPluginLoader::load(const PluginList& plugins)
{
	assert(plugins_.empty());
	plugins_ = plugins;
	pluginManager_.loadPlugins(plugins_);
}

TestPluginLoader::~TestPluginLoader()
{
	pluginManager_.unloadPlugins(plugins_);
	plugins_.clear();
}
} // end namespace wgt
