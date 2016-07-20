#include "core_data_model/i_tree_model.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_window.hpp"
#include "test_tree_model.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
/**
* A plugin which displays a tree as a list with sample data
*
* @ingroup plugins
* @image html plg_tree_to_list_test.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class TreeToListTest
	: public PluginMain
	, public Depends< IViewCreator >
{
public:
	//==========================================================================
	TreeToListTest( IComponentContext & contextManager )
		: Depends( contextManager )
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
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( uiFramework != nullptr );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );

		// Create the view and present it
		auto model = std::unique_ptr< ITreeModel >( new TestTreeModel() );

		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			testView_ = viewCreator->createView(
				"plg_tree_to_list_test/test_tree_to_list_panel.qml",
				std::move(model) );
		}
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );

		if (testView_.valid())
		{
            auto view = testView_.get();
			uiApplication->removeView( *view );
			view = nullptr;
		}

		return true;
	}

private:
	
	wg_future<std::unique_ptr< IView >> testView_;
};

PLG_CALLBACK_FUNC( TreeToListTest )
} // end namespace wgt
