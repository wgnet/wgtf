#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/default_meta_type_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_dependency_system/depends.hpp"

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

#include "active_filters_test_view_model.hpp"

namespace wgt
{
//------------------------------------------------------------------------------
// Plugin Class
// Spins up an instance of the plugin and creates the panel with the view
// model being passed is as source.
//------------------------------------------------------------------------------
class TestActiveFiltersPlugin
	: public PluginMain
	, public Depends< IViewCreator >
{
public:
	//==========================================================================
	TestActiveFiltersPlugin( IComponentContext & contextManager )
		: Depends( contextManager )
	{
	}

	//==========================================================================
	void Initialise(IComponentContext & contextManager) override
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		auto defManager = contextManager.queryInterface< IDefinitionManager >();
		if (defManager == nullptr)
		{
			return;
		}

		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		if (uiFramework == nullptr)
		{
			return;
		}

		defManager->registerDefinition< TypeClassDefinition< ActiveFiltersTestViewModel > >();

		auto testViewModel = defManager->create< ActiveFiltersTestViewModel >();
		testViewModel->init( *defManager, *uiFramework );

		auto viewCreator = get< IViewCreator >();
		if (viewCreator == nullptr)
		{
			return;
		}
		viewCreator->createView( 
			"TestActiveFilters/ActiveFiltersTestPanel.qml",
			testViewModel, testView_ );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return true;
		}

		if (testView_ != nullptr)
		{
			uiApplication->removeView( *testView_ );
			testView_ = nullptr;
		}
		return true;
	}

private:
	std::unique_ptr< IView > testView_;
};

PLG_CALLBACK_FUNC( TestActiveFiltersPlugin )
} // end namespace wgt
