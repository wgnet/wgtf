#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core_ui_framework/i_progress_dialog.hpp"
#include <memory>
#include <vector>

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

		void init(IUIApplication * uiApplication, IComponentContext & contextManager);
		void fini();

	private:
		std::vector<std::unique_ptr< IAction >> commands_;
		IComponentContext * contextManager_;

		void createToolBar();
		void createMenuBar();
		void createActions();
		void executeTestCommand1(IAction * action);
		void executeTestCommand2(IAction * action);
		void executeProgressStepTest(bool modal, bool cancelable);
		void executeProgressTest(bool modal, bool cancelable);
		void executeProgressDurationTest(bool modal, bool cancelable, long long duration_ms);
		void executeProgressChangeCancelTest(bool modal, bool cancelable);
		void executeAllProgressTests();
	};
} // end namespace wgt
#endif // MAINWINDOW_H
