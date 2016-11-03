#include "core_generic_plugin/generic_plugin.hpp"

#include "core_data_model/i_list_model.hpp"
#include "macros_object.hpp"
#include "metadata/macros_object.mpp"

#include "core_command_system/i_command_manager.hpp"

#include "core_logging/logging.hpp"

#include "core_qt_common/i_qt_framework.hpp"

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_dependency_system/depends.hpp"

namespace wgt
{
/**
* A plugin that creates a panel that allow actions to be recorded and played back. 
*
* @ingroup plugins
* @image html plg_macros_ui.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class MacrosUIPlugin
	: public PluginMain
	, public Depends< IViewCreator >
{
public:
	MacrosUIPlugin( IComponentContext& contextManager )
		: Depends( contextManager )
	{
	}

	bool PostLoad( IComponentContext& contextManager ) override
	{
		return true;
	}

	void Initialise( IComponentContext& contextManager ) override
	{
		auto qtFramework = Context::queryInterface< IQtFramework >();
		if (qtFramework == nullptr)
		{
			return;
		}

		auto pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return;
		}
		auto& definitionManager = *pDefinitionManager;
		REGISTER_DEFINITION( MacrosObject )

		ICommandManager* pCommandSystemProvider =
			Context::queryInterface< ICommandManager >();
		if (pCommandSystemProvider == nullptr)
		{
			return;
		}

		auto pMacroDefinition = pDefinitionManager->getDefinition(
			getClassIdentifier< MacrosObject >() );
		assert( pMacroDefinition != nullptr );
		macros_ = pMacroDefinition->create();
		macros_.getBase< MacrosObject >()->init( *pCommandSystemProvider );

		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			panel_ = viewCreator->createView(
				"WGMacros/WGMacroView.qml",
				macros_ );
		}
	}

	bool Finalise( IComponentContext& contextManager ) override
	{
		auto uiApplication = Context::queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return true;
		}
		if (panel_.valid())
		{
            auto view = panel_.get();
			uiApplication->removeView( *view );
            view = nullptr;
		}

		return true;
	}

private:
	wg_future<std::unique_ptr< IView >> panel_;
	ObjectHandle macros_;
};

PLG_CALLBACK_FUNC( MacrosUIPlugin )
} // end namespace wgt
