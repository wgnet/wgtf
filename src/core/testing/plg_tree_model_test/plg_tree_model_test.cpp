#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "test_tree_model.hpp"
#include <vector>

namespace wgt
{
//==============================================================================
class TreeModelTestPlugin
	: public PluginMain
	, Depends< IViewCreator >
{
private:
	std::vector<IInterface*> types_;
	std::unique_ptr<IView> oldTreeView_;
	std::unique_ptr<IView> treeView_;
	std::shared_ptr<AbstractTreeModel> treeModel_;
public:
	//==========================================================================
	TreeModelTestPlugin(IComponentContext & contextManager )
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
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );

		auto model = std::unique_ptr< ITreeModel >( new TestTreeModelOld() );

		auto viewCreator = get< IViewCreator >();
		if( viewCreator )
		{

			viewCreator->createView(
				"plg_tree_model_test/test_tree_panel_old.qml",
				std::move(model), oldTreeView_);

			treeModel_ = std::make_shared<TestTreeModel>();
			viewCreator->createView(
				"plg_tree_model_test/test_tree_panel.qml",
				treeModel_, treeView_);
		}
	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		if (treeView_ != nullptr)
		{
			uiApplication->removeView( *treeView_ );
		}
		if (oldTreeView_ != nullptr)
		{
			uiApplication->removeView( *oldTreeView_ );
		}
		treeModel_ = nullptr;
		treeView_ = nullptr;
		oldTreeView_ = nullptr;
		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

};


PLG_CALLBACK_FUNC( TreeModelTestPlugin )
} // end namespace wgt
