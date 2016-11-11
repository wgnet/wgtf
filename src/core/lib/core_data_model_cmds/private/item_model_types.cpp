#include "item_model_types.hpp"
#include "commands/metadata/insert_default_command_arg.mpp"
#include "commands/metadata/insert_item_command_arg.mpp"
#include "commands/metadata/insert_rows_command_arg.mpp"
#include "commands/metadata/move_item_data_command_arg.mpp"
#include "commands/metadata/remove_item_command_arg.mpp"
#include "commands/metadata/remove_rows_command_arg.mpp"
#include "commands/metadata/set_model_data_command_arg.mpp"
#include "commands/metadata/set_item_data_command_arg.mpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
namespace ItemModelCommands
{
void registerTypes(IDefinitionManager& definitionManager)
{
	REGISTER_DEFINITION(InsertDefaultCommandArgument);
	REGISTER_DEFINITION(InsertItemCommandArgument);
	REGISTER_DEFINITION(InsertRowsCommandArgument);
	REGISTER_DEFINITION(MoveItemDataCommandArgument);
	REGISTER_DEFINITION(RemoveItemCommandArgument);
	REGISTER_DEFINITION(RemoveRowsCommandArgument);
	REGISTER_DEFINITION(SetItemDataCommandArgument);
	REGISTER_DEFINITION(SetModelDataCommandArgument);
}
}

} // end namespace wgt
