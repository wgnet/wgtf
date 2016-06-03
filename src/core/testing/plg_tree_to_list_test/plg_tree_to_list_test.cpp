#include "core_data_model/i_tree_model.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_action_manager.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_variant/variant.hpp"
#include "test_tree_model.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
//==============================================================================
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
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( uiFramework != nullptr );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );

		// Create the view and present it
		auto model = std::unique_ptr< ITreeModel >( new TestTreeModel() );

		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			viewCreator->createView(
				"plg_tree_to_list_test/test_tree_to_list_panel.qml",
				std::move(model),
				testView_);
		}
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );

		if (testView_ != nullptr)
		{
			uiApplication->removeView( *testView_ );
			testView_ = nullptr;
		}

		return true;
	}

private:
	
	std::unique_ptr<IView> testView_;
};

PLG_CALLBACK_FUNC( TreeToListTest )
} // end namespace wgt
