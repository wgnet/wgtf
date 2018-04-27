#include "pch.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_unit_test/unit_test.hpp"
#include "core_object/managed_object.hpp"
#include "test_helpers.hpp"
#include "test_reflection_fixture.hpp"
#include "wg_types/binary_block.hpp"
#include "test_function_property.hpp"

#include <memory>

namespace wgt
{

typedef TestPropertyFixtureBase::TestPropertyObject::ExposedStruct TestExposedStruct;
typedef TestPropertyFixtureBase::TestPropertyObject::ExposedObject TestExposedObject;


TestPropertyFixtureBase* TestPropertyFixtureBase::s_instance = nullptr;

TestPropertyFixtureBase::TestPropertyFixtureBase()
{
	TF_ASSERT(s_instance == nullptr);
	s_instance = this;
}

#define IMPLEMENT_TEST_F(FIXTURE, TEST)     \
	TEST_F(FIXTURE, TEST)                   \
	{                                       \
		test_##TEST(this, m_name, result_); \
	}

// To add a new test create a new X entry - X( new_test )
// and define the test function -
// template <typename FIXTURE>
// void test_new_test( FIXTURE* fixture, const char * m_name, TestResult& result_ ) {}
#define TESTS                    \
	X(boolean_property)          \
	X(integer_property)          \
	X(unsigned_integer_property) \
	X(float_property)            \
	X(string_property)           \
	X(wstring_property)          \
	X(binary_data_property)      \
	X(exposed_struct_property)   \
	X(exposed_object_property)

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_boolean_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	{
		subject_.boolean_ = false;

		bool value;
		Variant variant = fixture->booleanProperty_->get(provider, fixture->getDefinitionManager());
		variant.tryCast(value);

		CHECK_EQUAL(false, subject_.boolean_);
		CHECK_EQUAL(subject_.boolean_, value);
	}

