#include "test_polystruct.hpp"
#include "test_objects_common.hpp"

namespace wgt
{
//==============================================================================
TestPolyStruct::TestPolyStruct() INIT_DATA()
{
}

//==============================================================================
void TestPolyStruct::init(const IDefinitionManager& definitionManager)
{
	INIT_COL_PRE(definitionManager);
}

//==============================================================================
TestInheritedPolyStruct::TestInheritedPolyStruct() INIT_DATA_INHERITS()
{
}

//==============================================================================
void TestInheritedPolyStruct::init(const IDefinitionManager& definitionManager)
{
	TestPolyStruct::init(definitionManager);
	INIT_COL_PRE_DERIVED(definitionManager);
}
} // end namespace wgt
