#include "item_model_controller.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_data_model_cmds/commands/set_item_data_command.hpp"
#include "core_data_model_cmds/commands/set_item_data_command_arg.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{


ItemModelController::ItemModelController( IComponentContext & context )
	: commandManager_( context )
	, definitionManager_( context )
{
}


bool ItemModelController::setValue( AbstractItemModel & model,
	const AbstractItemModel::ItemIndex & index,
	size_t roleId,
	const Variant & data ) /* override */
{
	std::unique_ptr< ItemDataCommandArgument > args(
		new ItemDataCommandArgument() );
	args->setModel( model );
	args->setIndex( index );
	args->setValue( roleId, data );
	
	// Access is only on the main thread
	assert( std::this_thread::get_id() == commandManager_->ownerThreadId() );

	const auto commandId = getClassIdentifier< SetItemDataCommand >();
	const auto pArgsDefinition =
		definitionManager_->getDefinition< ItemDataCommandArgument >();
	ObjectHandle commandArgs( std::move( args ), pArgsDefinition );
	auto command = commandManager_->queueCommand( commandId, commandArgs );
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	assert( command->isComplete() );

	// Completion is required to get the return value
	return wgt::isCommandSuccess( command->getErrorCode() );
}


} // end namespace wgt
