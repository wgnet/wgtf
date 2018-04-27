#include "test_plugin_loader.hpp"

#include "memory_plugin_context_creator.hpp"

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include <cassert>

namespace wgt
{
TestPluginLoader::TestPluginLoader()
	: pluginManager_(*getPluginManager())
{
	interface_ = getGlobalContext()->registerInterface(new MemoryPluginContextCreator);
	plugins_.push_back(L"plugins/plg_reflection");
	pluginManager_.loadPlugins(plugins_);
}

TestPluginLoader::~TestPluginLoader()
{
	pluginManager_.unloadPlugins(plugins_);
	plugins_.clear();

	if (interface_ != nullptr)
	{
		deregisterInterface(interface_.get());
	}
}
} // end namespace wgt
