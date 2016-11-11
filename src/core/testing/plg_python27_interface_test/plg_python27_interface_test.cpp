#include "pch.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "python27_interface_test.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which queries the IPythonScriptingEngine and runs unit tests on its interface
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*       - Python27Plugin
*/
class Python27TestPlugin : public PluginMain
{
public:
	Python27TestPlugin(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		auto clp = contextManager.queryInterface<ICommandLineParser>();
		assert(clp != nullptr);
		// Pass reference to unit tests
		g_contextManager = &contextManager;
		BWUnitTest::runTest("python27_interface_test", clp->argc(), clp->argv());
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
	}
};

PLG_CALLBACK_FUNC(Python27TestPlugin)
} // end namespace wgt
