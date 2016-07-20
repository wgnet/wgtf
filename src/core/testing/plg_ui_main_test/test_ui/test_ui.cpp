#include "test_ui.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_command_system/i_env_system.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "interfaces/i_datasource.hpp"

#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_data_model/reflection/reflected_tree_model_new.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_copy_paste/i_copy_paste_manager.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
//==============================================================================
TestUI::TestUI( IComponentContext & context )
	: Depends( context )
	, context_( context )
{
}

//==============================================================================
TestUI::~TestUI()
{

}

//==============================================================================
void TestUI::init( IUIApplication & uiApplication, IUIFramework & uiFramework )
{
	app_ = &uiApplication;
	fw_ = &uiFramework;

	createActions( uiFramework );
	addActions( uiApplication );
}

//------------------------------------------------------------------------------
void TestUI::fini()
{
	closeAll();
	destroyActions();
}

// =============================================================================
void TestUI::createActions( IUIFramework & uiFramework )
{
	// hook open/close
	testOpen_ = uiFramework.createAction(
		"Open", 
		std::bind( &TestUI::open, this ),
		std::bind( &TestUI::canOpen, this ) );

	testClose_ = uiFramework.createAction(
		"Close", 
		std::bind( &TestUI::close, this ),
		std::bind( &TestUI::canClose, this ) );

	ICommandManager * commandSystemProvider =
		get< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == NULL)
	{
		return;
	}
}

// =============================================================================
void TestUI::createViews( IUIFramework & uiFramework, IDataSource* dataSrc, int envIdx )
{
	auto defManager = get<IDefinitionManager>(); 
	assert( defManager != nullptr );
	auto controller = get<IReflectionController>();
	assert( controller != nullptr );
	auto viewCreator = get< IViewCreator >();
	assert(viewCreator != nullptr);

    test1Models_.emplace_back( new ReflectedTreeModelNew( context_, dataSrc->getTestPage() ) );
	std::string uniqueName1 = dataSrc->description() + std::string("testing_ui_main/test_property_tree_panel.qml");
	auto view = viewCreator->createView(
		"testing_ui_main/test_property_tree_panel.qml",
		test1Models_.back().get(),
		[&] (IView & view )
		{
            view.registerListener(this);
		},
		uniqueName1.c_str() );
	test1Views_.emplace_back( TestViews::value_type( std::move( view.get() ), envIdx ) );

    auto model = std::unique_ptr< ITreeModel >(
        new ReflectedTreeModel( dataSrc->getTestPage2(), *defManager, controller ) );
	std::string uniqueName2 = dataSrc->description() + std::string("testing_ui_main/test_reflected_tree_panel.qml");

	view = viewCreator->createView(
		"testing_ui_main/test_reflected_tree_panel.qml",
		std::move(model),
		[&] ( IView & view )
		{
            view.registerListener(this);
		},
		uniqueName2.c_str());
	test2Views_.emplace_back(TestViews::value_type(std::move( view.get() ), envIdx ) );
}

// =============================================================================
void TestUI::destroyActions()
{
	assert( app_ != nullptr );
	app_->removeAction( *testOpen_ );
	app_->removeAction( *testClose_ );
	testOpen_.reset();
	testClose_.reset();
}

// =============================================================================
void TestUI::destroyViews( size_t idx )
{
	assert( test1Views_.size() == test2Views_.size() );
	removeViews( idx );
    test1Models_.erase( test1Models_.begin() + idx );
	test1Views_.erase( test1Views_.begin() + idx );
	test2Views_.erase( test2Views_.begin() + idx );
}

// =============================================================================
void TestUI::closeAll()
{
	while (!dataSrcEnvPairs_.empty())
	{
		close();
	}

	assert( test1Views_.empty() );
	assert( test2Views_.empty() );
}

// =============================================================================
void TestUI::addActions( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testOpen_ );
	uiApplication.addAction( *testClose_ );
}

// =============================================================================
void TestUI::removeViews( size_t idx )
{
	assert( app_ != nullptr );
	if (test1Views_[idx].first != nullptr)
	{
		app_->removeView( *test1Views_[idx].first );
	}
	if (test2Views_[idx].first != nullptr)
	{
		app_->removeView( *test2Views_[idx].first );
	}
}

void TestUI::onFocusIn( IView* view )
{
	// NGT_MSG("%s focus in\n", view->title());

	auto pr = [&]( TestViews::value_type& x ) { return x.first.get() == view; };

	auto it1 = std::find_if( test1Views_.begin(), test1Views_.end(), pr );
	if ( it1 != test1Views_.end() )
	{
		get<IEnvManager>()->selectEnv( it1->second );
		return;
	}

	auto it2 = std::find_if( test2Views_.begin(), test2Views_.end(), pr );
	if ( it2 != test2Views_.end() )
	{
		get<IEnvManager>()->selectEnv( it2->second );
	}
}

void TestUI::onFocusOut( IView* view )
{
	// NGT_MSG("%s focus out\n", view->title());
}

void TestUI::open()
{
	assert(test1Views_.size() < 5);

	IDataSourceManager* dataSrcMngr = get<IDataSourceManager>();
	IDataSource* dataSrc = dataSrcMngr->openDataSource();

	IEnvManager* em = get<IEnvManager>();
	int envIdx = em->addEnv( dataSrc->description() );

	dataSrcEnvPairs_.push_back( DataSrcEnvPairs::value_type( dataSrc, envIdx ) );
	createViews( *fw_, dataSrc, envIdx );
}

void TestUI::close()
{
	assert( dataSrcEnvPairs_.size() > 0 );

	IDataSource* dataSrc = dataSrcEnvPairs_.back().first;
	int envIdx = dataSrcEnvPairs_.back().second;

	dataSrcEnvPairs_.pop_back();
	destroyViews( dataSrcEnvPairs_.size() );

	IEnvManager* em = get<IEnvManager>();
	em->removeEnv( envIdx );

	auto dataSrcMngr = get<IDataSourceManager>();
	dataSrcMngr->closeDataSource( dataSrc );
}

bool TestUI::canOpen() const
{
	assert(test1Views_.size() == test2Views_.size());
	return test1Views_.size() < 5;
}

bool TestUI::canClose() const
{
	assert(test1Views_.size() == test2Views_.size());
	return test1Views_.size() > 0;
}
} // end namespace wgt
