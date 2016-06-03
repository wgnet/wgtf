#include "pch.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_iterator.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"

#include "test_helpers.hpp"
#include "test_objects.hpp"

#include "core_variant/collection.hpp"

namespace wgt
{
std::ostream & operator << (
	std::ostream & output, const TypeId & propType )
{
	output << propType.getName();
	return output;
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, create)
{
	auto subject = klass_->createManagedObject();

	CHECK_EQUAL(getClassIdentifier<TestDefinitionObject>(), klass_->getName());
	CHECK(klass_->getParent() == NULL);
	CHECK(klass_->getMetaData() != NULL);
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, properties)
{
	auto provider = klass_->createManagedObject();

	// counter
	PropertyIterator pi = klass_->allProperties().begin();
	IBasePropertyPtr property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// Function Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional counter"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() != NULL);

	// Function Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional text"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() != NULL);

	// Getter only Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter getter"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// Getter only Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text getter"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// raw string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw string"), property->getName());
	CHECK_EQUAL(TypeId::getType< const char * >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("string"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// strings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("strings"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// raw wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType< const wchar_t * >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::wstring >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// wstrings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstrings"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// exposed structure
	++pi;
	property = *pi;
	CHECK(property != NULL);
	const PropertyAccessor& paStruct = klass_->bindProperty(property->getName(), provider );
	CHECK( paStruct.isValid() );
	CHECK_EQUAL(std::string("exposed structure"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// exposed structures
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed structures"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// exposed object
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed object"), property->getName());
	CHECK_EQUAL(TypeId::getType< ObjectHandleT< TestPolyStruct2 > >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// exposed objects
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed objects"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// boolean
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("boolean"), property->getName());
	CHECK_EQUAL(TypeId::getType< bool >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// booleans
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("booleans"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// uint32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32"), property->getName());
	CHECK_EQUAL(TypeId::getType< uint32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// uint32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// int32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// int32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// uint64
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64"), property->getName());
	CHECK_EQUAL(TypeId::getType< uint64_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// uint64s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("float"), property->getName());
	CHECK_EQUAL(TypeId::getType< float >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// floats
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("floats"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// vector3
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3"), property->getName());
	CHECK_EQUAL(TypeId::getType< Vector3 >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// vector3s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// vector4
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4"), property->getName());
	CHECK_EQUAL(TypeId::getType< Vector4 >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// vector4s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// binary
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binary"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::shared_ptr< BinaryBlock > >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// binaries
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binaries"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// multidimensional
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("multidimensional"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// Finished
	++pi;
	property = *pi;
	CHECK(property == NULL);
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, derived_properties)
{
	const auto & derived_klass =
		*getDefinitionManager().getDefinition< TestDefinitionDerivedObject >();
	auto subject = derived_klass.createManagedObject();
	auto derived = reflectedCast< TestDefinitionDerivedObject >( subject, getDefinitionManager() );
	CHECK( derived != NULL );
	auto base = reflectedCast< TestDefinitionObject >( subject, getDefinitionManager() );
	CHECK( base != NULL );
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, property_iterator_self_only)
{
	const auto & derived_klass =
		*getDefinitionManager().getDefinition< TestDefinitionDerivedObject >();
	PropertyIterator pi = derived_klass.directProperties().begin();

	// some integer
	IBasePropertyPtr property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some integer"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// some float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some float"), property->getName());
	CHECK_EQUAL(TypeId::getType< float >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// Finished
	++pi;
	property = *pi;
	CHECK(property == NULL);
}


// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, property_iterator_parents)
{
	const auto & derived_klass =
		*getDefinitionManager().getDefinition< TestDefinitionDerivedObject >();
	auto provider = derived_klass.createManagedObject();

	PropertyIterator pi = derived_klass.allProperties().begin();
	// some integer
	IBasePropertyPtr property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some integer"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// some float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("some float"), property->getName());
	CHECK_EQUAL(TypeId::getType< float >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// Function Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional counter"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() != NULL);

	// Function Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("functional text"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() != NULL);

	// Getter only Counter
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("counter getter"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// Getter only Text
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("text getter"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// raw string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw string"), property->getName());
	CHECK_EQUAL(TypeId::getType< const char * >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// string
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("string"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::string >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// strings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("strings"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// raw wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("raw wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType< const wchar_t * >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// wstring
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstring"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::wstring >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// wstrings
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("wstrings"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// exposed structure
	++pi;
	property = *pi;
	CHECK(property != NULL);
	const PropertyAccessor& paStruct = derived_klass.bindProperty(property->getName(), provider );
	CHECK(paStruct.isValid());
	CHECK_EQUAL(std::string("exposed structure"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// exposed structures
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed structures"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// exposed object
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed object"), property->getName());
	CHECK_EQUAL(TypeId::getType< ObjectHandleT< TestPolyStruct2 > >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// exposed objects
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("exposed objects"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// boolean
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("boolean"), property->getName());
	CHECK_EQUAL(TypeId::getType< bool >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// booleans
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("booleans"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// uint32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32"), property->getName());
	CHECK_EQUAL(TypeId::getType< uint32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// uint32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint32s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// int32
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32"), property->getName());
	CHECK_EQUAL(TypeId::getType< int32_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// int32s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("int32s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// uint64
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64"), property->getName());
	CHECK_EQUAL(TypeId::getType< uint64_t >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// uint64s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("uint64s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// float
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("float"), property->getName());
	CHECK_EQUAL(TypeId::getType< float >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// floats
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("floats"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// vector3
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3"), property->getName());
	CHECK_EQUAL(TypeId::getType< Vector3 >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// vector3s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector3s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// vector4
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4"), property->getName());
	CHECK_EQUAL(TypeId::getType< Vector4 >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// vector4s
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("vector4s"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// binary
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binary"), property->getName());
	CHECK_EQUAL(TypeId::getType< std::shared_ptr< BinaryBlock > >(), property->getType());
	CHECK(property->getMetaData() == NULL);

	// binaries
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("binaries"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// multidimensional
	++pi;
	property = *pi;
	CHECK(property != NULL);
	CHECK_EQUAL(std::string("multidimensional"), property->getName());
	CHECK(property->getMetaData() == NULL);

	// Finished
	++pi;
	property = *pi;
	CHECK(property == NULL);
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, property_accessor_int)
{
	auto provider = klass_->createManagedObject();

	PropertyAccessor counter = klass_->bindProperty("counter", provider );
	CHECK(counter.isValid());
	CHECK_EQUAL(TypeId::getType< int32_t >(), counter.getType());
	CHECK_EQUAL(std::string("counter"), counter.getName());
	CHECK(counter.getMetaData() == NULL);

	const int TEST_VALUE = 57;
	{
		int value = TEST_VALUE;
		CHECK(counter.setValue(value));
	}

	{
		int value = 0;
		Variant variant = counter.getValue();
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, property_accessor_vector3)
{
	auto provider = klass_->createManagedObject();

	PropertyAccessor position = klass_->bindProperty("vector3", provider );
	CHECK(position.isValid());
	CHECK_EQUAL(TypeId::getType< Vector3 >(), position.getType());
	CHECK_EQUAL(std::string("vector3"), position.getName());
	CHECK(position.getMetaData() == NULL);

	const Vector3 TEST_VALUE = Vector3(1.0f, -1.0f, 0.0f);
	{
		Vector3 value = TEST_VALUE;
		CHECK(position.setValue(value));
	}

	{
		Vector3 value;
		Variant variant = position.getValue();
		CHECK(variant.tryCast( value ));
		CHECK_EQUAL(TEST_VALUE, value);
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, property_accessor_collection)
{
	auto provider = klass_->createManagedObject();

	PropertyAccessor container = klass_->bindProperty("floats", provider );
	CHECK(container.isValid());

	Collection collection;
	bool isCollection = container.getValue().tryCast( collection );
	CHECK( isCollection );
	CHECK_EQUAL(std::string("floats"), container.getName());
	CHECK(container.getMetaData() == NULL);

	fillValuesWithNumbers( collection );

	CHECK_EQUAL(5, collection.size());

	{
		float increment = 3.25f;
		float check_value = 1.0f;
		float value = 0.0f;
		int count = 0;
		for (auto iter = collection.begin(), end = collection.end(); iter != end; ++iter, ++count)
		{
			iter.value().tryCast( value );
			CHECK_EQUAL(check_value, value);
			check_value += increment;
			increment += 3.25f;
			CHECK(count < 5);
		}

		CHECK_EQUAL(5, count);
	}
}

// =============================================================================

class TestBaseObject
{
	DECLARE_REFLECTED

public:
	int value_;

	TestBaseObject() :
		value_(0)
	{
	}
};

BEGIN_EXPOSE(TestBaseObject, MetaNone())
	EXPOSE("value", value_, MetaNone())
END_EXPOSE()


class TestDerivedObject : public TestBaseObject
{
	DECLARE_REFLECTED

public:
	float number_;

	TestDerivedObject() :
		TestBaseObject(),
		number_(1.0f)
	{
	}
};

BEGIN_EXPOSE(TestDerivedObject, TestBaseObject, MetaNone())
	EXPOSE("number", number_, MetaNone())
END_EXPOSE()

class TestDeepObject : public TestDerivedObject
{
	DECLARE_REFLECTED

public:
	bool deep_;

	TestDeepObject() :
		deep_(false)
	{
	}
};

BEGIN_EXPOSE(TestDeepObject, TestDerivedObject, MetaNone())
	EXPOSE("deep", deep_, MetaNone())
END_EXPOSE()

class TestRandomObject : public TestBaseObject
{
	DECLARE_REFLECTED

public:
	const int random_;

	TestRandomObject() :
		TestBaseObject(),
		random_(rand())
	{
	}

	const int & random() const
	{
		return random_;
	}
};

BEGIN_EXPOSE(TestRandomObject, TestBaseObject, MetaNone())
	//EXPOSE_MEMBER(random_, "random", MetaNone()) // Cannot expose constant variables
	EXPOSE("random()", random, MetaNone())
END_EXPOSE()

class TestDerivationFixture
{
public:
	IClassDefinition * base_klass;
	IClassDefinition * derived_klass;
	IClassDefinition * deep_klass;
	IClassDefinition * random_klass;

	TestDerivationFixture()
		: objectManager_( new ObjectManager() )
		, definitionManager_( new DefinitionManager( *objectManager_ ) )
	{
		objectManager_->init( definitionManager_.get() );
		IDefinitionManager & definitionManager = *definitionManager_;
		Reflection::initReflectedTypes( definitionManager );
		REGISTER_DEFINITION( TestBaseObject );
		REGISTER_DEFINITION( TestDerivedObject );
		REGISTER_DEFINITION( TestDeepObject );
		REGISTER_DEFINITION( TestRandomObject );

		base_klass =
			definitionManager.getDefinition< TestBaseObject >();
		derived_klass =
			definitionManager.getDefinition< TestDerivedObject >();
		deep_klass =
			definitionManager.getDefinition< TestDeepObject >();
		random_klass =
			definitionManager.getDefinition< TestRandomObject >();
	}

	IDefinitionManager & getDefinitionManager() const
	{
		return *definitionManager_;
	}

private:
	std::unique_ptr< ObjectManager > objectManager_;
	std::unique_ptr< DefinitionManager > definitionManager_;
};

// -----------------------------------------------------------------------------
TEST_F( TestDerivationFixture, hierarchy)
{
	CHECK_EQUAL(base_klass, derived_klass->getParent());
	CHECK_EQUAL(derived_klass, deep_klass->getParent());
	CHECK_EQUAL(base_klass, random_klass->getParent());

	CHECK( base_klass->canBeCastTo(*base_klass));
	CHECK(!base_klass->canBeCastTo(*derived_klass));
	CHECK(!base_klass->canBeCastTo(*deep_klass));
	CHECK(!base_klass->canBeCastTo(*random_klass));

	CHECK( derived_klass->canBeCastTo(*base_klass));
	CHECK( derived_klass->canBeCastTo(*derived_klass));
	CHECK(!derived_klass->canBeCastTo(*deep_klass));
	CHECK(!derived_klass->canBeCastTo(*random_klass));

	CHECK( deep_klass->canBeCastTo(*base_klass));
	CHECK( deep_klass->canBeCastTo(*derived_klass));
	CHECK( deep_klass->canBeCastTo(*deep_klass));
	CHECK(!deep_klass->canBeCastTo(*random_klass));

	CHECK( random_klass->canBeCastTo(*base_klass));
	CHECK(!random_klass->canBeCastTo(*derived_klass));
	CHECK(!random_klass->canBeCastTo(*deep_klass));
	CHECK( random_klass->canBeCastTo(*random_klass));
}

// -----------------------------------------------------------------------------
TEST_F( TestDerivationFixture, hierarchy_variables )
{
	auto provider = deep_klass->createManagedObject();
	CHECK(reflectedCast< TestBaseObject >( provider, getDefinitionManager() ) != NULL);

	// Access property on object
	PropertyAccessor deep = deep_klass->bindProperty("deep", provider );
	CHECK(deep.isValid());
	CHECK_EQUAL(TypeId::getType< bool >(), deep.getType());

	PropertyAccessor number = deep_klass->bindProperty("number", provider);
	CHECK(number.isValid());
	CHECK_EQUAL(TypeId::getType< float >(), number.getType());

	PropertyAccessor value = deep_klass->bindProperty("value", provider );
	CHECK(value.isValid());
	CHECK_EQUAL(TypeId::getType< int32_t >(), value.getType());

	{
		PropertyAccessor random = deep_klass->bindProperty("random()", provider );
		CHECK(!random.isValid());
	}
}

TEST_F( TestDefinitionFixture, multidimensional )
{
	auto provider = klass_->createManagedObject();

	auto obj = provider.getBase< TestDefinitionObject >();
	CHECK( obj );

	auto& mdElement = obj->multidimensional_[ "hello" ];
	mdElement.push_back(
		getDefinitionManager().create< TestStructure2 >() );
	mdElement[0]->name_ = "one";
	mdElement.push_back(
		getDefinitionManager().create< TestStructure2 >() );
	mdElement[1]->name_ = "two";

	auto v0 = klass_->bindProperty( "multidimensional[ \"hello\" ][0].name", provider ).getValue();
	std::string s0;
	CHECK( v0.tryCast( s0 ) );
	CHECK_EQUAL( "one", s0 );

	auto v1 = klass_->bindProperty( "multidimensional[\"hello\"][1].name", provider ).getValue();
	std::string s1;
	CHECK( v1.tryCast( s1 ) );
	CHECK_EQUAL( "two", s1 );
}
} // end namespace wgt
