#include "pch.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_object/managed_object.hpp"
#include "core_unit_test/test_framework.hpp"

#include "test_helpers.hpp"
#include "test_objects.hpp"

#include "core_variant/collection.hpp"
#include <numeric>

#include "test_class_definition.hpp"

namespace wgt
{
std::ostream& operator<<(std::ostream& output, const TypeId& propType)
{
	output << propType.getName();
	return output;
}

TEST_F(TestDefinitionFixture, create)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	CHECK_EQUAL(getClassIdentifier<TestDefinitionObject>(), klass_->getName());
	CHECK(klass_->getParentNames().empty());
	CHECK(klass_->getMetaData() != nullptr);
}

TEST_F(TestDefinitionFixture, objectScope)
{
	ObjectHandle handle;

	{
		TestDefinitionObject data;
		ManagedObject<TestDefinitionObject> object(data);
		handle = object.getHandle();
		CHECK(handle.getBase<TestDefinitionObject>() != nullptr);
	}

	CHECK(handle.getBase<TestDefinitionObject>() == nullptr);
}

TEST_F(TestDefinitionFixture, properties)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();

	// counter
	PropertyIterator pi = klass_->allProperties().begin();
	IBasePropertyPtr property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// Function Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional counter"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// Function Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional text"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// Function Int
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional ints"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::vector<int32_t>>(), property->getType());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// Function Raw String
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional string"), property->getName());
	CHECK_EQUAL(TypeId::getType<const char*>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// Getter only Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter getter"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// Getter only Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text getter"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// link
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("link"), property->getName());
	CHECK_EQUAL(TypeId::getType<ObjectHandleT<TestPolyStruct2>>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// raw string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw string"), property->getName());
	CHECK_EQUAL(TypeId::getType<const char*>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("string"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// strings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("strings"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::vector<std::string>>(), property->getType());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// raw wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType<const wchar_t*>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::wstring>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// wstrings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstrings"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// exposed structure
	++pi;
	property = *pi;
	CHECK(property != NULL);
	const PropertyAccessor& paStruct = klass_->bindProperty(property->getName(), handle);
	CHECK(paStruct.isValid());
	CHECK_EQUAL(std::string("exposed structure"), property->getName());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// exposed structures
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed structures"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// exposed object
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed object"), property->getName());
	CHECK_EQUAL(TypeId::getType<ObjectHandleT<TestPolyStruct2>>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// exposed objects
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed objects"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// boolean
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("boolean"), property->getName());
	CHECK_EQUAL(TypeId::getType<bool>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// booleans
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("booleans"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// uint32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32"), property->getName());
	CHECK_EQUAL(TypeId::getType<uint32_t>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// uint32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32s"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// int32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// int32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32s"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// uint64
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64"), property->getName());
	CHECK_EQUAL(TypeId::getType<uint64_t>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// uint64s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64s"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("float"), property->getName());
	CHECK_EQUAL(TypeId::getType<float>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// floats
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("floats"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// vector3
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3"), property->getName());
	CHECK_EQUAL(TypeId::getType<Vector3>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// vector3s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3s"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// vector4
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4"), property->getName());
	CHECK_EQUAL(TypeId::getType<Vector4>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// vector4s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4s"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// binary
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binary"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::shared_ptr<BinaryBlock>>(), property->getType());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// binaries
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binaries"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// multidimensional
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("multidimensional"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() == nullptr);

	// string lambda
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("string lambda"), property->getName());
	CHECK(!property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// strings lambda
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("strings lambda"), property->getName());
	CHECK(property->isCollection());
	CHECK(property->getMetaData() != nullptr);

	// Finished
	++pi;
	property = *pi;
	CHECK(property == NULL);
}

TEST_F(TestDefinitionFixture, derived_properties)
{
	const auto& derived_klass = *getDefinitionManager().getDefinition<TestDefinitionDerivedObject>();
	TestDefinitionDerivedObject data;
	ManagedObject<TestDefinitionDerivedObject> object(data);
	auto derived = reflectedCast<TestDefinitionDerivedObject>(object.getHandle(), getDefinitionManager());
	CHECK(derived != nullptr);
	auto base = reflectedCast<TestDefinitionObject>(object.getHandle(), getDefinitionManager());
	CHECK(base != nullptr);
}

TEST_F(TestDefinitionFixture, property_iterator_self_only)
{
	const auto& derived_klass = *getDefinitionManager().getDefinition<TestDefinitionDerivedObject>();
	PropertyIterator pi = derived_klass.directProperties().begin();

	// some integer
	IBasePropertyPtr property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some integer"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// some float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some float"), property->getName());
	CHECK_EQUAL(TypeId::getType<float>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// Finished
	++pi;
	property = *pi;
	CHECK(property == NULL);
}

TEST_F(TestDefinitionFixture, property_iterator_parents)
{
	ManagedObject<TestDefinitionDerivedObject> object(std::make_unique<TestDefinitionDerivedObject>());
	auto handle = object.getHandle();
	const auto& derived_klass = *getDefinitionManager().getDefinition<TestDefinitionDerivedObject>();

	PropertyIterator pi = derived_klass.allProperties().begin();
	// some integer
	IBasePropertyPtr property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some integer"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// some float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some float"), property->getName());
	CHECK_EQUAL(TypeId::getType<float>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// Function Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional counter"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(property->getMetaData() != nullptr);

	// Function Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional text"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(property->getMetaData() != nullptr);

	// Function Int
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional ints"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::vector<int32_t>>(), property->getType());
	CHECK(property->getMetaData() != nullptr);

	// Function Raw String
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional string"), property->getName());
	CHECK_EQUAL(TypeId::getType<const char*>(), property->getType());
	CHECK(property->getMetaData() != nullptr);

	// Getter only Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter getter"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// Getter only Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text getter"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// link
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("link"), property->getName());
	CHECK_EQUAL(TypeId::getType<ObjectHandleT<TestPolyStruct2>>(), property->getType());
	CHECK(property->getMetaData() != nullptr);

	// raw string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw string"), property->getName());
	CHECK_EQUAL(TypeId::getType<const char*>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("string"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::string>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// strings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("strings"), property->getName());
	CHECK(property->getMetaData() != nullptr);

	// raw wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType<const wchar_t*>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::wstring>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// wstrings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstrings"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// exposed structure
	++pi;
	property = *pi;
	CHECK(property != NULL);
	const PropertyAccessor& paStruct = derived_klass.bindProperty(property->getName(), handle);
	CHECK(paStruct.isValid());
	CHECK_EQUAL(std::string("exposed structure"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// exposed structures
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed structures"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// exposed object
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed object"), property->getName());
	CHECK_EQUAL(TypeId::getType<ObjectHandleT<TestPolyStruct2>>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// exposed objects
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed objects"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// boolean
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("boolean"), property->getName());
	CHECK_EQUAL(TypeId::getType<bool>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// booleans
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("booleans"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// uint32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32"), property->getName());
	CHECK_EQUAL(TypeId::getType<uint32_t>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// uint32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32s"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// int32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32"), property->getName());
	CHECK_EQUAL(TypeId::getType<int32_t>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// int32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32s"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// uint64
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64"), property->getName());
	CHECK_EQUAL(TypeId::getType<uint64_t>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// uint64s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64s"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("float"), property->getName());
	CHECK_EQUAL(TypeId::getType<float>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// floats
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("floats"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// vector3
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3"), property->getName());
	CHECK_EQUAL(TypeId::getType<Vector3>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// vector3s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3s"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// vector4
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4"), property->getName());
	CHECK_EQUAL(TypeId::getType<Vector4>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// vector4s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4s"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// binary
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binary"), property->getName());
	CHECK_EQUAL(TypeId::getType<std::shared_ptr<BinaryBlock>>(), property->getType());
	CHECK(property->getMetaData() == nullptr);

	// binaries
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binaries"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// multidimensional
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("multidimensional"), property->getName());
	CHECK(property->getMetaData() == nullptr);

	// string lambda
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("string lambda"), property->getName());
	CHECK(property->getMetaData() != nullptr);

	// strings lambda
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("strings lambda"), property->getName());
	CHECK(property->getMetaData() != nullptr);

	// Finished
	++pi;
	property = *pi;
	CHECK(property == NULL);
}

TEST_F(TestDefinitionFixture, property_accessor_int)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();

	PropertyAccessor counter = klass_->bindProperty("counter", handle);
	CHECK(counter.isValid());
	CHECK_EQUAL(TypeId::getType<int32_t>(), counter.getType());
	CHECK_EQUAL(std::string("counter"), counter.getName());
	CHECK(counter.getMetaData() == nullptr);

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		CHECK(counter.setValue(value));
	}

	{
		int value = 0;
		Variant variant = counter.getValue();
		CHECK(variant.tryCast(value));
		CHECK_EQUAL(TEST_VALUE, value);
	}
}

TEST_F(TestDefinitionFixture, property_accessor_vector3)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();

	PropertyAccessor position = klass_->bindProperty("vector3", handle);
	CHECK(position.isValid());
	CHECK_EQUAL(TypeId::getType<Vector3>(), position.getType());
	CHECK_EQUAL(std::string("vector3"), position.getName());
	CHECK(position.getMetaData() == nullptr);

	const Vector3 TEST_VALUE = Vector3(1.0f, -1.0f, 0.0f);
	{
		Vector3 value = TEST_VALUE;
		CHECK(position.setValue(value));
	}

	{
		Vector3 value;
		Variant variant = position.getValue();
		CHECK(variant.tryCast(value));
		CHECK(TEST_VALUE == value);
	}
}

TEST_F(TestDefinitionFixture, property_accessor_float_collection)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();

	PropertyAccessor container = klass_->bindProperty("floats", handle);
	CHECK(container.isValid());

	Collection collection;
	bool isCollection = container.getValue().tryCast(collection);
	CHECK(isCollection);
	CHECK_EQUAL(std::string("floats"), container.getName());
	CHECK(container.getMetaData() == nullptr);

	fillValuesWithNumbers(collection);
	CHECK_EQUAL(5, collection.size());

	{
		float increment = 3.25f;
		float check_value = 1.0f;
		float value = 0.0f;
		int count = 0;
		for (auto iter = collection.begin(), end = collection.end(); iter != end; ++iter, ++count)
		{
			iter.value().tryCast(value);
			CHECK_EQUAL(check_value, value);
			check_value += increment;
			increment += 3.25f;
			CHECK(count < 5);
		}

		CHECK_EQUAL(5, count);
	}
}

TEST_F(TestDefinitionFixture, property_accessor_collection_metadata)
{
	{
		TestDefinitionObject data;
		ManagedObject<TestDefinitionObject> object(data);
		ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();

		PropertyAccessor container = klass_->bindProperty("strings", handle);
		CHECK(container.isValid());

		Collection collection;
		bool isCollection = container.getValue().tryCast(collection);
		CHECK(isCollection);
		CHECK_EQUAL(std::string("strings"), container.getName());
		CHECK(container.getMetaData() != nullptr);
		CHECK(findFirstMetaData<MetaInPlaceObj>(container, getDefinitionManager()) != nullptr);
		CHECK(findFirstMetaData<MetaUrlObj>(container, getDefinitionManager()) == nullptr);
	}

	{
		TestDefinitionObject data;
		ManagedObject<TestDefinitionObject> object(data);
		ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();

		PropertyAccessor container = klass_->bindProperty("strings[0]", handle);
		CHECK(container.isValid());

		Collection collection;
		bool isCollection = container.getValue().tryCast(collection);
		CHECK(!isCollection);
		CHECK_EQUAL(std::string("[0]"), container.getName());
		CHECK(container.getMetaData() != nullptr);
		CHECK(findFirstMetaData<MetaInPlaceObj>(container, getDefinitionManager()) == nullptr);
		CHECK(findFirstMetaData<MetaUrlObj>(container, getDefinitionManager()) != nullptr);
	}
}

TEST_F(TestDefinitionFixture, property_accessor_int_collection)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	ObjectHandle handle = object.getHandle();

	auto property = klass_->bindProperty("functional ints", handle);
	auto obj = handle.getBase<TestDefinitionObject>();
	CHECK(property.isValid());

	const size_t size = 5;
	std::vector<int32_t> input(size);
	std::iota(input.begin(), input.end(), 0); // Fill with values [0, 4]
	CHECK(property.setValue(input));
	CHECK(!obj->int32sSelf_);

	std::vector<int32_t> output;
	CHECK(property.getValue().tryCast(output));

	CHECK_EQUAL(output.size(), size);
	CHECK_EQUAL(input.size(), size);

	for (size_t i = 0; i < size; ++i)
	{
		CHECK_EQUAL(input[i], output[i]);
	}

	CHECK(property.setValue(obj->getInts()));
	CHECK(obj->int32sSelf_);
}

TEST_F(TestDefinitionFixture, property_accessor_string)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();

	auto property = klass_->bindProperty("functional string", handle);
	CHECK(property.isValid());

	// Setting const char* is currently not supported
	// Update this test if it becomes supported
	CHECK(!property.setValue("unsupported"));

	std::string output;
	CHECK(property.getValue().tryCast(output));
	CHECK_EQUAL(output, "test_string");
}

TEST_F(TestDefinitionFixture, property_accessor_polystruct)
{
    auto provider = ManagedObject<TestDefinitionObject>::make();

	PropertyAccessor position = klass_->bindProperty("link", provider.getHandleT());
	CHECK(position.isValid());
	CHECK_EQUAL(TypeId::getType<ObjectHandleT<TestPolyStruct2>>(), position.getType());
	CHECK_EQUAL(std::string("link"), position.getName());
	CHECK(position.getMetaData() != nullptr);

	const TestPolyStruct2 TEST_VALUE;
    ManagedObject<TestPolyStruct2> link(&TEST_VALUE);
    CHECK(position.setValue(link.getHandleT()));
	
	Variant variant = position.getValue();
	ObjectHandle handle;
	CHECK(variant.tryCast(handle));
    CHECK(handle.isValid());
    ObjectHandleT<TestPolyStruct2> value = reflectedCast<TestPolyStruct2>(handle, getDefinitionManager());
	CHECK(value == link.getHandleT());
}

TEST_F(TestDefinitionFixture, successive_bind)
{
	TestDefinitionObject data;
	ManagedObject<TestDefinitionObject> object(data);
	ObjectHandleT<TestDefinitionObject> handle = object.getHandleT();
	ObjectHandle take1;
	ObjectHandle take2;

	auto TEST_VALUE = ManagedObject<TestPolyStruct2>::make();

	{
		PropertyAccessor position = klass_->bindProperty("link", handle);
		CHECK(position.setValue(TEST_VALUE.getHandleT()));
		Variant value = position.getValue();
		CHECK(value.tryCast(take1));
	}

	{
		PropertyAccessor position = klass_->bindProperty("link", handle);
		Variant value = position.getValue();
		CHECK(value.tryCast(take2));
	}

	CHECK(take1 == take2);
}



class TestDerivationFixture: public TestReflectionFixture
{
public:
	IClassDefinition* base_klass;
	IClassDefinition* derived_klass;
	IClassDefinition* deep_klass;
	IClassDefinition* random_klass;

	TestDerivationFixture()
	{
		IDefinitionManager& definitionManager = getDefinitionManager();

		base_klass = definitionManager.getDefinition<TestBaseObject>();
		derived_klass = definitionManager.getDefinition<TestDerivedObject>();
		deep_klass = definitionManager.getDefinition<TestDeepObject>();
		random_klass = definitionManager.getDefinition<TestRandomObject>();
	}
};

TEST_F(TestDerivationFixture, hierarchy)
{
	CHECK_EQUAL(base_klass->getName(), derived_klass->getParentNames()[ 0 ]);
	CHECK_EQUAL(derived_klass->getName(), deep_klass->getParentNames()[ 0 ]);
	CHECK_EQUAL(base_klass->getName(), random_klass->getParentNames()[ 0 ] );

	CHECK(base_klass->canBeCastTo(*base_klass));
	CHECK(!base_klass->canBeCastTo(*derived_klass));
	CHECK(!base_klass->canBeCastTo(*deep_klass));
	CHECK(!base_klass->canBeCastTo(*random_klass));

	CHECK(derived_klass->canBeCastTo(*base_klass));
	CHECK(derived_klass->canBeCastTo(*derived_klass));
	CHECK(!derived_klass->canBeCastTo(*deep_klass));
	CHECK(!derived_klass->canBeCastTo(*random_klass));

	CHECK(deep_klass->canBeCastTo(*base_klass));
	CHECK(deep_klass->canBeCastTo(*derived_klass));
	CHECK(deep_klass->canBeCastTo(*deep_klass));
	CHECK(!deep_klass->canBeCastTo(*random_klass));

	CHECK(random_klass->canBeCastTo(*base_klass));
	CHECK(!random_klass->canBeCastTo(*derived_klass));
	CHECK(!random_klass->canBeCastTo(*deep_klass));
	CHECK(random_klass->canBeCastTo(*random_klass));
}

TEST_F(TestDerivationFixture, hierarchy_variables)
{
	ManagedObject<TestDeepObject> object(std::make_unique<TestDeepObject>());
	ObjectHandle handle = object.getHandle();
	CHECK(reflectedCast<TestBaseObject>(handle, getDefinitionManager()) != nullptr);

	// Access property on object
	PropertyAccessor deep = deep_klass->bindProperty("deep", handle);
	CHECK(deep.isValid());
	CHECK_EQUAL(TypeId::getType<bool>(), deep.getType());

	PropertyAccessor number = deep_klass->bindProperty("number", handle);
	CHECK(number.isValid());
	CHECK_EQUAL(TypeId::getType<float>(), number.getType());

	PropertyAccessor value = deep_klass->bindProperty("value", handle);
	CHECK(value.isValid());
	CHECK_EQUAL(TypeId::getType<int32_t>(), value.getType());

	{
		PropertyAccessor random = deep_klass->bindProperty("random()", handle);
		CHECK(!random.isValid());
	}
}

TEST_F(TestDefinitionFixture, multidimensional)
{
	ManagedObject<TestDefinitionObject> object(std::make_unique<TestDefinitionObject>());
	ObjectHandle handle = object.getHandle();

	auto obj = handle.getBase<TestDefinitionObject>();
	CHECK(obj);

    auto one = ManagedObject<TestStructure2>::make();
    auto two = ManagedObject<TestStructure2>::make();

	auto& mdElement = obj->multidimensional_["hello"];
	mdElement.push_back(one.getHandleT());
	mdElement[0]->name_ = "one";
	mdElement.push_back(two.getHandleT());
	mdElement[1]->name_ = "two";

	auto v0 = klass_->bindProperty("multidimensional[ \"hello\" ][0].name", handle).getValue();
	std::string s0;
	CHECK(v0.tryCast(s0));
	CHECK_EQUAL("one", s0);

	auto v1 = klass_->bindProperty("multidimensional[\"hello\"][1].name", handle).getValue();
	std::string s1;
	CHECK(v1.tryCast(s1));
	CHECK_EQUAL("two", s1);
}
} // end namespace wgt
