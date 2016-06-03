#include "main_window.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"

#include "core_data_model/reflection/reflected_tree_model.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"

namespace wgt
{
//==============================================================================
MainWindow::MainWindow()
	: app_( nullptr )
{
}

//==============================================================================
MainWindow::~MainWindow()
{

}

//==============================================================================
void MainWindow::init( IUIApplication & uiApplication, IUIFramework & uiFramework )
{
	uiFramework.loadActionData( 
		":/plg_window_test/actions.xml", IUIFramework::ResourceType::File );
	mainWindow_ = uiFramework.createWindow( 
		":/plg_window_test/main_window.ui", IUIFramework::ResourceType::File );
	uiApplication.addWindow( *mainWindow_ );

	createActions( uiFramework );
	addMenuBar( uiApplication );
	app_ = &uiApplication;
	uiFramework_ = &uiFramework;

	connections_ += mainWindow_->signalClose.connect( std::bind( &MainWindow::onClose, this ) );
	connections_ += mainWindow_->signalTryClose.connect( std::bind( &MainWindow::onTryClose, this, std::placeholders::_1 ) );
	connections_ += uiApplication.signalStartUp.connect( std::bind( &MainWindow::onStartUp, this ) );
}

//------------------------------------------------------------------------------
void MainWindow::fini()
{
	app_->removeAction( *testExit_ );
	app_->removeWindow( *mainWindow_ );
	destroyActions();
	mainWindow_.reset();
	connections_.clear();
}

void MainWindow::close( IAction * action )
{
	mainWindow_->close();
}

void MainWindow::onClose()
{
	assert( app_ != nullptr );
	app_->quitApplication();
}

void MainWindow::onTryClose( bool& shouldClose )
{
	if (shouldClose)
	{
		int result = uiFramework_->displayMessageBox( "Do you want to close?", 
														"Are you sure you want to close the Generic App?",
														IUIFramework::Cancel| IUIFramework::Ok );

		if ( result == IUIFramework::Cancel )
		{
			shouldClose = false;
		}
	}
}

void MainWindow::createActions( IUIFramework & uiFramework )
{
	// hook application exit
	testExit_ = uiFramework.createAction(
		"Exit", 
		std::bind( &MainWindow::close, this, std::placeholders::_1 ) );
}

void MainWindow::destroyActions()
{
	testExit_.reset();
}

void MainWindow::addMenuBar( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testExit_ );
}

void MainWindow::onStartUp()
{
	mainWindow_->show( true );
}
} // end namespace wgt
