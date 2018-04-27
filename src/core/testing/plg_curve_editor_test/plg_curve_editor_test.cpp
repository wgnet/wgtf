#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "curve_editor/i_curve_editor.hpp"
#include "curve_editor/bezier_point_data.hpp"
#include "curve_editor/curve_types.hpp"
#include "curve_editor/i_curve.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
/**
* A plugin which queries the ICurveEditor interface and adds test data to be displayed and manipulated
*
* @ingroup plugins
* @image html plg_curve_editor_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - CurveEditorPlugin
*/
class CurveEditorTestPlugin : public PluginMain, public Depends<ICurveEditor>
{
public:
	void Initialise(IComponentContext& contextManager) override
	{
		auto curveEditor = get<ICurveEditor>();
		if (curveEditor == nullptr)
		{
			return;
		}
		curves_.emplace_back(curveEditor->addCurve(CurveTypes::Linear));
		{
			auto curve = curves_.back();
			BezierPointData pointData1 = { { 0.0f, 0.0f } };
			curve->add(pointData1);
			BezierPointData pointData2 = { { 0.25f, 0.25f } };
			curve->add(pointData2);
			BezierPointData pointData3 = { { 0.75f, 0.50f } };
			curve->add(pointData3);
			BezierPointData pointData4 = { { 1.0f, 1.0f } };
			curve->add(pointData4);
		}

		curves_.emplace_back(curveEditor->addCurve(CurveTypes::CubicBezier));
		auto curve = curves_.back();
		{
			BezierPointData pointData1 = { { 0.0f, 0.0f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
			curve->add(pointData1);
			BezierPointData pointData2 = { { 0.25f, 0.5f }, { -0.1f, 0.1f }, { 0.1f, -0.1f } };
			curve->add(pointData2);
			BezierPointData pointData3 = { { 0.75f, 0.50f }, { -0.1f, -0.2f }, { 0.1f, 0.2f } };
			curve->add(pointData3);
			BezierPointData pointData4 = { { 1.0f, 1.00f }, { -0.2f, 0.1f }, { 0.2f, -0.1f } };
			curve->add(pointData4);
		}
		curve->setShowControlPoints(true);

		curves_.emplace_back(curveEditor->addCurve(CurveTypes::Linear));
		{
			curve = curves_.back();
			BezierPointData pointData1 = { { 0.0f, 0.0f } };
			curve->add(pointData1);
			BezierPointData pointData2 = { { 0.25f, 0.1f } };
			curve->add(pointData2);
			BezierPointData pointData3 = { { 0.75f, 0.9f } };
			curve->add(pointData3);
			BezierPointData pointData4 = { { 1.0f, 0.1f } };
			curve->add(pointData4);
		}

		curves_.emplace_back(curveEditor->addCurve(CurveTypes::CubicBezier));
		{
			curve = curves_.back();
			BezierPointData pointData1 = { { 0.0f, 0.75f }, { 0.00f, 0.00f }, { 0.1f, 0.1f } };
			curve->add(pointData1);
			BezierPointData pointData2 = { { 0.25f, 0.25f }, { -0.1f, -0.1f }, { 0.0f, 0.0f } };
			curve->add(pointData2);
			BezierPointData pointData3 = { { 0.75f, 0.9f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
			curve->add(pointData3);
			BezierPointData pointData4 = { { 1.0f, 0.1f }, { -0.1f, -0.1f }, { 0.1f, 0.1f } };
			curve->add(pointData4);
		}
		curve->setShowControlPoints(true);
		curveEditor->setSubTitle("Test Curve Sub-Title");
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		auto curveEditor = get<ICurveEditor>();
		if (curveEditor == nullptr)
		{
			return true;
		}
		for (auto&& curve : curves_)
		{
			curveEditor->removeCurve(curve);
		}
		curves_.clear();
		return true;
	}

	std::vector<ICurveHandle> curves_;
};

PLG_CALLBACK_FUNC(CurveEditorTestPlugin)
} // end namespace wgt
