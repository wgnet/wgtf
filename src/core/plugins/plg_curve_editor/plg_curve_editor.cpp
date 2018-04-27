#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "core_logging/logging.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_dependency_system/depends.hpp"

#include "core_reflection/reflection_macros.hpp"

#include "models/curve_editor.hpp"
#include "models/bezier_point.hpp"
#include "models/point.hpp"
#include "models/curve.hpp"

#include "reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which registers an ICurveEditor interface on a panel that allows curves to be displayed and manipulated
*
* @ingroup plugins
* @image html plg_curve_editor.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class CurveEditorPlugin : public PluginMain, public Depends<IViewCreator, ICurveEditor>
{
public:
	CurveEditorPlugin( IComponentContext & contextManager )
	{
		registerCallback([this, &contextManager ]( IDefinitionManager & defManager )
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
			curveEditor_ = ManagedObject<CurveEditor>::make();
			types_.push_back(contextManager.registerInterface(curveEditor_.getPointer(), false));
		});
	}

	void Initialise(IComponentContext& contextManager) override
	{
		curveEditor_->init(curveEditor_.getHandleT());
	}

	bool Finalise(IComponentContext& contextManager) override
	{
        curveEditor_->fini();
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
        curveEditor_ = nullptr;
	}

private:
    ManagedObject<CurveEditor> curveEditor_;
	InterfacePtrs types_;
};

PLG_CALLBACK_FUNC(CurveEditorPlugin)
} // end namespace wgt
