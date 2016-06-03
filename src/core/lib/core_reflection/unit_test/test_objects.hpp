#ifndef TEST_OBJECTS2_HPP
#define TEST_OBJECTS2_HPP
#include "pch.hpp"

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"

#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "wg_types/binary_block.hpp"

#include "test_reflection_fixture.hpp"
#include <memory>
#include <vector>
#include <unordered_map>


namespace wgt
{
//------------------------------------------------------------------------------
struct TestStructure2
{
	bool operator==( const TestStructure2& tps ) const
	{
		return name_ == tps.name_;
	}

	bool operator!=( const TestStructure2& tps ) const
	{
		return !operator==( tps );
	}

	std::string name_;
};


//------------------------------------------------------------------------------
class TestPolyStruct2
{
	DECLARE_REFLECTED

public:
	bool operator==( const TestPolyStruct2& tps ) const
	{
		return name_ == tps.name_;
	}

	bool operator!=( const TestPolyStruct2& tps ) const
	{
		return !operator==( tps );
	}

	std::string name_;
};


//------------------------------------------------------------------------------
class TestDefinitionObject
{
	DECLARE_REFLECTED

public:
	void setCounter( const int & value ) { counter_ = value; }
	void getCounter( int * value ) const { *value = counter_; }

	void setText( const std::string & value ) { text_ = value; }
	const std::string & getText() const { return text_; }

	TestDefinitionObject();
	bool operator==( const TestDefinitionObject& tdo ) const;
	bool operator!=( const TestDefinitionObject& tdo ) const;

public:
	int counter_;

	std::string text_;

	// PropertyType::Raw_String
	const char * raw_string_;

	// PropertyType::String
	std::string string_;
	std::vector< std::string > strings_;

	// PropertyType::Raw_WString
	const wchar_t * raw_wstring_;

	// PropertyType::WString
	std::wstring wstring_;
	std::vector< std::wstring > wstrings_;

	// PropertyType::ExposedStruct
	TestStructure2 exposedStruct_;
	std::vector< TestStructure2 > exposedStructs_;

	// PropertyType::Link
	ObjectHandleT< TestPolyStruct2 > exposedObject_;
	std::vector< ObjectHandleT< TestPolyStruct2 > > exposedObjects_;

	// PropertyType::Boolean
	bool boolean_;
	std::vector< bool > booleans_;

	// PropertyType::UInt32
	uint32_t uint32_;
	std::vector< uint32_t > uint32s_;

	// PropertyType::Int32
	int32_t int32_;
	std::vector< int32_t > int32s_;

	// PropertyType::UInt64
	uint64_t uint64_;
	std::vector< uint64_t > uint64s_;

	// PropertyType::Float
	float float_;
	std::vector< float > floats_;

	// PropertyType::Enum
	//! TODO?
	
	// PropertyType::Vector3_Type
	Vector3 vector3_;
	std::vector< Vector3 > vector3s_;

	// PropertyType::Vector4_Type
	Vector4 vector4_;
	std::vector< Vector4 > vector4s_;

	// PropertyType::Raw_Data,
	std::shared_ptr< BinaryBlock > binary_;
	std::vector< std::shared_ptr< BinaryBlock > > binaries_;

	// multidimensional container
	std::unordered_map<
		std::string,
		std::vector< ObjectHandleT< TestStructure2 > >
	> multidimensional_;
};


//------------------------------------------------------------------------------
class TestDefinitionDerivedObject : public TestDefinitionObject
{
	DECLARE_REFLECTED

public:
	bool operator==( const TestDefinitionDerivedObject & tdo ) const
	{
		return 
			someInteger_ == tdo.someInteger_ &&
			fabsf( someFloat_ - tdo.someFloat_ ) < 0.0004f &&
			TestDefinitionObject::operator==( tdo );
	}
	bool operator!=( const TestDefinitionDerivedObject & tdo ) const
	{
		return !operator==( tdo );
	}

public:
	int someInteger_;

	float someFloat_;
};


//------------------------------------------------------------------------------
class TestDefinitionFixture
	: public TestReflectionFixture
{
public:
	TestDefinitionFixture();

	void fillValuesWithNumbers(Collection& values)
	{
		float increment = 3.25f;
		float value = 1.0f;
		for (int i = 0; i < 5; ++i)
		{
			auto it = values.insert(i);
			it.setValue(value);
			value += increment;
			increment += 3.25f;
		}
	}

public:
	IClassDefinition * klass_;
	IClassDefinition * derived_klass_;
};
} // end namespace wgt
#endif //TEST_OBJECTS2_HPP
