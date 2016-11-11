#include "pch.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "wg_types/binary_block.hpp"
#include "test_helpers.hpp"
#include "core_unit_test/unit_test.hpp"
#include "test_reflection_fixture.hpp"

// =============================================================================

// Figure out how to suppress exception stack traces for expected exceptions.
// TEST( non_collection_size_helper )
//{
//	ThrowTestExceptionOnAssert thrower;
//
//	typedef CollectionSizeHelper<int, false> NonCollectionSizeHelper;
//
//	CHECK_ASSERT(NonCollectionSizeHelper::getSize(NULL, NULL));
//	CHECK_ASSERT(NonCollectionSizeHelper::begin(NULL, NULL));
//	CHECK_ASSERT(NonCollectionSizeHelper::end(NULL, NULL));
//	CHECK_ASSERT(NonCollectionSizeHelper::findKey(NULL, NULL, NULL));
//	CHECK_ASSERT(NonCollectionSizeHelper::getContainedType(NULL, NULL));
//}

// =============================================================================

namespace wgt
{
class TestPropertyFixture : public TestReflectionFixture
{
public:
	class TestPropertyObject
	{
	public:
		bool boolean_;
		int integer_;
		unsigned int uinteger_;
		float floating_;
		std::string string_;
		std::wstring wstring_;
		const char* raw_string_;
		const wchar_t* raw_wstring_;
		std::shared_ptr<BinaryBlock> binary_data_;

		TestPropertyObject()
		    : boolean_(false), integer_(0), uinteger_(0U), floating_(0.0f), string_(), wstring_(), raw_string_(NULL),
		      raw_wstring_(NULL), binary_data_()
		{
		}
	};

	ReflectedProperty<bool, TestPropertyObject> booleanProperty_;
	ReflectedProperty<int, TestPropertyObject> integerProperty_;
	ReflectedProperty<unsigned int, TestPropertyObject> uintegerProperty_;
	ReflectedProperty<float, TestPropertyObject> floatProperty_;

	ReflectedProperty<std::string, TestPropertyObject> stringProperty_;
	ReflectedProperty<std::wstring, TestPropertyObject> wstringProperty_;
	ReflectedProperty<const char*, TestPropertyObject> rawStringProperty_;
	ReflectedProperty<const wchar_t*, TestPropertyObject> rawWStringProperty_;

	ReflectedProperty<std::shared_ptr<BinaryBlock>, TestPropertyObject> binaryDataProperty_;

public:
	TestPropertyFixture();
};

BEGIN_EXPOSE(TestPropertyFixture::TestPropertyObject, MetaNone())
END_EXPOSE()

TestPropertyFixture::TestPropertyFixture()
    : booleanProperty_("boolean", &TestPropertyObject::boolean_, TypeId::getType<bool>()),
      integerProperty_("integer", &TestPropertyObject::integer_, TypeId::getType<int32_t>()),
      uintegerProperty_("uinteger", &TestPropertyObject::uinteger_, TypeId::getType<uint32_t>()),
      floatProperty_("float", &TestPropertyObject::floating_, TypeId::getType<float>()),
      stringProperty_("string", &TestPropertyObject::string_, TypeId::getType<std::string>()),
      wstringProperty_("wstring", &TestPropertyObject::wstring_, TypeId::getType<std::wstring>()),
      rawStringProperty_("raw string", &TestPropertyObject::raw_string_, TypeId::getType<const char*>()),
      rawWStringProperty_("raw wstring", &TestPropertyObject::raw_wstring_, TypeId::getType<const wchar_t*>()),
      binaryDataProperty_("binary data", &TestPropertyObject::binary_data_,
                          TypeId::getType<std::shared_ptr<BinaryBlock>>())
{
	IDefinitionManager& definitionManager = getDefinitionManager();
	REGISTER_DEFINITION(TestPropertyObject);
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, boolean_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.boolean_ = false;

		bool value;
		Variant variant = booleanProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);

