#include "pch.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_unit_test/unit_test.hpp"
#include "core_variant/variant.hpp"

#include "python27_unit_test.hpp"


namespace wgt
{
/**
 *	Runs unit tests.
 */
class MainApplication
	: public Implements< IApplication >
{
public:
	MainApplication( IComponentContext& contextManager )
		: contextManager_( contextManager )
	{
	}


	int startApplication() override
	{
		// Pass reference to unit tests
		g_contextManager = &contextManager_;

		auto clp = contextManager_.queryInterface< ICommandLineParser >();
		assert( clp != nullptr );

		return BWUnitTest::runTest( "python27_unit_test",
			clp->argc(),
			clp->argv() );
	}

	void quitApplication() override
	{
		g_contextManager = nullptr;
	}

private:
	IComponentContext & contextManager_;
};


/**
 *	Start up main application.
 */
class Python27UnitTestPlugin
	: public PluginMain
{
public:
	Python27UnitTestPlugin( IComponentContext & contextManager )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		contextManager.registerInterface(
			new MainApplication( contextManager ) );
		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}


	void Unload( IComponentContext & contextManager ) override
	{
	}
};

PLG_CALLBACK_FUNC( Python27UnitTestPlugin )
} // end namespace wgt
