#include "test_objects.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "metadata/test_structure.mpp"
#include "metadata/test_polystruct.mpp"
#include "metadata/test_methods_object.mpp"

namespace wgt
{
//==============================================================================
void TestObjects::initDefs( IDefinitionManager & definitionManager )
{
	REGISTER_DEFINITION( TestStructure );
	REGISTER_DEFINITION( TestInheritedStruct );
	REGISTER_DEFINITION( TestPolyStruct );
	REGISTER_DEFINITION( TestInheritedPolyStruct );
	REGISTER_DEFINITION( TestMethodsObject );
	REGISTER_DEFINITION( TestEmptyStructure );
}


void TestObjects::finiDefs( IDefinitionManager & definitionManager )
{
	DEREGISTER_DEFINITION( TestEmptyStructure );
	DEREGISTER_DEFINITION( TestMethodsObject );
	DEREGISTER_DEFINITION( TestInheritedPolyStruct );
	DEREGISTER_DEFINITION( TestPolyStruct );
	DEREGISTER_DEFINITION( TestInheritedStruct );
	DEREGISTER_DEFINITION( TestStructure );
}


//=============================================================================
TestStructure & TestObjects::getTestStructure()
{
	return structure_;
}
} // end namespace wgt
