#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "plugins/plg_curve_editor/interfaces/i_curve_editor.hpp"
#include "plugins/plg_curve_editor/models/bezier_point_data.hpp"
#include "plugins/plg_curve_editor/interfaces/curve_types.hpp"
#include "plugins/plg_curve_editor/interfaces/i_curve.hpp"


namespace wgt
{
//==============================================================================
class CurveEditorTestPlugin
	: public PluginMain
{
public:
	//==========================================================================
	CurveEditorTestPlugin( IComponentContext & contextManager ) {}

	void Initialise( IComponentContext & contextManager ) override
	{
		auto curveEditor = contextManager.queryInterface<ICurveEditor>();
        if(curveEditor == nullptr)
        {
            return;
        }
        auto curve = curveEditor->createCurve(CurveTypes::Linear, true);
        {
            BezierPointData pointData1 = { { 0.0f, 0.0f }, { -0.2f, 0.0f }, { 0.2f, 0.0f } };
            curve->add(pointData1);
            BezierPointData pointData2 = { { 0.25f, 0.25f }, { -0.2f, 0.00f }, { 0.2f, 0.0f } };
            curve->add(pointData2);
            BezierPointData pointData3 = { { 0.75f, 0.50f }, { -0.1f, -0.2f }, { 0.1f, 0.2f } };
            curve->add(pointData3);
            BezierPointData pointData4 = { { 1.0f, 1.0f }, { -0.2f, 0.0f }, { 0.2f, 0.0f } };
            curve->add(pointData4);
        }

        curve = curveEditor->createCurve(CurveTypes::Linear, true);
        {
            BezierPointData pointData1 = { { 0.0f, 0.0f }, { -0.1f, 0.0f }, { 0.1f, 0.1f } };
            curve->add(pointData1);
            BezierPointData pointData2 = { { 0.25f, 0.5f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
			curve->add(pointData2);
			BezierPointData pointData3 = { { 0.75f, 0.50f }, { -0.1f, -0.2f }, { 0.1f, 0.2f } };
			curve->add(pointData3);
			BezierPointData pointData4 = { { 1.0f, 1.00f }, { -0.2f, 0.00f }, { 0.2f, 0.0f } };
			curve->add(pointData4);
        }

		curve = curveEditor->createCurve(CurveTypes::Linear, true);
        {
            BezierPointData pointData1 = { { 0.0f, 0.0f }, { -0.1f, 0.0f }, { 0.1f, 0.1f } };
            curve->add(pointData1);
            BezierPointData pointData2 = { { 0.25f, 0.1f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
            curve->add(pointData2);
            BezierPointData pointData3 = { { 0.75f, 0.9f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
            curve->add(pointData3);
            BezierPointData pointData4 = { { 1.0f, 0.1f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
            curve->add(pointData4);
        }

		curve = curveEditor->createCurve(CurveTypes::Linear, true);
        {
            BezierPointData pointData1 = { { 0.0f, 0.75f }, { 0.00f, 0.00f }, { 0.1f, 0.1f } };
            curve->add(pointData1);
            BezierPointData pointData2 = { { 0.25f, 0.25f }, { -0.1f, -0.1f }, { 0.0f, 0.0f } };
			curve->add(pointData2);
			BezierPointData pointData3 = { { 0.75f, 0.9f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
			curve->add(pointData3);
			BezierPointData pointData4 = { { 1.0f, 0.1f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
			curve->add(pointData4);
        }
	}
};

PLG_CALLBACK_FUNC( CurveEditorTestPlugin )
} // end namespace wgt
