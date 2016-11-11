#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_dependency_system/depends.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates a panel that when right mouse clicked opens up a context menu
*
* @ingroup plugins
* @image html plg_context_menu_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ContextMenuTest : public PluginMain, public Depends<IViewCreator>
{
public:
	//==========================================================================
	ContextMenuTest(IComponentContext& contextManager) : Depends(contextManager)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		assert(uiFramework != nullptr);

		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);

		// Load the action data
		uiFramework->loadActionData(":/plg_context_menu_test/actions.xml", IUIFramework::ResourceType::File);

		// Create actions and add them to the UI Application
		using namespace std::placeholders;
		cmTestOpen_ = uiFramework->createAction(
		"CMTestExplorerOpen", std::bind(&ContextMenuTest::executeOpen, this, _1),
		std::bind(&ContextMenuTest::canExecuteOpen, this, _1), std::bind(&ContextMenuTest::canCheckOpen, this, _1));
		uiApplication->addAction(*cmTestOpen_);

		cmTestCheckOut_ =
		uiFramework->createAction("CMTestPerforceCheckOut", std::bind(&ContextMenuTest::executeCheckOut, this, _1),
		                          std::bind(&ContextMenuTest::canTestPerforce, this, _1));
		uiApplication->addAction(*cmTestCheckOut_);

		testActions_.push_back(uiFramework->createAction("TestMenu.Fifth:2", [](IAction*) {}));
		testActions_.push_back(uiFramework->createAction("TestMenu.Fourth:1", [](IAction*) {}));
		testActions_.push_back(uiFramework->createAction("TestMenu.Second", [](IAction*) {}));
		testActions_.push_back(uiFramework->createAction("TestMenu.Third", [](IAction*) {}));
		testActions_.push_back(uiFramework->createAction("TestMenu.First", [](IAction*) {}));

		for (auto& testAction : testActions_)
		{
			uiApplication->addAction(*testAction);
		}

		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			// Create the view and present it
			testView_ = viewCreator->createView("plg_context_menu_test/test_contextmenu_panel.qml", ObjectHandle());
		}
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);

		uiApplication->removeAction(*cmTestOpen_);
		uiApplication->removeAction(*cmTestCheckOut_);

		for (auto& testAction : testActions_)
		{
			uiApplication->removeAction(*testAction);
		}

		cmTestOpen_.reset();
		cmTestCheckOut_.reset();

		testActions_.clear();

		if (testView_.valid())
		{
			auto view = testView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}

		return true;
	}

	//==========================================================================
	bool canExecuteOpen(const IAction* action)
	{
		return true;
	}

	bool canTestPerforce(const IAction* action)
	{
		return false;
	}

	bool canCheckOpen(const IAction* action)
	{
		return true;
	}

	void executeOpen(IAction* action)
	{
		unsigned int echoValue = 0;

		if (action != nullptr)
		{
			Variant& variant = action->getData();
			if (variant.canCast<unsigned int>())
			{
				echoValue = variant.cast<unsigned int>();
			}
		}

		NGT_DEBUG_MSG("Open file context menu item clicked: %d !\n", echoValue);
	}

	//==========================================================================
	void executeCheckOut(IAction* action)
	{
		NGT_DEBUG_MSG("Perforce check out context menu item clicked!\n");
	}

private:
	wg_future<std::unique_ptr<IView>> testView_;
	std::unique_ptr<IAction> cmTestCheckOut_;
	std::unique_ptr<IAction> cmTestOpen_;
	std::vector<std::unique_ptr<IAction>> testActions_;
};

PLG_CALLBACK_FUNC(ContextMenuTest)
} // end namespace wgt
