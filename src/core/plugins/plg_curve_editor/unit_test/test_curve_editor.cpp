#include "CppUnitLite2/src/CppUnitLite2.h"
#include "core_unit_test/unit_test.hpp"
#include "core_unit_test/test_framework.hpp"
#include "core_object/managed_object.hpp"

#include "core_reflection_utils/reflection_auto_reg.mpp"
#include "../reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

namespace wgt
{
class TestCurveEditor : public CurveEditor
{
public:
    const Collection& curves() const
    {
        return dynamic_cast<const CollectionModel*>(CurveEditor::getCurves())->getSource();
    }
};

BEGIN_EXPOSE(TestCurveEditor, MetaDirectBases( CurveEditor))
END_EXPOSE()

ManagedObject<TestCurveEditor> createCurveEditor(TestFramework& framework)
{
    auto& definitionManager = framework.getDefinitionManager();
	ReflectionAutoRegistration::initAutoRegistration(definitionManager);
    return ManagedObject<TestCurveEditor>::make();
}

TEST(testCurveEditor)
{
    TestFramework framework;
    ManagedObject<TestCurveEditor> curveEditor = createCurveEditor(framework);
    CHECK(curveEditor->curves().size() == 0);

    // Test adding a curve owned by the curve editor
    auto ownedCurve = curveEditor->createCurve();
    auto ownedHandle = ownedCurve->getHandleT<ICurve>();
    CHECK(curveEditor->addCurve(std::move(ownedCurve)) != nullptr);
    CHECK(curveEditor->curves().size() == 1);
    CHECK(curveEditor->addCurve() != nullptr);
    CHECK(curveEditor->curves().size() == 2);
    CHECK(ownedHandle != nullptr);

    // Test adding an externally owned curve
    auto notOwnedCurve = curveEditor->createCurve();
    auto notOwnedHandle = notOwnedCurve->getHandleT<ICurve>();
    CHECK(curveEditor->addCurve(notOwnedHandle));
    CHECK(curveEditor->curves().size() == 3);

    // Test removing a curve owned the curve editor
    CHECK(curveEditor->removeCurve(ownedHandle));
    CHECK(curveEditor->curves().size() == 2);
    CHECK(ownedHandle == nullptr);

    // Test removing an externally owned curve
    CHECK(curveEditor->removeCurve(notOwnedHandle));
    CHECK(curveEditor->curves().size() == 1);
    CHECK(notOwnedHandle != nullptr);
    notOwnedCurve = nullptr;
    CHECK(notOwnedHandle == nullptr);

    // Testing clearing curves
    curveEditor->clear();
    CHECK(curveEditor->curves().size() == 0);
}

TEST(testCurve)
{
    TestFramework framework;
    ManagedObject<TestCurveEditor> curveEditor = createCurveEditor(framework);

    auto areSame = [&](BezierPointData& a, BezierPointData& b)
    {
        return a.pos.x == b.pos.x &&
               a.pos.y == b.pos.y &&
               a.cp1.x == b.cp1.x &&
               a.cp1.y == b.cp1.y &&
               a.cp2.x == b.cp2.x &&
               a.cp2.y == b.cp2.y;
    };
    
    auto handle = curveEditor->addCurve(CurveTypes::CubicBezier);
    CHECK(handle != nullptr);

    // Test adding new points
    BezierPointData data0 = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
    handle->add(data0, false);
    CHECK(handle->getNumPoints() == 1);
    CHECK(areSame(handle->at(0U), data0));

    BezierPointData data1 = { 7.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    handle->addAt(data1.pos.x, false);
    CHECK(handle->getNumPoints() == 2);
    CHECK(areSame(handle->at(0U), data0));
    CHECK(areSame(handle->at(1U), data1));

    // Test modifying an existing point
    data1 = { 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f };
    handle->modify(1U, data1);
    CHECK(handle->getNumPoints() == 2);
    CHECK(areSame(handle->at(0U), data0));
    CHECK(areSame(handle->at(1U), data1));

    // Test undo/redo for removing a point
    handle->removeAt(7.0f, false);
    CHECK(handle->getNumPoints() == 1);
    CHECK(areSame(handle->at(0U), data0));

    handle->undo(nullptr, nullptr);
    CHECK(handle->getNumPoints() == 2);
    CHECK(areSame(handle->at(0U), data0));
    CHECK(areSame(handle->at(1U), data1));

    handle->redo(nullptr, nullptr);
    CHECK(handle->getNumPoints() == 1);
    CHECK(areSame(handle->at(0U), data0));

    // Test undo/redo for adding a point
    handle->add(data1, false);
    CHECK(handle->getNumPoints() == 2);
    CHECK(areSame(handle->at(0U), data0));
    CHECK(areSame(handle->at(1U), data1));

    handle->undo(nullptr, nullptr);
    CHECK(handle->getNumPoints() == 1);
    CHECK(areSame(handle->at(0U), data0));

    handle->redo(nullptr, nullptr);
    CHECK(handle->getNumPoints() == 2);
    CHECK(areSame(handle->at(0U), data0));
    CHECK(areSame(handle->at(1U), data1));
}

} // end namespace wgt