		CHECK_EQUAL(false, subject_.boolean_);
		CHECK_EQUAL(subject_.boolean_, value);
	}

	{
		bool value = true;
		booleanProperty_.set(provider, value, getDefinitionManager());

		CHECK_EQUAL(true, subject_.boolean_);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, integer_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.integer_ = -3567345;

		int value;
		Variant variant = integerProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);

		CHECK_EQUAL(-3567345, subject_.integer_);
		CHECK_EQUAL(subject_.integer_, value);
	}

	{
		int value = 5645654;
		integerProperty_.set(provider, value, getDefinitionManager());

		CHECK_EQUAL(5645654, subject_.integer_);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, unsigned_integer_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.uinteger_ = 1321491649u;

		unsigned int value;
		Variant variant = uintegerProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);
		CHECK_EQUAL(1321491649u, subject_.uinteger_);
		CHECK_EQUAL(subject_.uinteger_, value);
	}

	{
		unsigned int value = 564658465u;
		uintegerProperty_.set(provider, value, getDefinitionManager());

		CHECK_EQUAL(564658465u, subject_.uinteger_);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, float_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.floating_ = 367.345f;

		float value;
		Variant variant = floatProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);

		CHECK_EQUAL(367.345f, subject_.floating_);
		CHECK_EQUAL(subject_.floating_, value);
	}

	{
		float value = -321.587f;
		floatProperty_.set(provider, value, getDefinitionManager());

		CHECK_EQUAL(-321.587f, subject_.floating_);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, string_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.string_ = std::string("Hello World!");

		std::string value;
		Variant variant = stringProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);

		CHECK_EQUAL(subject_.string_, value);
	}

	{
		std::string value = "Delicious Cupcakes";
		stringProperty_.set(provider, value, getDefinitionManager());

		CHECK_EQUAL(value, subject_.string_);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, wstring_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.wstring_ = std::wstring(L"Chunky Bacon!");

		std::wstring value;
		Variant variant = wstringProperty_.get(provider, getDefinitionManager());

		variant.tryCast(value);
		// cppunitlite wants to serialise the expected and actual values
		// via a std::stringstream, hence wide character strings don't work.

		// CHECK_EQUAL(subject_.wstring_, value);
		CHECK(subject_.wstring_ == value);
	}

	{
		std::wstring value = L"Foxes driving pickups";
		wstringProperty_.set(provider, value, getDefinitionManager());

		// CHECK_EQUAL(value, subject_.wstring_);
		CHECK(value == subject_.wstring_);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, raw_string_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.raw_string_ = "Hello World!";

		std::string value;
		Variant variant = rawStringProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);

		CHECK(strcmp(subject_.raw_string_, value.c_str()) == 0);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, raw_wstring_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		subject_.raw_wstring_ = L"Hello World!";

		std::wstring value;
		Variant variant = rawWStringProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);
		CHECK(subject_.raw_wstring_ == value); // Only checks pointers are equal
		CHECK(wcscmp(subject_.raw_wstring_, value.c_str()) == 0);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestPropertyFixture, binary_data_property)
{
	TestPropertyObject subject_;
	ObjectHandle provider(&subject_, getDefinitionManager().getDefinition<TestPropertyObject>());

	{
		const char* randomData = "Something evil this way comes.";
		subject_.binary_data_ = std::make_shared<BinaryBlock>(randomData, strlen(randomData) + 1, false);

		std::shared_ptr<BinaryBlock> value;
		Variant variant = binaryDataProperty_.get(provider, getDefinitionManager());
		variant.tryCast(value);
		CHECK(subject_.binary_data_->compare(*value) == 0);
	}

	{
		const char* randomData = "Oh no, the boost library is here.";
		auto value = std::make_shared<BinaryBlock>(randomData, strlen(randomData) + 1, false);
		binaryDataProperty_.set(provider, value, getDefinitionManager());

		CHECK(value->compare(*(subject_.binary_data_)) == 0);
	}
}

