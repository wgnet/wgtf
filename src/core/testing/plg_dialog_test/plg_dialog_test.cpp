#include "dialog_test_panel.hpp"
#include "metadata/dialog_reflected_data.mpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_dependency_system/depends.hpp"

#include <memory>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* This plugin creates a panel which allows testing of custom QML dialogs
*
* @ingroup plugins
* @image html plg_dialog_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class DialogTestPlugin : public PluginMain, public Depends<IViewCreator>
{
public:
	bool PostLoad(IComponentContext& context)
	{
		auto definitionManager = context.queryInterface<IDefinitionManager>();
		assert(definitionManager != nullptr);

		definitionManager->registerDefinition<TypeClassDefinition<DialogTestPanel>>();
		definitionManager->registerDefinition<TypeClassDefinition<DialogReflectedData>>();

		return true;
	}

	void Initialise(IComponentContext& context)
	{
		auto definitionManager = context.queryInterface<IDefinitionManager>();
		assert(definitionManager != nullptr);

		dialogPanel_ = ManagedObject<DialogTestPanel>::make();
		dialogPanel_->initialise();
		dialogView_ = get<IViewCreator>()->createView("plg_dialog_test/dialog_test_panel.qml", dialogPanel_.getHandleT());
	}

	bool Finalise(IComponentContext& context)
	{
		auto uiApplication = context.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);

		if (dialogView_.valid())
		{
			auto view = dialogView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}

		dialogPanel_ = nullptr;
		return true;
	}

	void Unload(IComponentContext& context)
	{
	}

private:
	ManagedObject<DialogTestPanel> dialogPanel_ = nullptr;
	wg_future<std::unique_ptr<IView>> dialogView_;
};

PLG_CALLBACK_FUNC(DialogTestPlugin)
} // end namespace wgt
