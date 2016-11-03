#ifndef TEST_STRUCTURE_HPP
#define TEST_STRUCTURE_HPP

#include "test_macros.hpp"
#include <vector>
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
class TestPolyStruct;

struct TestEmptyStructure
{
	DECLARE_REFLECTED
};

struct TestStructure
{
	TestStructure();

	void init( const IDefinitionManager & defManager );

	DEFINE_TEST_DATA_TYPES()
};

struct TestInheritedStruct
	: public TestStructure
{
	TestInheritedStruct();

	void init( const IDefinitionManager & defManager );

	DEFINE_INHERITS_TEST_DATA_TYPES()

	bool operator==( const TestInheritedStruct & that ) const
	{
		return 
			Derivedboolean_ == that.Derivedboolean_ &&
			DerivedsignedInt_ == that.DerivedsignedInt_ &&
			DerivedunsignedInt_ == that.DerivedunsignedInt_ &&
			DerivedsignedInt64_ == that.DerivedsignedInt64_ &&
			DerivedunsignedInt64_ == that.DerivedunsignedInt64_ &&
			Derivedfloat_ == that.Derivedfloat_ &&
			Deriveddouble_ == that.Deriveddouble_ &&
			Derivedraw_string_ == that.Derivedraw_string_ &&
			Derivedstring_ == that.Derivedstring_ &&
			Derivedraw_wstring_ == that.Derivedraw_wstring_ &&
			Derivedwstring_ == that.Derivedwstring_;
	}
};
} // end namespace wgt
#endif //TEST_STRUCTURE
