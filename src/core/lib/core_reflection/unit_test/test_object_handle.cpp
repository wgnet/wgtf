#include "pch.hpp"
#include "core_unit_test/unit_test.hpp"
#include "test_object_handle_fixture.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"

namespace wgt
{
TEST_F(TestObjectHandleFixture, test_object_handle_cast)
{
    IDefinitionManager& definitionManager = getDefinitionManager();

	auto object = ManagedObject<TestCast>::make();
	auto object2 = ManagedObject<TestCast>::make();
	auto handle = object.getHandle();
	auto handleT = object.getHandleT();
	auto handle2 = object2.getHandle();
	auto handleT2 = object2.getHandleT();

    CHECK(handle != nullptr);
    CHECK(handleT != nullptr);
    CHECK(handle == handleT);
    CHECK(handle == ObjectHandle(handleT));
    CHECK(handleT.get());
    CHECK(handle.getBase<TestCast>());
    CHECK(ObjectHandle(handleT).getBase<TestCast>());
    CHECK(handle.type() == TypeId::getType<TestCast>());
    CHECK(handleT.type() == TypeId::getType<TestCast>());

    CHECK(handle != handle2);
    CHECK(handle != handleT2);
    CHECK(handleT != handle2);
    CHECK(handleT != handleT2);
    CHECK(!handle.getBase<Test1>());
    CHECK(!ObjectHandle(handleT2).getBase<Test1>());
    CHECK(handle.type() == handle2.type());
    CHECK(handleT.type() == handleT2.type());
    CHECK(handle.type() != TypeId::getType<Test1>());
    CHECK(handleT.type() != TypeId::getType<Test1>());

	// Test supported handle casting
    ObjectHandleT<TestCast> downcasted = nullptr;
    downcast(&downcasted, handle);
    auto upcasted = upcast(handleT);
    auto safeCasted = safeCast<TestCast>(handle);
    auto reinterpretCasted = reinterpretCast<TestCast>(handle);
    auto reflectCasted = reflectedCast<TestCast>(handle, definitionManager);
    auto reflectCastedBase = reflectedCast<ITestCast>(handle, definitionManager);
    auto reflectRootCasted = reflectedRoot(reflectCastedBase, definitionManager);
    auto plainRootCasted = reflectedRoot(handleT, definitionManager);
	auto staticCastedBase = staticCast<ITestCast>(handleT);
	auto staticCasted = staticCast<TestCast>(staticCastedBase);
	auto staticCastedReflected = staticCast<TestCast>(reflectCastedBase);

    auto testEqualAllHandles = [&](const auto& h)
    {
        CHECK(h == handleT);
        CHECK(h == handle);
        CHECK(h != handle2);
        CHECK(h != handleT2);
        CHECK(h == upcasted);
		CHECK(h == staticCasted);
		CHECK(h == staticCastedBase);
		CHECK(h == staticCastedReflected);
        CHECK(h == safeCasted);
        CHECK(h == downcasted);
        CHECK(h == reinterpretCasted);
        CHECK(h == reflectCastedBase);
        CHECK(h == reflectCasted);
        CHECK(h == reflectRootCasted);
        CHECK(h == plainRootCasted);

        const auto hash = h.getRecursiveHash();
        CHECK(hash == handleT.getRecursiveHash());
        CHECK(hash == handle.getRecursiveHash());
        CHECK(hash != handle2.getRecursiveHash());
        CHECK(hash != handleT2.getRecursiveHash());
		CHECK(hash == staticCasted.getRecursiveHash());
		CHECK(hash == staticCastedBase.getRecursiveHash());
		CHECK(hash == staticCastedReflected.getRecursiveHash());
        CHECK(hash == upcasted.getRecursiveHash());
        CHECK(hash == safeCasted.getRecursiveHash());
        CHECK(hash == downcasted.getRecursiveHash());
        CHECK(hash == reinterpretCasted.getRecursiveHash());
        CHECK(hash == reflectCastedBase.getRecursiveHash());
        CHECK(hash == reflectCasted.getRecursiveHash());
        CHECK(hash == reflectRootCasted.getRecursiveHash());
        CHECK(hash == plainRootCasted.getRecursiveHash());
    };

    CHECK(upcasted.type() == TypeId::getType<TestCast>());
    testEqualAllHandles(upcasted);
    CHECK(upcasted.getBase<TestCast>());
    CHECK(!upcasted.getBase<Test1>());

    CHECK(downcasted.type() == TypeId::getType<TestCast>());
    testEqualAllHandles(downcasted);
    CHECK(downcasted.get());
    CHECK(ObjectHandle(downcasted).getBase<TestCast>());
    CHECK(!ObjectHandle(downcasted).getBase<Test1>());

	CHECK(staticCastedBase.type() == TypeId::getType<ITestCast>());
	testEqualAllHandles(staticCastedBase);
	CHECK(staticCastedBase.get());
	CHECK(ObjectHandle(staticCastedBase).getBase<ITestCast>());
	CHECK(!ObjectHandle(staticCastedBase).getBase<Test1>());

	CHECK(staticCastedReflected.type() == TypeId::getType<TestCast>());
	testEqualAllHandles(staticCastedReflected);
	CHECK(staticCastedReflected.get());
	CHECK(ObjectHandle(staticCastedReflected).getBase<TestCast>());
	CHECK(!ObjectHandle(staticCastedReflected).getBase<Test1>());

	CHECK(staticCasted.type() == TypeId::getType<TestCast>());
	testEqualAllHandles(staticCasted);
	CHECK(staticCasted.get());
	CHECK(ObjectHandle(staticCasted).getBase<TestCast>());
	CHECK(!ObjectHandle(staticCasted).getBase<Test1>());

    CHECK(safeCasted.type() == TypeId::getType<TestCast>());
    testEqualAllHandles(safeCasted);
    CHECK(safeCasted.get());
    CHECK(ObjectHandle(safeCasted).getBase<TestCast>());
    CHECK(!ObjectHandle(safeCasted).getBase<Test1>());

    CHECK(reinterpretCasted.type() == TypeId::getType<TestCast>());
    testEqualAllHandles(reinterpretCasted);
    CHECK(reinterpretCasted.get());
    CHECK(ObjectHandle(reinterpretCasted).getBase<TestCast>());
    CHECK(!ObjectHandle(reinterpretCasted).getBase<Test1>());

    CHECK(reflectCasted.type() == TypeId::getType<TestCast>());
    testEqualAllHandles(reflectCasted);
    CHECK(reflectCasted.get());
    CHECK(ObjectHandle(reflectCasted).getBase<TestCast>());
    CHECK(!ObjectHandle(reflectCasted).getBase<Test1>());

    CHECK(reflectCastedBase.type() == TypeId::getType<ITestCast>());
    testEqualAllHandles(reflectCastedBase);
    CHECK(reflectCastedBase.get());
    CHECK(ObjectHandle(reflectCastedBase).getBase<TestCast>());
    CHECK(ObjectHandle(reflectCastedBase).getBase<ITestCast>());
    CHECK(!ObjectHandle(reflectCastedBase).getBase<Test1>());

    CHECK(reflectRootCasted.type() == TypeId::getType<TestCast>());
    testEqualAllHandles(reflectRootCasted);
    CHECK(reflectRootCasted.getBase<TestCast>());
    CHECK(!reflectRootCasted.getBase<Test1>());

    CHECK(plainRootCasted.type() == TypeId::getType<TestCast>());
    testEqualAllHandles(plainRootCasted);
    CHECK(plainRootCasted.getBase<TestCast>());
    CHECK(!plainRootCasted.getBase<Test1>());

	auto reflectCastedPtr = reflectedCast<TestCast>(handle.data(), handle.type(), definitionManager);
	CHECK(reflectCastedPtr == handle.data());
	CHECK(reflectCastedPtr == handleT.get());

	auto reflectCastedBasePtr = reflectedCast<ITestCast>(handle.data(), handle.type(), definitionManager);
	CHECK(reflectCastedBasePtr == handle.data());
	CHECK(reflectCastedBasePtr == handleT.get());

	// Test unsupported handle casting
	ObjectHandleT<ITestCast> downcastedBase = nullptr;
	downcast(&downcastedBase, handle);
	CHECK(downcastedBase == nullptr);

	CHECK(safeCast<Test1>(handle) == nullptr);
	CHECK(safeCast<ITestCast>(handle) == nullptr);
	CHECK(reflectedCast<Test1>(handle, definitionManager) == nullptr);
	CHECK(reflectedCast<Test1>(handle.data(), handle.type(), definitionManager) == nullptr);

	// Test auto handle nulling when object is destroyed
	object = nullptr;
    CHECK(handle == nullptr);
    CHECK(handleT == nullptr);
    CHECK(downcasted == nullptr);
	CHECK(staticCasted == nullptr);
	CHECK(staticCastedBase == nullptr);
	CHECK(staticCastedReflected == nullptr);
    CHECK(upcasted == nullptr);
    CHECK(safeCasted == nullptr);
    CHECK(reflectCasted == nullptr);
    CHECK(reflectCastedBase == nullptr);
    CHECK(reflectRootCasted == nullptr);
    CHECK(plainRootCasted == nullptr);
    CHECK(reinterpretCasted == nullptr);
}

TEST_F(TestObjectHandleFixture, test_on_stack_object)
{
    IDefinitionManager& definitionManager = getDefinitionManager();

	IClassDefinition* def1 = definitionManager.getDefinition< Test1Stack>();
	IClassDefinition* def2 = definitionManager.getDefinition< Test2Stack>();

	std::unique_ptr<GListTest> glist(new GListTest(&definitionManager));
	glist->addItem(ManagedObject<Test1Stack>::make_iunique(5));
	glist->addItem(ManagedObject<Test2Stack>::make_iunique(58));
	glist->addItem(ManagedObject<Test1Stack>::make_iunique(7));

	reflectionController.setValue(glist->bindProperty(0u, def1, "Value"), Variant(13));
	reflectionController.setValue(glist->bindProperty(1u, def2, "Test1.Value"), Variant(17));
	reflectionController.setValue(glist->bindProperty(2u, def1, "Value"), Variant(19));

	// TODO: replace this call with reflectionController.flush
	reflectionController.getValue(glist->bindProperty(2u, def1, "Value"));

	CHECK_EQUAL(glist->bindProperty(0u, def1, "Value").getValue(), 13);
	CHECK_EQUAL(glist->bindProperty(1u, def2, "Test1.Value").getValue(), 17);
	CHECK_EQUAL(glist->bindProperty(2u, def1, "Value").getValue(), 19);
}

TEST_F(TestObjectHandleFixture, test_object_reuse)
{
	auto object = ManagedObject<TestCast>::make();
	ObjectHandle handle = object.getHandle();
	RefObjectId id = handle.id();
	CHECK(handle.isValid());
	object = nullptr;
	CHECK(!handle.isValid());
	object = ManagedObject<TestCast>::make_id(id);
	CHECK(handle.isValid());
}
} // end namespace wgt
