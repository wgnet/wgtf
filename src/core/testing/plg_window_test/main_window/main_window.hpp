#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core_ui_framework/i_window.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include <memory>

namespace wgt
{
class IAction;
class IUIApplication;
class IUIFramework;

class MainWindow
{    
public:
    explicit MainWindow();
    ~MainWindow();

	void init( IUIApplication & uiApplication, IUIFramework & uiFramework );
	void fini();

private:
	void createActions( IUIFramework & uiFramework );
	void destroyActions();

	void close( IAction * action );
	void onTryClose( bool& shouldClose );
	void onClose();

	void onStartUp();

	void addMenuBar( IUIApplication & uiApplication );

private:
	IUIApplication*			   app_;
	IUIFramework*			   uiFramework_;
	std::unique_ptr< IWindow > mainWindow_;
	std::unique_ptr< IAction > testExit_;
	ConnectionHolder connections_;
};
} // end namespace wgt
#endif // MAINWINDOW_H
