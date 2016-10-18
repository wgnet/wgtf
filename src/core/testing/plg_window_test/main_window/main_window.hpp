#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core_ui_framework/i_window.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

#include <memory>

namespace wgt
{
class IAction;
class IComponentContext;

class MainWindow
	: public Depends< IViewCreator, IUIApplication, IUIFramework >
{    
public:
    explicit MainWindow( IComponentContext & context );
    ~MainWindow();

	void init();
	void fini();

private:
	void createActions( IUIFramework & uiFramework );
	void destroyActions();

	void close( IAction * action );
	void showShortcutConfig( IAction* action );
	void onTryClose( bool& shouldClose );
	void onClose();

	void onStartUp();

	void addMenuBar( IUIApplication & uiApplication );

private:
	std::unique_ptr< IWindow > mainWindow_;
	std::unique_ptr< IAction > testExit_;
	std::unique_ptr< IAction > shortcutConfig_;
	ConnectionHolder connections_;
};
} // end namespace wgt
#endif // MAINWINDOW_H
