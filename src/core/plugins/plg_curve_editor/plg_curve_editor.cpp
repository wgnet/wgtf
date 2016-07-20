#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/type_class_definition.hpp"
#include <QWidget>
#include <QQmlEngine>
#include <QQmlContext>

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "core_logging/logging.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"

#include "core_dependency_system/depends.hpp"

#include "core_reflection/reflection_macros.hpp"

#include "models/curve_editor.hpp"
#include "models/bezier_point.hpp"
#include "models/point.hpp"
#include "models/curve.hpp"
#include "metadata/i_curve_editor.mpp"

void initQtResources()
{
	Q_INIT_RESOURCE( plg_curve_editor );
}

void cleanupQtResources()
{
	Q_CLEANUP_RESOURCE( plg_curve_editor );
}

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
class CurveEditorPlugin
	: public PluginMain
	, public Depends< IViewCreator, ICurveEditor >
{
public:
	CurveEditorPlugin(IComponentContext & contextManager)
		: Depends( contextManager )
	{
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		initQtResources();

		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		assert(definitionManager != nullptr);
		if (definitionManager == nullptr)
			return false;

		// Setup the models for the view
		definitionManager->registerDefinition<TypeClassDefinition<Point>>();
		definitionManager->registerDefinition<TypeClassDefinition<BezierPoint>>();
		definitionManager->registerDefinition<TypeClassDefinition<ICurve>>();
		definitionManager->registerDefinition<TypeClassDefinition<ICurveEditor>>();

		contextManager.registerInterface( new CurveEditor() );

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		auto viewCreator = get< IViewCreator >();
		auto curveModel = get< ICurveEditor >();

		if (viewCreator != nullptr)
		{
			curvePanel_ = viewCreator->createView(
				"plg_curve_editor/CurveEditor.qml", curveModel);
		}
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if(uiApplication && (curvePanel_.valid()))
		{
            auto view = curvePanel_.get();
			uiApplication->removeView(*view);
            view = nullptr;
		}
		
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type : types_ )
		{
			contextManager.deregisterInterface(type);
		}

		cleanupQtResources();
	}

private:
	std::vector< IInterface * > types_;
	wg_future<std::unique_ptr< IView >> curvePanel_;
};

PLG_CALLBACK_FUNC(CurveEditorPlugin)
} // end namespace wgt
