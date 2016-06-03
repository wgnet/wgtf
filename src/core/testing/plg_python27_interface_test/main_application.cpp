#include "pch.hpp"
#include "main_application.hpp"
#include "python27_interface_test.hpp"

#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include <cassert>


namespace wgt
{
MainApplication::MainApplication( IComponentContext & contextManager )
	: contextManager_( contextManager )
{
}


int MainApplication::startApplication() /* override */
{

	// Pass reference to unit tests
	g_contextManager = &contextManager_;

	auto clp = contextManager_.queryInterface< ICommandLineParser >();
	assert( clp != nullptr );

	return BWUnitTest::runTest( "python27_interface_test",
		clp->argc(),
		clp->argv() );
}


void MainApplication::quitApplication() /* override */
{
}
} // end namespace wgt