// =============================================================================

class TestCollectionFixture
{
public:
	typedef std::vector<int> IntVector;
	typedef std::vector<float> FloatVector;
	typedef std::vector<std::string> StringVector;
	typedef std::vector<const char*> RawStringVector;
	typedef std::map<int, int> IntMap;
	typedef std::map<int, float> FloatMap;

	class TestCollectionObject
	{
	public:
		IntVector int_vector_;
		FloatVector float_vector_;
		StringVector string_vector_;
		RawStringVector raw_string_vector_;
		IntMap int_map_;
		FloatMap float_map_;

		TestCollectionObject()
		{
		}
	};

	ReflectedProperty<IntVector, TestCollectionObject> intVectorProperty_;
	ReflectedProperty<FloatVector, TestCollectionObject> floatVectorProperty_;
	ReflectedProperty<StringVector, TestCollectionObject> stringVectorProperty_;
	ReflectedProperty<RawStringVector, TestCollectionObject> rawStringVectorProperty_;
	ReflectedProperty<IntMap, TestCollectionObject> intMapProperty_;
	ReflectedProperty<FloatMap, TestCollectionObject> floatMapProperty_;

public:
	TestCollectionFixture();

	IDefinitionManager& getDefinitionManager()
	{
		return definitionManager_;
	}

private:
	ObjectManager objectManager_;
	DefinitionManager definitionManager_;
};

BEGIN_EXPOSE(TestCollectionFixture::TestCollectionObject, MetaNone())
EXPOSE("int vector", int_vector_)
END_EXPOSE()

TestCollectionFixture::TestCollectionFixture()
    : intVectorProperty_("int vector", &TestCollectionObject::int_vector_, TypeId::getType<IntVector>()),
      floatVectorProperty_("float vector", &TestCollectionObject::float_vector_, TypeId::getType<FloatVector>()),
      stringVectorProperty_("string vector", &TestCollectionObject::string_vector_, TypeId::getType<StringVector>()),
      rawStringVectorProperty_("raw string vector", &TestCollectionObject::raw_string_vector_,
                               TypeId::getType<RawStringVector>()),
      intMapProperty_("int map", &TestCollectionObject::int_map_, TypeId::getType<IntMap>()),
      floatMapProperty_("float map", &TestCollectionObject::float_map_, TypeId::getType<FloatMap>()),
      definitionManager_(objectManager_)
{
	objectManager_.init(&definitionManager_);
	IDefinitionManager& definitionManager = definitionManager_;
	Reflection::initReflectedTypes(definitionManager);
	REGISTER_DEFINITION(TestCollectionObject);
}

