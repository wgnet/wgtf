#include "main_window.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_window.hpp"

namespace wgt
{
//==============================================================================
MainWindow::~MainWindow()
{
}

//==============================================================================
void MainWindow::init()
{
	auto uiFramework = get<IUIFramework>();
	auto viewCreator = get<IViewCreator>();
	auto uiApplication = get<IUIApplication>();
	assert(uiApplication != nullptr);
	assert(uiFramework != nullptr);

	if (viewCreator)
	{
		ObjectHandle handle;
		viewCreator->createWindow(
		":/plg_window_test/main_window.ui", handle,
		[this, uiApplication, uiFramework](std::unique_ptr<IWindow>& window) {
			mainWindow_ = std::move(window);
			connections_ += mainWindow_->signalClose.connect(std::bind(&MainWindow::onClose, this));
			connections_ +=
			mainWindow_->signalTryClose.connect(std::bind(&MainWindow::onTryClose, this, std::placeholders::_1));
			connections_ += uiApplication->signalStartUp.connect(std::bind(&MainWindow::onStartUp, this));

			uiFramework->loadActionData(":/plg_window_test/actions.xml", IUIFramework::ResourceType::File);
			createActions(*uiFramework);
			addMenuBar(*uiApplication);
		},
		IUIFramework::ResourceType::File);
	}
}

//------------------------------------------------------------------------------
void MainWindow::fini()
{
	auto app = get<IUIApplication>();
	if (app)
	{
		app->removeAction(*testExit_);
		app->removeAction(*shortcutConfig_);
		app->removeWindow(*mainWindow_);
	}
	destroyActions();
	mainWindow_.reset();
	connections_.clear();
}

void MainWindow::close(IAction* action)
{
	mainWindow_->close();
}

void MainWindow::onClose()
{
	auto app = get<IUIApplication>();
	assert(app != nullptr);
	app->quitApplication();
}

void MainWindow::showShortcutConfig(IAction* action)
{
	auto framework = get<IUIFramework>();
	assert(framework != nullptr);
	framework->showShortcutConfig();
}

void MainWindow::onTryClose(bool& shouldClose)
{
	if (shouldClose)
	{
		auto uiFramework = get<IUIFramework>();
		assert(uiFramework != nullptr);
		int result =
		uiFramework->displayMessageBox("Do you want to close?", "Are you sure you want to close the Generic App?",
		                               IUIFramework::Cancel | IUIFramework::Ok);

		if (result == IUIFramework::Cancel)
		{
			shouldClose = false;
		}
	}
}

void MainWindow::createActions(IUIFramework& uiFramework)
{
	// hook application exit
	testExit_ = uiFramework.createAction("Exit", std::bind(&MainWindow::close, this, std::placeholders::_1));
	shortcutConfig_ = uiFramework.createAction("Preferences.Shortcuts",
	                                           std::bind(&MainWindow::showShortcutConfig, this, std::placeholders::_1));
}

void MainWindow::destroyActions()
{
	testExit_.reset();
	shortcutConfig_.reset();
}

void MainWindow::addMenuBar(IUIApplication& uiApplication)
{
	uiApplication.addAction(*testExit_);
	uiApplication.addAction(*shortcutConfig_);
}

void MainWindow::onStartUp()
{
	mainWindow_->show(true);
}
} // end namespace wgt
