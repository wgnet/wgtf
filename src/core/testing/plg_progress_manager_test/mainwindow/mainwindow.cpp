#include "mainwindow.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "commands/test_command.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"

namespace wgt
{
//==============================================================================
MainWindow::MainWindow()
	: contextManager_( nullptr )
{
}

//==============================================================================
MainWindow::~MainWindow()
{
}

//==============================================================================
void MainWindow::init( IUIApplication * uiApplication, IComponentContext & contextManager )
{
	contextManager_ = &contextManager;

	createActions();

	uiApplication->addAction( *testCommand1_ );
	uiApplication->addAction( *testCommand2_ );
}

//==============================================================================
void MainWindow::fini()
{
	auto uiApplication = contextManager_->queryInterface< IUIApplication >();
	assert( uiApplication != nullptr );
	uiApplication->removeAction( *testCommand1_ );
	uiApplication->removeAction( *testCommand2_ );
	testCommand2_.reset();
	testCommand1_.reset();
}

// =============================================================================
void MainWindow::createActions()
{
	IUIFramework * uiFramework = contextManager_->queryInterface< IUIFramework >();
	assert( uiFramework );

	uiFramework->loadActionData( 
		":/plg_progress_manager_test/actions.xml",
		IUIFramework::ResourceType::File );

	// TestCommand1/TestCommand2 QActions
	using namespace std::placeholders;

	testCommand1_ = uiFramework->createAction( "TestCommand1", 
		std::bind( &MainWindow::executeTestCommand1, this, _1 ) );

	testCommand2_ = uiFramework->createAction( "TestCommand2", 
		std::bind( &MainWindow::executeTestCommand2, this, _1 ) );
}

// =============================================================================
void MainWindow::executeTestCommand1( IAction * action )
{
	// Queue the TestCommand1
	ICommandManager * commandSystemProvider = contextManager_->queryInterface< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == nullptr)
	{
		return;
	}

	const char * commandId = getClassIdentifier<TestCommand1>();

	// Tell the Progress Manager to handle this command
	commandSystemProvider->notifyHandleCommandQueued( commandId );
	auto commandIns = commandSystemProvider->queueCommand( commandId );
	commandSystemProvider->waitForInstance( commandIns );
}

// =============================================================================
void MainWindow::executeTestCommand2( IAction * action )
{
	// Queue the TestCommand2
	ICommandManager * commandSystemProvider = contextManager_->queryInterface< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == nullptr)
	{
		return;
	}

	const char * commandId = getClassIdentifier<TestCommand2>();

	// Tell the Progress Manager to handle this command
	commandSystemProvider->notifyHandleCommandQueued( commandId );
	auto commandIns = commandSystemProvider->queueCommand( commandId );
	commandSystemProvider->waitForInstance( commandIns );
}
} // end namespace wgt
