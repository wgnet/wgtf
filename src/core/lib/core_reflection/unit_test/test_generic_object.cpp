#include "pch.hpp"

#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_variant/variant.hpp"

#include "test_helpers.hpp"
#include "test_objects.hpp"

namespace wgt
{
TEST_F(TestDefinitionFixture, createGenericObject)
{
	ManagedObject<GenericObject> object;
	CHECK(object == nullptr);
	object = GenericObject::create();
	CHECK(object != nullptr);

	auto handle = object.getHandleT();
	CHECK(handle != nullptr);

	auto genericObjHandle = object->handle();
	CHECK(genericObjHandle == handle);
	CHECK(std::is_reference<decltype(object->handle())>::value);

	auto genericObj = handle.get();
	CHECK(genericObj != nullptr);
	CHECK(genericObj->getDefinition());
	CHECK(genericObj->getDefinition() == getDefinitionManager().getDefinition(handle));

	int intValue = 1234;
	genericObj->set("intValue", intValue);
	{
		int value;
		CHECK(genericObj->get("intValue", value));
		CHECK_EQUAL(intValue, value);
	}

	int newIntValue = 5678;
	genericObj->set("intValue", newIntValue);
	{
		int value;
		CHECK(genericObj->get("intValue", value));
		CHECK_EQUAL(newIntValue, value);
	}

	Vector3 vector3Value(1, 2, 3);
	genericObj->set("vector3Value", vector3Value);
	{
		Vector3 value;
		CHECK(genericObj->get("vector3Value", value));
		CHECK(vector3Value == value);
	}

	std::string stringValue = "Hello World";
	genericObj->set("stringValue", stringValue);
	{
		std::string value;
		CHECK(genericObj->get("stringValue", value));
		CHECK_EQUAL(stringValue, value);
	}

	auto obj = GenericObject::create();
	auto objHandle = obj.getHandleT();
	std::vector<std::string> entries = {"A", "B", "C" };
	for(int i = 0; i < (int)entries.size(); ++i)
	{
		objHandle->set(std::to_string(i).c_str(), entries[i]);
	}
	genericObj->set("genericObjectValue", objHandle);
	{
		GenericObjectPtr value;
		CHECK(genericObj->get("genericObjectValue", value));
		CHECK(value != nullptr);

		for(size_t i = 0; i < entries.size(); ++i)
		{
			std::string entry;
			CHECK(value->get(std::to_string(i).c_str(), entry));
			CHECK_EQUAL(entry, entries[i]);
		}
	}

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set("testStructure", testStructure);
	{
		TestStructure2 value;
		CHECK(genericObj->get("testStructure", value));
		CHECK(testStructure == value);
	}

	auto testDefinitionObject = ManagedObject<TestDefinitionObject>::make();
	genericObj->set("testDefinitionObject", testDefinitionObject.getHandleT());
	{
		ObjectHandle provider;
		CHECK(genericObj->get("testDefinitionObject", provider));
		CHECK(testDefinitionObject.getPointer() == provider.getBase<TestDefinitionObject>());

        testDefinitionObject = nullptr;
        CHECK(genericObj->get("testDefinitionObject", provider));
        CHECK(provider == nullptr);
	}

    auto testDefinitionDerivedObject = ManagedObject<TestDefinitionDerivedObject>::make();
	genericObj->set("testDefinitionObject", testDefinitionDerivedObject.getHandleT());
	{
		ObjectHandle provider;
		CHECK(genericObj->get("testDefinitionObject", provider));
		CHECK(testDefinitionDerivedObject.getPointer() == provider.getBase<TestDefinitionDerivedObject>());

        testDefinitionDerivedObject = nullptr;
        CHECK(genericObj->get("testDefinitionObject", provider));
        CHECK(provider == nullptr);
	}

	object = nullptr;
	CHECK(object == nullptr);
	CHECK(handle == nullptr);
	CHECK(genericObjHandle == nullptr);
	CHECK(object.getHandleT() == nullptr);
}

TEST_F(TestDefinitionFixture, testGenericObjectSet)
{
	auto object1 = GenericObject::create();
	auto handle1 = object1.getHandleT();
	handle1->set("intValue", 1234);
	handle1->set("stringValue", "Hello World");

	auto object2 = GenericObject::create();
	auto handle2 = object2.getHandleT();
	object2->set("intValue", 5678);
    object2->set("boolValue", true);
	*object2 = handle1;

    bool value = false;
    CHECK(!object2->get("boolValue", value));

	int value1, value2;
	CHECK(object1->get("intValue", value1));
	CHECK(object2->get("intValue", value2));
	CHECK_EQUAL(value1, value2);

	std::string stringValue1, stringValue2;
	CHECK(object1->get("stringValue", stringValue1));
	CHECK(object2->get("stringValue", stringValue2));
	CHECK_EQUAL(stringValue1, stringValue2);

	object1 = nullptr;
	CHECK(object1 == nullptr);
	CHECK(handle1 == nullptr);
	CHECK(object2 != nullptr);
	CHECK(handle2 != nullptr);
}

TEST_F(TestDefinitionFixture, createMultipleGenericObjects)
{
	auto object = GenericObject::create();
	auto handle = object.getHandleT();
	auto genericObj = handle.get();
	CHECK(genericObj != nullptr);

	int intValue = 1234;
	genericObj->set("intValue", intValue);

	Vector3 vector3Value(1, 2, 3);
	genericObj->set("vector3Value", vector3Value);

	std::string stringValue = "Hello World";
	genericObj->set("stringValue", stringValue);

	auto newStorage = getDefinitionManager().getDefinition(handle)->createObjectStorage();
    auto newObject = ManagedObject<GenericObject>(newStorage);
    auto newHandle = newObject.getHandle();
	auto newGenericObj = newHandle.getBase<GenericObject>();

	{
		int value;
		CHECK(genericObj->get("intValue", value));
		CHECK_EQUAL(intValue, value);
	}
	{
		Vector3 value;
		CHECK(genericObj->get("vector3Value", value));
		CHECK(vector3Value == value);
	}
	{
		std::string value;
		CHECK(genericObj->get("stringValue", value));
		CHECK_EQUAL(stringValue, value);
	}

	{
		int value;
		CHECK(!newGenericObj->get("intValue", value));
	}
	{
		Vector3 value;
		CHECK(!newGenericObj->get("vector3Value", value));
	}
	{
		std::string value;
		CHECK(!newGenericObj->get("stringValue", value));
	}

	int newIntValue = 5678;
	newGenericObj->set("intValue", newIntValue);

	{
		int value;
		CHECK(genericObj->get("intValue", value));
		CHECK_EQUAL(intValue, value);
	}
	{
		int value;
		CHECK(newGenericObj->get("intValue", value));
		CHECK_EQUAL(newIntValue, value);
	}

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set("testStructure", testStructure);
	{
		TestStructure2 value;
		CHECK(genericObj->get("testStructure", value));
		CHECK(testStructure == value);
	}
	{
		TestStructure2 value;
		CHECK(!newGenericObj->get("testStructure", value));
	}
}
} // end namespace wgt
