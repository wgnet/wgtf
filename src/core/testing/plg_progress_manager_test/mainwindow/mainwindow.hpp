#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

namespace wgt
{
class IAction;
class IUIApplication;
class IComponentContext;

class MainWindow
{    
public:
    explicit MainWindow();
    ~MainWindow();

	void init( IUIApplication * uiApplication, IComponentContext & contextManager );
	void fini();

private:
	std::unique_ptr< IAction > testCommand1_;
	std::unique_ptr< IAction > testCommand2_;
	IComponentContext * contextManager_;

	void createToolBar();
	void createMenuBar();
	void createActions();
	void executeTestCommand1( IAction * action );
	void executeTestCommand2( IAction * action );
};
} // end namespace wgt
#endif // MAINWINDOW_H
