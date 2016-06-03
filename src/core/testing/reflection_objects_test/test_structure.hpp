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
};
} // end namespace wgt
#endif //TEST_STRUCTURE