// -----------------------------------------------------------------------------
TEST_F(TestCollectionFixture, int_vector)
{
	TestCollectionObject subject;

	IntVector test1;
	test1.push_back(1);
	test1.push_back(2);
	test1.push_back(4);
	test1.push_back(8);
	test1.push_back(16);
	test1.push_back(32);

	IntVector test2;
	test2.push_back(-1);
	test2.push_back(-2);
	test2.push_back(-4);
	test2.push_back(-8);
	test2.push_back(-16);
	test2.push_back(-32);
	test2.push_back(-64);
	test2.push_back(-128);

	// Verify initial size & types
	CHECK_EQUAL(0, subject.int_vector_.size());

	ObjectHandle provider(&subject);

	Variant vIntVector = intVectorProperty_.get(provider, getDefinitionManager());
	Collection collection;
	vIntVector.tryCast(collection);
	CHECK_EQUAL(0, collection.size());

	// Verify initial iterator properties
	{
		CHECK(collection.begin() == collection.end());
		const size_t index = 0;
		CHECK(collection.end() == collection.find(index));
	}

	{
		subject.int_vector_ = test1;

		CHECK(Collection(subject.int_vector_) == collection);
		CHECK_EQUAL(test1.size(), collection.size());
	}

	// Verify iteration
	{
		IntVector result;
		for (auto iter = collection.begin(), end = collection.end(); iter != end; ++iter)
		{
			int value;
			iter.value().tryCast(value);
			result.push_back(value);
		}

		CHECK(test1 == result);
	}

	// Verify iterator access
	{
		size_t index = 3, test_index = 0;
		auto iter = collection.find(index);
		iter.key().tryCast(test_index);
		CHECK_EQUAL(index, test_index);

		int value = 0;
		iter.value().tryCast(value);
		CHECK_EQUAL(test1[3], value);

		index = 0;
		iter = collection.find(index);
		iter.key().tryCast(test_index);
		CHECK_EQUAL(index, test_index);

		iter.value().tryCast(value);
		CHECK_EQUAL(test1[0], value);

		index = 1000;
		iter = collection.find(index);
		CHECK(collection.end() == iter);
	}

	{
		IntVector value = test2;
		intVectorProperty_.set(provider, value, getDefinitionManager());

		CHECK(test2 == value);
		CHECK(test2 == subject.int_vector_);

		CHECK_EQUAL(test2.size(), collection.size());
	}

	auto definition = getDefinitionManager().getDefinition<TestCollectionObject>();
	CHECK(definition);

	{
		auto pa = definition->bindProperty("int vector[0]", provider);
		CHECK(pa.isValid());

		int i = 0;
		CHECK(pa.getValue().tryCast(i));

		CHECK_EQUAL(-1, i);

		// not Variant
		// not Variant::traits< int >::storage_type
		CHECK(pa.getType() == TypeId::getType<int>());
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestCollectionFixture, int_map)
{
	TestCollectionObject subject;

	IntMap test1;
	test1[1] = 0;
	test1[2] = 1;
	test1[4] = 2;
	test1[8] = 3;
	test1[16] = 4;
	test1[32] = 5;

	IntMap test2;
	test2[-1] = 99;
	test2[-2] = 98;
	test2[-4] = 96;
	test2[-8] = 92;
	test2[-16] = 84;
	test2[-32] = 68;
	test2[-64] = 34;
	test2[-128] = -28;

	// Verify initial size & types
	CHECK_EQUAL(0, subject.int_map_.size());

	ObjectHandle provider(&subject, getDefinitionManager().getDefinition<TestCollectionObject>());

	auto vCollection = intMapProperty_.get(provider, getDefinitionManager());
	Collection collection;
	vCollection.tryCast(collection);
	CHECK_EQUAL(0, collection.size());

	// Verify initial iterator properties
	{
		CHECK(collection.begin() == collection.end());

		const int index = 0;
		CHECK(collection.end() == collection.find(index));
	}

	{
		subject.int_map_ = test1;

		CHECK(Collection(subject.int_map_) == collection);
		CHECK_EQUAL(test1.size(), collection.size());
	}

	// Verify iteration
	{
		IntMap result;
		for (auto iter = collection.begin(), end = collection.end(); iter != end; ++iter)
		{
			int index, value;
			iter.key().tryCast(index);
			iter.value().tryCast(value);
			result[index] = value;
		}

		CHECK(test1 == result);
	}

	// Verify iterator access
	{
		int index = 32, test_index = 0;
		auto iter = collection.find(index);
		iter.key().tryCast(test_index);
		CHECK_EQUAL(index, test_index);

		int value = 0;
		iter.value().tryCast(value);
		CHECK_EQUAL(test1[32], value);

		index = 2;
		iter = collection.find(index);
		iter.key().tryCast(test_index);
		CHECK_EQUAL(index, test_index);

		iter.value().tryCast(value);
		CHECK_EQUAL(test1[2], value);

		index = 1000;
		iter = collection.find(index);
		CHECK(collection.end() == iter);
	}

	{
		IntMap value = test2;
		intMapProperty_.set(provider, value, getDefinitionManager());

		CHECK(test2 == value);
		CHECK(test2 == subject.int_map_);

		CHECK_EQUAL(test2.size(), collection.size());
	}
}
} // end namespace wgt
