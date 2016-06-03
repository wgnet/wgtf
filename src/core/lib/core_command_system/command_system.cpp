#include "core_reflection/i_definition_manager.hpp"
#include "metadata/command_instance.mpp"
#include "metadata/compound_command.mpp"
#include "metadata/macro_object.mpp"

namespace wgt
{
namespace CommandSystem
{

//==============================================================================
void initReflectedTypes( IDefinitionManager & definitionManager )
{
	REGISTER_DEFINITION( CommandInstance )
	REGISTER_DEFINITION( CompoundCommand )
	REGISTER_DEFINITION( MacroObject)
	REGISTER_DEFINITION( MacroEditObject)
	REGISTER_DEFINITION( ReflectedPropertyCommandArgumentController )
	REGISTER_DEFINITION( MethodParam )
	REGISTER_DEFINITION( ReflectedMethodCommandParametersController )
}

}
} // end namespace wgt
