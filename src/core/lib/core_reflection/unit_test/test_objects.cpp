#include "pch.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/utilities/reflection_method_utilities.hpp"

#include "test_objects.hpp"


namespace wgt
{
BEGIN_EXPOSE( TestDefinitionObject, MetaNone() )
	EXPOSE( "counter", counter_ )
	EXPOSE( "text", text_ )
	EXPOSE( "functional counter", getCounter, setCounter, MetaNone() )
	EXPOSE( "functional text", getText, setText, MetaNone() )
	EXPOSE( "counter getter", getCounter )
	EXPOSE( "text getter", getText )
	EXPOSE( "raw string", raw_string_ )
	EXPOSE( "string", string_ )
	EXPOSE( "strings", strings_ )
	EXPOSE( "raw wstring", raw_wstring_ )
	EXPOSE( "wstring", wstring_ )
	EXPOSE( "wstrings", wstrings_ )
	EXPOSE( "exposed structure", exposedStruct_ )
	EXPOSE( "exposed structures", exposedStructs_ )
	EXPOSE( "exposed object", exposedObject_ )
	EXPOSE( "exposed objects", exposedObjects_ )
	EXPOSE( "boolean", boolean_ )
	EXPOSE( "booleans", booleans_ )
	EXPOSE( "uint32", uint32_ )
	EXPOSE( "uint32s", uint32s_ )
	EXPOSE( "int32", int32_ )
	EXPOSE( "int32s", int32s_ )
	EXPOSE( "uint64", uint64_ )
	EXPOSE( "uint64s", uint64s_ )
	EXPOSE( "float", float_ )
	EXPOSE( "floats", floats_ )
	EXPOSE( "vector3", vector3_ )
	EXPOSE( "vector3s", vector3s_ )
	EXPOSE( "vector4", vector4_ )
	EXPOSE( "vector4s", vector4s_ )
	EXPOSE( "binary", binary_ )
	EXPOSE( "binaries", binaries_ )
	EXPOSE( "multidimensional", multidimensional_ )
END_EXPOSE()

BEGIN_EXPOSE( TestDefinitionDerivedObject, TestDefinitionObject, MetaNone() )
	EXPOSE( "some integer", someInteger_ )
	EXPOSE( "some float", someFloat_ )
END_EXPOSE()

BEGIN_EXPOSE( TestStructure2, MetaNone() )
	EXPOSE( "name", name_ )
END_EXPOSE()

BEGIN_EXPOSE( TestPolyStruct2, MetaNone() )
	EXPOSE( "name", name_ )
END_EXPOSE()

TestDefinitionFixture::TestDefinitionFixture()
{
	IDefinitionManager & definitionManager = getDefinitionManager();
	REGISTER_DEFINITION( TestStructure2 );
	REGISTER_DEFINITION( TestDefinitionObject );
	REGISTER_DEFINITION( TestDefinitionDerivedObject );
	klass_ = definitionManager.getDefinition< TestDefinitionObject >();
	derived_klass_ = definitionManager.getDefinition< TestDefinitionDerivedObject >();
}

namespace
{
	uint32_t RandomNumber32() 
	{
		RefObjectId uid( RefObjectId::generate() );
		return uid.getA() + uid.getB() + uid.getC() + uid.getD();
	}

	uint64_t RandomNumber64() 
	{
		RefObjectId uid( RefObjectId::generate() );
		uint64_t a = ( uint64_t ) uid.getA();
		uint64_t b = ( uint64_t ) uid.getB();
		uint64_t c = ( uint64_t ) uid.getC();
		uint64_t d = ( uint64_t ) uid.getD();
		return (a << 32) + (b << 32) + c + d;
	}

	std::string RandomString() 
	{
		std::string random( "Random Data: " );
		random += RefObjectId::generate().toString();
		random += RefObjectId::generate().toString();
		random += RefObjectId::generate().toString();
		random += RefObjectId::generate().toString();
		return random;
	}
}


//------------------------------------------------------------------------------
TestDefinitionObject::TestDefinitionObject() :
	counter_( 0 ), raw_string_( "RAW READ-0NLY STR1NG!!!" ), 
	raw_wstring_( L"RAW READ-0NLY W1DE STR1NG!!!" ), boolean_( false ),
	uint32_( 0 ), int32_( 0 ), uint64_( 0L ), float_( 0.f )
{
}


//------------------------------------------------------------------------------
bool TestDefinitionObject::operator==( const TestDefinitionObject& tdo ) const
{
	if (counter_ != tdo.counter_)
		return false;

	if (text_ != tdo.text_)
		return false;

	if (std::string(raw_string_) != std::string( tdo.raw_string_ ))
		return false;

	if (string_ != tdo.string_  || strings_ != tdo.strings_)
		return false;

	if (std::wstring( raw_wstring_ ) != std::wstring( tdo.raw_wstring_ ))
		return false;

	if (wstring_ != tdo.wstring_ || wstrings_ != tdo.wstrings_)
		return false;

	if (exposedStruct_ != tdo.exposedStruct_ || exposedStructs_ != tdo.exposedStructs_)
		return false;

	if (!(exposedObject_ == tdo.exposedObject_ && exposedObjects_ == tdo.exposedObjects_))
		return false;

	if (boolean_ != tdo.boolean_ || booleans_ != tdo.booleans_)
		return false;

	if (uint32_ != tdo.uint32_ || uint32s_ != tdo.uint32s_)
		return false;

	if (int32_ != tdo.int32_ || int32s_ != tdo.int32s_)
		return false;

	if (uint64_ != tdo.uint64_ || uint64s_ != tdo.uint64s_)
		return false;

	if (float_ != tdo.float_ || floats_ != tdo.floats_)
		return false;

	if (vector3_ != tdo.vector3_ || vector3s_ != tdo.vector3s_)
		return false;

	if (vector4_ != tdo.vector4_ || vector4s_ != tdo.vector4s_)
		return false;

	if (binary_->compare( *tdo.binary_ ) != 0 || binaries_.size() != tdo.binaries_.size())
		return false;

	auto i = 0u;
	for (; i < binaries_.size() && binaries_[i]->compare( *tdo.binaries_[i] ) == 0; ++i);
	if (i != binaries_.size())
		return false;

	if (multidimensional_ != tdo.multidimensional_)
		return false;

	return true;
}


//------------------------------------------------------------------------------
bool TestDefinitionObject::operator!=( const TestDefinitionObject & tdo ) const
{
	return !operator==( tdo );
}
} // end namespace wgt
