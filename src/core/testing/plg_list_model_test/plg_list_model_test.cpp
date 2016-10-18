#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "test_list_model.hpp"
#include <vector>


namespace wgt
{
/**
* A plugin which creates various styles of lists with sample data
*
* @ingroup plugins
* @image html plg_list_model_test.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ListModelTestPlugin
	: public PluginMain
	, public Depends< IViewCreator >
{
private:
	std::vector<IInterface*> types_;
	wg_future<std::unique_ptr< IView >> oldListView_;
	wg_future<std::unique_ptr< IView >> oldShortListView_;
	wg_future<std::unique_ptr< IView >> listView_;
	wg_future<std::unique_ptr< IView >> shortListView_;
	wg_future<std::unique_ptr< IView >> multiColumnListView_;
	std::shared_ptr<AbstractListModel> listModel_;

public:
	//==========================================================================
	ListModelTestPlugin(IComponentContext & contextManager )
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
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );
		
		std::unique_ptr< IListModel > oldListModel( new OldTestListModel() );
		std::unique_ptr< IListModel > oldShortListModel( new OldTestListModel( true ) );
		listModel_ = std::make_shared<TestListModel>();

		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			oldListView_ = viewCreator->createView(
				"plg_list_model_test/test_list_panel_old.qml",
				std::move( oldListModel ) );

			oldShortListView_ = viewCreator->createView(
				"plg_list_model_test/test_short_list_panel_old.qml",
				std::move( oldShortListModel ) );

			listView_ = viewCreator->createView(
				"plg_list_model_test/test_list_panel.qml",
				listModel_ );

		}
	}


	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		if (listView_.valid())
		{
			auto view = listView_.get();
			uiApplication->removeView( *view );
			view = nullptr;
		}
		if (oldShortListView_.valid())
		{
            auto view = oldShortListView_.get();
			uiApplication->removeView( *view );
            view = nullptr;
		}
		if (oldListView_.valid())
		{
            auto view = oldListView_.get();
			uiApplication->removeView( *view );
            view = nullptr;
		}
		listModel_ = nullptr;
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


PLG_CALLBACK_FUNC( ListModelTestPlugin )
} // end namespace wgt
