#include "dialog_test_model.hpp"
#include "dialog_reflected_tree_model.hpp"
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
	DialogTestPlugin(IComponentContext& context) : Depends(context)
	{
	}

	bool PostLoad(IComponentContext& context)
	{
		auto definitionManager = context.queryInterface<IDefinitionManager>();
		assert(definitionManager != nullptr);

		definitionManager->registerDefinition<TypeClassDefinition<DialogTestModel>>();
		definitionManager->registerDefinition<TypeClassDefinition<DialogReflectedData>>();
		definitionManager->registerDefinition<TypeClassDefinition<DialogReflectedTreeModel>>();

		return true;
	}

	void Initialise(IComponentContext& context)
	{
		auto uiApplication = context.queryInterface<IUIApplication>();
		auto uiFramework = context.queryInterface<IUIFramework>();
		auto definitionManager = context.queryInterface<IDefinitionManager>();

		assert(uiApplication != nullptr);
		assert(uiFramework != nullptr);
		assert(definitionManager != nullptr);

		dialogModel_ = definitionManager->create<DialogTestModel>();
		assert(dialogModel_.get() != nullptr);
		auto classDefinition = dialogModel_.getDefinition(*definitionManager);
		dialogModel_->initialise(context, classDefinition);

		auto viewCreator = get<IViewCreator>();
		dialogPanel_ = viewCreator->createView("plg_dialog_test/dialog_test_panel.qml", dialogModel_);
	}

	bool Finalise(IComponentContext& context)
	{
		auto uiApplication = context.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);

		if (dialogPanel_.valid())
		{
			auto view = dialogPanel_.get();
			uiApplication->removeView(*view);
			dialogModel_->finalise();
			view = nullptr;
		}

		return true;
	}

	void Unload(IComponentContext& context)
	{
	}

private:
	ObjectHandleT<DialogTestModel> dialogModel_;
	wg_future<std::unique_ptr<IView>> dialogPanel_;
};

PLG_CALLBACK_FUNC(DialogTestPlugin)
} // end namespace wgt
