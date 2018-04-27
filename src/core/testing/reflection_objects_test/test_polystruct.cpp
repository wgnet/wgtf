#include "test_polystruct.hpp"
#include "test_objects_common.hpp"

namespace wgt
{
//==============================================================================
TestPolyStruct::TestPolyStruct() INIT_DATA()
{
    const IDefinitionManager& definitionManager = *Depends::get<IDefinitionManager>();
	INIT_COL_PRE(definitionManager);
}

//==============================================================================
TestInheritedPolyStruct::TestInheritedPolyStruct() INIT_DATA_INHERITS()
{
    const IDefinitionManager& definitionManager = *Depends::get<IDefinitionManager>();
	INIT_COL_PRE_DERIVED(definitionManager);
}

} // end namespace wgt
