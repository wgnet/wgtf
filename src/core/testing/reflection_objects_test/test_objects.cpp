#include "test_objects.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "test_structure.hpp"
#include "test_polystruct.hpp"
#include "test_methods_object.hpp"

namespace wgt
{

//=============================================================================
TestStructure& TestObjects::getTestStructure()
{
	return structure_;
}
} // end namespace wgt
