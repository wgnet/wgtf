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
#include "metadata/color_test_panel.mpp"
#include "metadata/color_tree_data.mpp"
#include <memory>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* This plugin creates a panel which allows testing of color features
*
* @ingroup plugins
* @image html plg_color_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ColorTestPlugin : public PluginMain
{
public:
	bool PostLoad(IComponentContext& context)
	{
		auto definitionManager = context.queryInterface<IDefinitionManager>();
		definitionManager->registerDefinition<TypeClassDefinition<ColorTestPanel>>();
		definitionManager->registerDefinition<TypeClassDefinition<ColorTreeData>>();
		return true;
	}

	void Initialise(IComponentContext& context)
	{
		panel_ = ManagedObject<ColorTestPanel>::make();
		auto viewCreator = context.queryInterface<IViewCreator>();
		view_ = viewCreator->createView("plg_color_test/ColorTestPanel.qml", panel_.getHandleT());
	}

	bool Finalise(IComponentContext& context)
	{
		auto uiApplication = context.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);

		if (view_.valid())
		{
			auto view = view_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}

        panel_ = nullptr;
		return true;
	}

	void Unload(IComponentContext& context)
	{
	}

private:
    ManagedObject<ColorTestPanel> panel_;
	wg_future<std::unique_ptr<IView>> view_;
};

PLG_CALLBACK_FUNC(ColorTestPlugin)
} // end namespace wgt
