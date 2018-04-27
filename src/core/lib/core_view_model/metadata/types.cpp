#include "types.hpp"

#include "property_view_model.mpp"
#include "view_model.mpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
namespace CoreViewModelTypes
{
void registerTypes(IDefinitionManager& definitionManager)
{
	REGISTER_DEFINITION(PropertyViewModel);
	REGISTER_DEFINITION(ViewModel);
}
}
}
