#include "core_command_system/i_command_manager.hpp"
#include "commands/test_command.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "mainwindow/mainwindow.hpp"


namespace wgt
{
/**
* A plugin which queries the IPanelManager to create a dialog with a progress bar. 
* Once the bar is complete the dialog will automatically close.
*
* @ingroup plugins
* @image html plg_progress_manager_test.png 
* @note Requires Plugins:
*       - @ref coreplugins
*       - ProgressManagerUIPlugin
*/
class TestProgressManagerPlugin
	: public PluginMain
{
private:
	MainWindow mainWindow_;

	std::unique_ptr< TestCommand1 > testCommand1_;
	std::unique_ptr< TestCommand2 > testCommand2_;

public:
	//==========================================================================
	TestProgressManagerPlugin(IComponentContext & contextManager )
		: testCommand1_( new TestCommand1 )
		, testCommand2_( new TestCommand2 )
	{
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		ICommandManager* pCommandSystemProvider = contextManager.queryInterface< ICommandManager >();

		if (pCommandSystemProvider == nullptr)
		{
			return;
		}

		// register test commands
		pCommandSystemProvider->registerCommand( testCommand1_.get() );
		pCommandSystemProvider->registerCommand( testCommand2_.get() );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return;
		}

		mainWindow_.init( uiApplication, contextManager );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		mainWindow_.fini();

		ICommandManager * commandSystemProvider =
			contextManager.queryInterface< ICommandManager >();

		if (commandSystemProvider)
		{
			commandSystemProvider->deregisterCommand( testCommand1_->getId() );
			commandSystemProvider->deregisterCommand( testCommand2_->getId() );
		}

		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}

};


PLG_CALLBACK_FUNC( TestProgressManagerPlugin )
} // end namespace wgt
