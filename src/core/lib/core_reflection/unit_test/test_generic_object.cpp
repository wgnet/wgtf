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
TEST_F( TestDefinitionFixture, createGenericObject )
{
	auto handle = GenericObject::create( getDefinitionManager() );
	auto genericObj = handle.get();
	CHECK( genericObj != nullptr );

	int intValue = 1234;
	genericObj->set( "intValue", intValue );
	{
		int value;
		CHECK( genericObj->get( "intValue", value ) );
		CHECK_EQUAL( intValue, value );
	}

	int newIntValue = 5678;
	genericObj->set( "intValue", newIntValue );
	{
		int value;
		CHECK( genericObj->get( "intValue", value ) );
		CHECK_EQUAL( newIntValue, value );
	}

	Vector3 vector3Value( 1, 2, 3 );
	genericObj->set( "vector3Value", vector3Value );
	{
		Vector3 value;
		CHECK( genericObj->get( "vector3Value", value ) );
		CHECK( vector3Value == value );
	}

	std::string stringValue = "Hello World";
	genericObj->set( "stringValue", stringValue );
	{
		std::string value;
		CHECK( genericObj->get( "stringValue", value ) );
		CHECK_EQUAL( stringValue, value );
	}

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set( "testStructure", testStructure );
	{
		TestStructure2 value;
		CHECK( genericObj->get( "testStructure", value ) );
		CHECK( testStructure == value );
	}

	auto testDefinitionObject = 
		getDefinitionManager().create< TestDefinitionObject >();
	genericObj->set( "testDefinitionObject", testDefinitionObject );
	{
		ObjectHandle provider;
		CHECK( genericObj->get( "testDefinitionObject", provider ) );
		CHECK( testDefinitionObject.get() == provider.getBase< TestDefinitionObject >() );
	}

	auto testDefinitionDerivedObject = 
		getDefinitionManager().create< TestDefinitionDerivedObject >();
	genericObj->set( "testDefinitionObject", testDefinitionDerivedObject );
	{
		ObjectHandle provider;
		CHECK( genericObj->get( "testDefinitionObject", provider ) );
		CHECK( testDefinitionDerivedObject.get() == provider.getBase< TestDefinitionDerivedObject >() );
	}
}

TEST_F( TestDefinitionFixture, createMultipleGenericObjects )
{
	auto handle = GenericObject::create( getDefinitionManager() );
	auto genericObj = handle.get();
	CHECK( genericObj != nullptr );

	int intValue = 1234;
	genericObj->set( "intValue", intValue );

	Vector3 vector3Value( 1, 2, 3 );
	genericObj->set( "vector3Value", vector3Value );

	std::string stringValue = "Hello World";
	genericObj->set( "stringValue", stringValue );

	auto newHandle = handle.getDefinition( getDefinitionManager() )->createManagedObject();
	auto newGenericObj = newHandle.getBase< GenericObject >();

	{
		int value;
		CHECK( genericObj->get( "intValue", value ) );
		CHECK_EQUAL( intValue, value );
	}
	{
		Vector3 value;
		CHECK( genericObj->get( "vector3Value", value ) );
		CHECK( vector3Value == value );
	}
	{
		std::string value;
		CHECK( genericObj->get( "stringValue", value ) );
		CHECK_EQUAL( stringValue, value );
	}

	{
		int value;
		CHECK( !newGenericObj->get( "intValue", value ) );
	}
	{
		Vector3 value;
		CHECK( !newGenericObj->get( "vector3Value", value ) );
	}
	{
		std::string value;
		CHECK( !newGenericObj->get( "stringValue", value ) );
	}

	int newIntValue = 5678;
	newGenericObj->set( "intValue", newIntValue );

	{
		int value;
		CHECK( genericObj->get( "intValue", value ) );
		CHECK_EQUAL( intValue, value );
	}
	{
		int value;
		CHECK( newGenericObj->get( "intValue", value ) );
		CHECK_EQUAL( newIntValue, value );
	}

	TestStructure2 testStructure;
	auto guid = RefObjectId::generate();
	testStructure.name_ = guid.toString();
	genericObj->set( "testStructure", testStructure );
	{
		TestStructure2 value;
		CHECK( genericObj->get( "testStructure", value ) );
		CHECK( testStructure == value );
	}
	{
		TestStructure2 value;
		CHECK( !newGenericObj->get( "testStructure", value ) );
	}
}
} // end namespace wgt
