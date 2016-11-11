#include "pch.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_unit_test/unit_test.hpp"

#include "python27_unit_test.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
 *	Runs python unit tests.
 */
class Python27UnitTestPlugin : public PluginMain
{
public:
	Python27UnitTestPlugin(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		// Pass reference to unit tests
		g_contextManager = &contextManager;

		auto clp = contextManager.queryInterface<ICommandLineParser>();
		assert(clp != nullptr);

		BWUnitTest::runTest("python27_unit_test", clp->argc(), clp->argv());
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
	}
};

PLG_CALLBACK_FUNC(Python27UnitTestPlugin)
} // end namespace wgt
