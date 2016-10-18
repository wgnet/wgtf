#include "mainwindow.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "commands/test_command.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"

using namespace std::chrono;
static const milliseconds noms(0);
static const milliseconds noDisplayDuration(1000);
static const milliseconds progressDuration(3000);
static const milliseconds displayDuration(2000);

namespace wgt
{
	//==============================================================================
	MainWindow::MainWindow()
		: contextManager_(nullptr)
	{
	}

	//==============================================================================
	MainWindow::~MainWindow()
	{
	}

	//==============================================================================
	void MainWindow::init(IUIApplication * uiApplication, IComponentContext & contextManager)
	{
		contextManager_ = &contextManager;

		createActions();

		for ( auto& command : commands_ )
		{
			uiApplication->addAction(*command);
		}
	}

	//==============================================================================
	void MainWindow::fini()
	{
		auto uiApplication = contextManager_->queryInterface< IUIApplication >();
		assert(uiApplication != nullptr);

		for ( auto& command : commands_ )
		{
			uiApplication->removeAction(*command);
		}
		commands_.clear();
	}

	// =============================================================================
	void MainWindow::createActions()
	{
		IUIFramework * uiFramework = contextManager_->queryInterface< IUIFramework >();
		assert(uiFramework);

		uiFramework->loadActionData(
			":/plg_progress_manager_test/actions.xml",
			IUIFramework::ResourceType::File);

		// TestCommand1/TestCommand2 QActions
		using namespace std::placeholders;

		commands_.emplace_back(uiFramework->createAction("TestCommand1",
			std::bind(&MainWindow::executeTestCommand1, this, _1)));

		commands_.emplace_back(uiFramework->createAction("TestCommand2",
			std::bind(&MainWindow::executeTestCommand2, this, _1)));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal",
			[this](IAction*){ executeProgressTest(true, false); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless",
			[this](IAction*){ executeProgressTest(false, false); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal Cancelable",
			[this](IAction*){ executeProgressTest(true, true); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless Cancelable",
			[this](IAction*){ executeProgressTest(false, true); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal Step",
			[this](IAction*){ executeProgressStepTest(true, false); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless Step",
			[this](IAction*){ executeProgressStepTest(false, false); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal Cancelable Step",
			[this](IAction*){ executeProgressStepTest(true, true); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless Cancelable Step",
			[this](IAction*){ executeProgressStepTest(false, true); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal No Display",
			[this](IAction*){ executeProgressDurationTest(true, false, noDisplayDuration.count()); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless No Display",
			[this](IAction*){ executeProgressDurationTest(false, false, noDisplayDuration.count()); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal Cancelable No Display",
			[this](IAction*){ executeProgressDurationTest(true, true, noDisplayDuration.count()); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless Cancelable No Display",
			[this](IAction*){ executeProgressDurationTest(false, true, noDisplayDuration.count()); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal Change from Cancelable",
			[this, uiFramework](IAction*){ executeProgressChangeCancelTest(true, true); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless Change from Cancelable",
			[this, uiFramework](IAction*){ executeProgressChangeCancelTest(false, true); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modal Change to Cancelable",
			[this, uiFramework](IAction*){ executeProgressChangeCancelTest(true, false); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.Modeless Change to Cancelable",
			[this, uiFramework](IAction*){ executeProgressChangeCancelTest(false, false); }));

		commands_.emplace_back(uiFramework->createAction("Test Progress.All",
			[this](IAction*){ executeAllProgressTests(); }));
	}

	// =============================================================================
	void MainWindow::executeTestCommand1(IAction * action)
	{
		// Queue the TestCommand1
		ICommandManager * commandSystemProvider = contextManager_->queryInterface< ICommandManager >();
		assert(commandSystemProvider);
		if ( commandSystemProvider == nullptr )
		{
			return;
		}

		const char * commandId = getClassIdentifier<TestCommand1>();

		// Tell the Progress Manager to handle this command
		commandSystemProvider->notifyHandleCommandQueued(commandId);
		auto commandIns = commandSystemProvider->queueCommand(commandId);
		commandSystemProvider->waitForInstance(commandIns);
	}

	// =============================================================================
	void MainWindow::executeTestCommand2(IAction * action)
	{
		// Queue the TestCommand2
		ICommandManager * commandSystemProvider = contextManager_->queryInterface< ICommandManager >();
		assert(commandSystemProvider);
		if ( commandSystemProvider == nullptr )
		{
			return;
		}

		const char * commandId = getClassIdentifier<TestCommand2>();

		// Tell the Progress Manager to handle this command
		commandSystemProvider->notifyHandleCommandQueued(commandId);
		auto commandIns = commandSystemProvider->queueCommand(commandId);
		commandSystemProvider->waitForInstance(commandIns);
	}

	// =============================================================================
	// using QtProgressDialog
	void MainWindow::executeProgressDurationTest(bool modal, bool cancelable, long long duration_ms)
	{
		IUIFramework * uiFramework = contextManager_->queryInterface< IUIFramework >();
		assert(uiFramework);

		auto cancelText = cancelable ? "Cancel" : "";

		auto start = steady_clock::now();
		if ( modal )
		{
			auto dlg = uiFramework->createProgressDialog("Modal Progress", "Progress:", cancelText, 0, 100, displayDuration);
			while ( dlg->value() < dlg->maximum() )
			{
				if ( dlg->wasCanceled() )
					break;
				auto elapsed = duration_cast<milliseconds>( steady_clock::now() - start );
				dlg->setValue(static_cast<int>( elapsed.count() * 100 / duration_ms ));
			}
		}
		else
		{
			uiFramework->createModelessProgressDialog("Modeless Progress", "Progress:", cancelText, 0, 100, displayDuration,
				[this, start, duration_ms](IProgressDialog& dialog){
				auto elapsed = duration_cast<milliseconds>( ( steady_clock::now() - start ) );
				dialog.setValue(static_cast<int>( elapsed.count() * 100 / duration_ms ));
			});
		}
	}

	void MainWindow::executeProgressChangeCancelTest(bool modal, bool cancelable)
	{
		IUIFramework * uiFramework = contextManager_->queryInterface< IUIFramework >();
		assert(uiFramework);

		auto start = steady_clock::now();
		auto cancelText = cancelable ? "Cancel" : "";
		auto updateCancelText = !cancelable ? "Cancel" : "";
		if ( modal )
		{
			auto dlg = uiFramework->createProgressDialog("Modal Change Cancel", "Progress:", cancelText, 0, 100, noms);
			while ( dlg->value() < dlg->maximum() )
			{
				if ( dlg->wasCanceled() )
					break;
				// Change cancelability after half way done
				if ( dlg->value() > dlg->maximum() / 2 )
					dlg->setCancelButtonText(updateCancelText);
				auto elapsed = duration_cast<milliseconds>( steady_clock::now() - start );
				dlg->setValue(static_cast<int>( elapsed.count() * 100 / progressDuration.count() ));
			}
		}
		else
		{
			uiFramework->createModelessProgressDialog("Modeless Change Cancel", "Progress:", cancelText, 0, 100, noms,
				[this, start, updateCancelText](IProgressDialog& dlg){
				// Change cancelability after half way done
				if ( dlg.value() > dlg.maximum() / 2 )
					dlg.setCancelButtonText(updateCancelText);
				auto elapsed = duration_cast<milliseconds>( steady_clock::now() - start );
				dlg.setValue(static_cast<int>( elapsed.count() * 100 / progressDuration.count() ));
			});
		}
	}

	void MainWindow::executeProgressTest(bool modal, bool cancelable)
	{
		IUIFramework * uiFramework = contextManager_->queryInterface< IUIFramework >();
		assert(uiFramework);

		auto cancelText = cancelable ? "Cancel" : "";

		auto start = steady_clock::now();
		if ( modal )
		{
			auto dlg = uiFramework->createProgressDialog("Modal Progress", "Progress:", cancelText, 0, 100, noms);
			while ( dlg->value() < dlg->maximum() )
			{
				if ( dlg->wasCanceled() )
					break;
				auto elapsed = duration_cast<milliseconds>( steady_clock::now() - start );
				dlg->setValue(static_cast<int>( elapsed.count() * 100 / progressDuration.count() ));
			}
		}
		else
		{
			uiFramework->createModelessProgressDialog("Modeless Progress", "Progress:", cancelText, 0, 100, noms,
				[this, start](IProgressDialog& dialog){
				auto elapsed = duration_cast<milliseconds>( ( steady_clock::now() - start ) );
				dialog.setValue(static_cast<int>( elapsed.count() * 100 / progressDuration.count() ));
			});
		}
	}

#ifdef _DEBUG
	static const uint32_t max_progress = 100;
#else
	static const uint32_t max_progress = 10000;
#endif
	void MainWindow::executeProgressStepTest(bool modal, bool cancelable)
	{
		IUIFramework * uiFramework = contextManager_->queryInterface< IUIFramework >();
		assert(uiFramework);

		auto cancelText = cancelable ? "Cancel" : "";

		if ( modal )
		{
			auto dlg = uiFramework->createProgressDialog("Modal Progress", "Stepping:", cancelText, 0, max_progress, noms);
			while ( dlg->value() < dlg->maximum() )
			{
				if ( dlg->wasCanceled() )
					break;
				dlg->step();
			}
		}
		else
		{
			uiFramework->createModelessProgressDialog("Modeless Progress", "Stepping:", cancelText, 0, max_progress, noms,
				[this](IProgressDialog& dialog){ dialog.step(); });
		}
	}

	// =============================================================================
	void MainWindow::executeAllProgressTests()
	{
		// Weave Modeless then modal 
		executeProgressTest(false, true);
		executeProgressTest(true, true);
		executeProgressTest(false, false);
		executeProgressTest(true, false);

		executeProgressStepTest(false, true);
		executeProgressStepTest(true, true);
		executeProgressStepTest(false, false);
		executeProgressStepTest(true, false);

		executeProgressDurationTest(false, true, noDisplayDuration.count());
		executeProgressDurationTest(true, true, noDisplayDuration.count());
		executeProgressDurationTest(false, false, noDisplayDuration.count());
		executeProgressDurationTest(true, false, noDisplayDuration.count());

		executeProgressChangeCancelTest(true, true);
		executeProgressChangeCancelTest(false, true);
		executeProgressChangeCancelTest(true, false);
		executeProgressChangeCancelTest(false, false);
	}
} // end namespace wgt
