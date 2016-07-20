#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "test_model.mpp"
#include <vector>
#include "core_reflection/i_definition_manager.hpp"


namespace wgt
{
//==============================================================================
class AsyncLoaderTestPlugin
	: public PluginMain
	, public Depends< IViewCreator, IDefinitionManager, IUIApplication, IUIFramework >
{
private:
	std::vector<IInterface*> types_;
	std::vector< wg_future<std::unique_ptr< IView >> > views_;
	std::unique_ptr< IAction > openViews_;
	std::unique_ptr< IAction > closeViews_;
	ObjectHandleT<ListData> listViewData1_;
	ObjectHandleT<ListData> listViewData2_;
	ObjectHandleT<ListData> listViewData3_;
	ListDataModel listViewModel1_;
	ListDataModel listViewModel2_;
	ListDataModel listViewModel3_;

public:
	//==========================================================================
	AsyncLoaderTestPlugin(IComponentContext & contextManager )
		: Depends( contextManager )
	{

	}

	//==========================================================================
	virtual bool PostLoad( IComponentContext & contextManager ) override
	{
		return true;
	}

	//==========================================================================
	virtual void Initialise( IComponentContext & contextManager ) override
	{
		auto defManager = get<IDefinitionManager>();
		auto& definitionManager = *defManager;
		REGISTER_DEFINITION( ListData );


		listViewData1_ = defManager->create<ListData>();
		listViewData1_->init( *defManager, 15 );
		listViewData2_ = defManager->create<ListData>();
		listViewData2_->init( *defManager, 30 );
		listViewData3_ = defManager->create<ListData>();
		listViewData3_->init( *defManager, 7 );

		listViewModel1_.init( 15 );
		listViewModel2_.init( 30 );
		listViewModel3_.init( 7 );



		auto uiApplication = get< IUIApplication >();
		auto uiFramework = get< IUIFramework >();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );
		uiFramework->loadActionData( 
			":/plg_async_loader_test/actions.xml", IUIFramework::ResourceType::File );
		auto viewCreator = get< IViewCreator >();
		assert( viewCreator != nullptr );

		openViews_ = uiFramework->createAction( "OpenViews", 
			[this, viewCreator ]( IAction * )
		{
			assert( views_.empty() );
			std::string id = "plg_async_loader_test/list_tab_panel_2.0.qml" + std::to_string(1);
			auto view  = viewCreator->createView(
				"plg_async_loader_test/list_tab_panel_2.0.qml", 
				ObjectHandleT< AbstractListModel >( &listViewModel1_ ), id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_tab_panel_2.0.qml" + std::to_string(2);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_tab_panel_2.0.qml", 
				ObjectHandleT< AbstractListModel >( &listViewModel2_ ), id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_tab_panel_2.0.qml" + std::to_string(3);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_tab_panel_2.0.qml", 
				ObjectHandleT< AbstractListModel >( &listViewModel3_ ), id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}


			id = "plg_async_loader_test/list_tab_panel_1.0.qml" + std::to_string(1);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_tab_panel_1.0.qml", listViewData1_, id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_tab_panel_1.0.qml" + std::to_string(2);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_tab_panel_1.0.qml", listViewData2_, id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_tab_panel_1.0.qml" + std::to_string(3);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_tab_panel_1.0.qml", listViewData3_, id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}


			
			id = "plg_async_loader_test/list_dock_panel_2.0.qml" + std::to_string(1);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_dock_panel_2.0.qml", 
				ObjectHandleT< AbstractListModel >( &listViewModel1_ ), id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_dock_panel_2.0.qml" + std::to_string(2);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_dock_panel_2.0.qml", 
				ObjectHandleT< AbstractListModel >( &listViewModel2_ ), id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_dock_panel_2.0.qml" + std::to_string(3);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_dock_panel_2.0.qml", 
				ObjectHandleT< AbstractListModel >( &listViewModel3_ ), id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}


			id = "plg_async_loader_test/list_dock_panel_1.0.qml" + std::to_string(1);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_dock_panel_1.0.qml", listViewData1_, id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_dock_panel_1.0.qml" + std::to_string(2);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_dock_panel_1.0.qml", listViewData2_, id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}

			id = "plg_async_loader_test/list_dock_panel_1.0.qml" + std::to_string(3);
			view  = viewCreator->createView(
				"plg_async_loader_test/list_dock_panel_1.0.qml", listViewData3_, id.c_str() );
			if(view.valid())
			{
				views_.push_back( std::move(view) );
			}



		}, [this](const IAction*)
		{
			return views_.empty();
		});
		closeViews_ = uiFramework->createAction( "CloseViews", 
			[this, uiApplication]( IAction * )
		{
			closeViews( uiApplication );
		}, [this] (const IAction*)
		{
			return !views_.empty();
		});
		
		uiApplication->addAction( *openViews_ );
		uiApplication->addAction( *closeViews_ );

	}


	//==========================================================================
	virtual bool Finalise( IComponentContext & contextManager ) override
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		uiApplication->removeAction(*openViews_);
		uiApplication->removeAction(*closeViews_);
		closeViews( uiApplication );
		openViews_ = nullptr;
		closeViews_ = nullptr;
		listViewData1_ = nullptr;
		listViewData2_ = nullptr;
		listViewData3_ = nullptr;
		return true;
	}
	//==========================================================================
	virtual void Unload( IComponentContext & contextManager ) override
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}


	void closeViews(IUIApplication* uiApplication)
	{
		for(auto& it : views_)
		{
			 if(it.valid())
			 {
				 auto view = it.get();
				 uiApplication->removeView( *view );
				 view = nullptr;
			 }
		}
		views_.clear();
	}


};


PLG_CALLBACK_FUNC( AsyncLoaderTestPlugin )
} // end namespace wgt
