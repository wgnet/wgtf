#include "test_structure.hpp"
#include "test_objects_common.hpp"

namespace wgt
{
//==============================================================================
TestStructure::TestStructure()
	INIT_DATA()
{
}


//==============================================================================
void TestStructure::init( const IDefinitionManager & defManager )
{
	INIT_COL_PRE( defManager )
}


//==============================================================================
TestInheritedStruct::TestInheritedStruct()
	INIT_DATA_INHERITS()
{
}


//==============================================================================
void TestInheritedStruct::init( const IDefinitionManager & defManager )
{
	TestStructure::init( defManager );
	INIT_COL_PRE_DERIVED( defManager )
}
} // end namespace wgt
