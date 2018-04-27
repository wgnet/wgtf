#include "core_generic_plugin/generic_plugin.hpp"

#include "macros_object.hpp"
#include "metadata/macros_object.mpp"
#include "metadata/macro_display.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_logging/logging.hpp"
#include "core_ui_framework/i_component.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_dependency_system/depends.hpp"

WGT_INIT_QRC_RESOURCE

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
class MacrosUIPlugin : public PluginMain, public Depends<IViewCreator>
{
public:
	MacrosUIPlugin()
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		macros_ = ManagedObject<MacrosObject>::make_unique();

		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			panel_ = viewCreator->createView("WGMacros/WGMacroView.qml", macros_->getHandle());
		}
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		if (uiApplication != nullptr)
		{
			if (panel_.valid())
			{
				auto view = panel_.get();
				uiApplication->removeView(*view);
				view = nullptr;
			}
		}

		macros_.reset();

		return true;
	}

private:
	wg_future<std::unique_ptr<IView>> panel_;
	std::unique_ptr<ManagedObject<MacrosObject>> macros_;
};

PLG_CALLBACK_FUNC(MacrosUIPlugin)
} // end namespace wgt
