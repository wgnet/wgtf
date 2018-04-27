#include "core_reflection/i_definition_manager.hpp"
#include "metadata/command_instance.mpp"

namespace wgt
{
namespace CommandSystem
{
//==============================================================================
void initReflectedTypes(IDefinitionManager& definitionManager)
{
	REGISTER_DEFINITION(CommandInstance)
}
}
} // end namespace wgt