	{
		bool value = true;
		CHECK(fixture->template setProperty<bool>(fixture->booleanProperty_.get(), provider, value));
		CHECK_EQUAL(true, subject_.boolean_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_integer_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	{
		subject_.integer_ = -3567345;

		int value;
		Variant variant = fixture->integerProperty_->get(provider, fixture->getDefinitionManager());
		variant.tryCast(value);
		CHECK_EQUAL(-3567345, subject_.integer_);
		CHECK_EQUAL(subject_.integer_, value);
	}

	{
		int value = 5645654;
		CHECK(fixture->template setProperty<int>(fixture->integerProperty_.get(), provider, value));
		CHECK_EQUAL(5645654, subject_.integer_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_unsigned_integer_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	{
		subject_.uinteger_ = 1321491649u;

		unsigned int value;
		Variant variant = fixture->uintegerProperty_->get(provider, fixture->getDefinitionManager());
		variant.tryCast(value);

		CHECK_EQUAL(1321491649u, subject_.uinteger_);
		CHECK_EQUAL(subject_.uinteger_, value);
	}

	{
		unsigned int value = 564658465u;
		CHECK(fixture->template setProperty<unsigned int>(fixture->uintegerProperty_.get(), provider, value));
		CHECK_EQUAL(564658465u, subject_.uinteger_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_float_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	{
		subject_.floating_ = 367.345f;

		float value;
		Variant variant = fixture->floatProperty_->get(provider, fixture->getDefinitionManager());
		variant.tryCast(value);
		CHECK_EQUAL(367.345f, subject_.floating_);
		CHECK_EQUAL(subject_.floating_, value);
	}

	{
		float value = -321.587f;
		CHECK(fixture->template setProperty<float>(fixture->floatProperty_.get(), provider, value));
		CHECK_EQUAL(-321.587f, subject_.floating_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_string_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	{
		subject_.string_ = std::string("Hello World!");

		std::string value;
		Variant variant = fixture->stringProperty_->get(provider, fixture->getDefinitionManager());
		variant.tryCast(value);

		CHECK_EQUAL(subject_.string_, value);
	}

	{
		std::string value = "Delicious Cupcakes";
		CHECK(fixture->template setProperty<std::string>(fixture->stringProperty_.get(), provider, value));
		CHECK_EQUAL(value, subject_.string_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_wstring_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	{
		subject_.wstring_ = std::wstring(L"Chunky Bacon!");

		std::wstring value;
		Variant variant = fixture->wstringProperty_->get(provider, fixture->getDefinitionManager());
		variant.tryCast(value);

		// cppunitlite wants to serialise the expected and actual values
		// via a std::stringstream, hence wide character strings don't work.

		// CHECK_EQUAL(subject_.wstring_, value);
		CHECK(subject_.wstring_ == value);
	}

	{
		std::wstring value = L"Foxes driving pickups";
		CHECK(fixture->template setProperty<std::wstring>(fixture->wstringProperty_.get(), provider, value));
		// CHECK_EQUAL(value, subject_.wstring_);
		CHECK(value == subject_.wstring_);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_binary_data_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	{
		const char* randomData = "Something evil this way comes.";
		subject_.binary_data_ = std::make_shared<BinaryBlock>(randomData, strlen(randomData) + 1, false);

		std::shared_ptr<BinaryBlock> value;
		Variant variant = fixture->binaryDataProperty_->get(provider, fixture->getDefinitionManager());
		CHECK(variant.tryCast(value));
		CHECK(subject_.binary_data_->compare(*value) == 0);
	}

	{
		const char* randomData = "Oh no, the boost library is here.";
		auto value = std::make_shared<BinaryBlock>(randomData, strlen(randomData) + 1, false);
		CHECK(fixture->template setProperty<decltype(value)>(fixture->binaryDataProperty_.get(), provider, value));
		CHECK(value->compare(*(subject_.binary_data_)) == 0);
	}
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_exposed_struct_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typename FIXTURE::TestPropertyObject subject_;
	ManagedObject<typename FIXTURE::TestPropertyObject> object(&subject_);
	auto provider = object.getHandleT();

	Variant vStruct = fixture->exposedStructProperty_->get(provider, fixture->getDefinitionManager());

	auto testStruct = vStruct.value<const TestExposedStruct*>();
	RETURN_ON_FAIL_CHECK(testStruct != nullptr);
	CHECK_EQUAL(subject_.exposedStruct_.boolean_, testStruct->boolean_);

	TestExposedStruct value;
	value.boolean_ = !testStruct->boolean_;
	CHECK(fixture->template setProperty<TestExposedStruct>(fixture->exposedStructProperty_.get(), provider, value));
	CHECK_EQUAL(value.boolean_, subject_.exposedStruct_.boolean_);
}

// -----------------------------------------------------------------------------
template <typename FIXTURE>
void test_exposed_object_property(FIXTURE* fixture, const char* m_name, TestResult& result_)
{
	typedef TestExposedObject TestObject;
    auto testObject1 = ManagedObject<FIXTURE::TestPropertyObject>::make();

	{
        testObject1->exposedObject_->string_ = std::string("Hello World!");

		Variant vObject = fixture->exposedObjectProperty_->get(testObject1.getHandleT(), fixture->getDefinitionManager());

		ObjectHandle objectProvider;
        CHECK(vObject.tryCast(objectProvider));

		auto testObject = reflectedCast<TestObject>(objectProvider, fixture->getDefinitionManager());
		CHECK(testObject != nullptr);
		if (testObject == nullptr)
		{
			return;
		}
		CHECK_EQUAL(testObject1->exposedObject_->string_, testObject->string_);
	}

	{
		auto testObject2 = ManagedObject<TestObject>::make();
		testObject2->string_ = "Delicious Cupcakes";

		CHECK(fixture->template setProperty<ObjectHandleT<TestObject>>(fixture->exposedObjectProperty_.get(), 
                                                                       testObject1.getHandleT(),
		                                                               testObject2.getHandleT()));
		CHECK_EQUAL(testObject2->string_, testObject1->exposedObject_->string_);
	}
}

#define CREATE_PROPERTY(name, getterFunc, setterFunc) \
	FunctionPropertyHelper<TestPropertyObject>::getBaseProperty(name, getterFunc, setterFunc)

#define CREATE_PROPERTIES(TYPE)                                                                                     \
	booleanProperty_.reset(                                                                                         \
	CREATE_PROPERTY("boolean", &TestPropertyObject::getBoolean##TYPE, &TestPropertyObject::setBoolean));            \
	integerProperty_.reset(                                                                                         \
	CREATE_PROPERTY("integer", &TestPropertyObject::getInteger##TYPE, &TestPropertyObject::setInteger));            \
	uintegerProperty_.reset(                                                                                        \
	CREATE_PROPERTY("unsigned integer", &TestPropertyObject::getUInteger##TYPE, &TestPropertyObject::setUInteger)); \
	floatProperty_.reset(                                                                                           \
	CREATE_PROPERTY("float", &TestPropertyObject::getFloat##TYPE, &TestPropertyObject::setFloat));                  \
	stringProperty_.reset(                                                                                          \
	CREATE_PROPERTY("string", &TestPropertyObject::getString##TYPE, &TestPropertyObject::setString));               \
	wstringProperty_.reset(                                                                                         \
	CREATE_PROPERTY("wstring", &TestPropertyObject::getWString##TYPE, &TestPropertyObject::setWString));            \
	binaryDataProperty_.reset(                                                                                      \
	CREATE_PROPERTY("binary data", &TestPropertyObject::getBinaryData##TYPE, &TestPropertyObject::setBinaryData));  \
	exposedStructProperty_.reset(CREATE_PROPERTY("exposed struct", &TestPropertyObject::getExposedStruct##TYPE,     \
	                                             &TestPropertyObject::setExposedStruct));                           \
	exposedObjectProperty_.reset(CREATE_PROPERTY("exposed object", &TestPropertyObject::getExposedObject##TYPE,     \
	                                             &TestPropertyObject::setExposedObject));

// =============================================================================

class TestPropertyFixtureValue : public TestPropertyFixtureBase
{
public:
	TestPropertyFixtureValue()
	{
		CREATE_PROPERTIES(Value)
	}
};

// -----------------------------------------------------------------------------
#define X(TEST) IMPLEMENT_TEST_F(TestPropertyFixtureValue, TEST)
TESTS
#undef X

// =============================================================================

class TestPropertyFixtureRef : public TestPropertyFixtureBase
{
public:
	TestPropertyFixtureRef()
	{
		CREATE_PROPERTIES(Ref)
	}
};

// -----------------------------------------------------------------------------
#define X(TEST) IMPLEMENT_TEST_F(TestPropertyFixtureRef, TEST)
TESTS
#undef X

// =============================================================================

class TestPropertyFixtureArg : public TestPropertyFixtureBase
{
public:
	TestPropertyFixtureArg()
	{
		CREATE_PROPERTIES(Arg)
	}
};

// -----------------------------------------------------------------------------
#define X(TEST) IMPLEMENT_TEST_F(TestPropertyFixtureArg, TEST)
TESTS
#undef X
} // end namespace wgt
