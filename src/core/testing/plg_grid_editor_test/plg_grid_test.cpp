#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "grid_editor_test_data.hpp"
#include <vector>


namespace wgt
{
/**
* A plugin which creates a grid of sample data that can be edited
*
* @ingroup plugins
* @image html plg_grid_editor_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class GridModelTestPlugin
		: public PluginMain
		, public Depends< IViewCreator >
{
private:
	std::vector<IInterface*> types_;
	wg_future<std::unique_ptr< IView >> gridView_;
	GridEditorTestModel model_;

public:
	//==========================================================================
	GridModelTestPlugin(IComponentContext & contextManager )
		: Depends( contextManager )
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		if (definitionManager == nullptr)
		{
			return false;
		}

		definitionManager->registerDefinition<TypeClassDefinition<GridEditorTestStruct>>();
		definitionManager->registerDefinition<TypeClassDefinition<GridEditorTestObject1>>();
		definitionManager->registerDefinition<TypeClassDefinition<GridEditorTestObject2>>();
		definitionManager->registerDefinition<TypeClassDefinition<GridEditorTestObject3>>();
		definitionManager->registerDefinition<TypeClassDefinition<GridEditorTestModel>>();

		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );

		model_.init(contextManager);

		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			gridView_ = viewCreator->createView(
			"plg_grid_editor_test/test_grid_editor_panel.qml",
			&model_);
		}
	}


	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		if (gridView_.valid())
		{
			auto view = gridView_.get();
			uiApplication->removeView( *view );
			view = nullptr;
		}
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


PLG_CALLBACK_FUNC( GridModelTestPlugin )
} // end namespace wgt
