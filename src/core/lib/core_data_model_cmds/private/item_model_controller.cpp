#include "item_model_controller.hpp"

#include "commands/insert_default_command.hpp"
#include "commands/insert_default_command_arg.hpp"
#include "commands/insert_item_command.hpp"
#include "commands/insert_item_command_arg.hpp"
#include "commands/insert_rows_command.hpp"
#include "commands/insert_rows_command_arg.hpp"
#include "commands/move_item_data_command.hpp"
#include "commands/move_item_data_command_arg.hpp"
#include "commands/remove_item_command.hpp"
#include "commands/remove_item_command_arg.hpp"
#include "commands/remove_rows_command.hpp"
#include "commands/remove_rows_command_arg.hpp"
#include "commands/set_item_data_command.hpp"
#include "commands/set_item_data_command_arg.hpp"
#include "commands/set_model_data_command.hpp"
#include "commands/set_model_data_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
namespace ItemModelController_Detail
{
bool insertRows(DIRef<ICommandManager>& commandManager,
                DIRef<IDefinitionManager>& definitionManager,
                AbstractItemModel& model,
                int startPos,
                InsertRowsCommandArgument::Type type,
                int count,
                const AbstractItem* pParent)
{
	// Access is only on the main thread
	assert(std::this_thread::get_id() == commandManager->ownerThreadId());

	const auto commandId = getClassIdentifier<InsertRowsCommand>();

	const auto commandArgs = definitionManager->create<InsertRowsCommandArgument>();
	commandArgs->setModel( model );
	commandArgs->setStartPos(startPos, type);
	commandArgs->setCount(count);
	commandArgs->setParent(pParent);

	auto command = commandManager->queueCommand(commandId, commandArgs);
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

bool removeRows(DIRef<ICommandManager>& commandManager,
                DIRef<IDefinitionManager>& definitionManager,
                AbstractItemModel& model,
                int startPos,
                RemoveRowsCommandArgument::Type type,
                int count,
                const AbstractItem* pParent)
{
	// Access is only on the main thread
	assert(std::this_thread::get_id() == commandManager->ownerThreadId());

	const auto commandId = getClassIdentifier<RemoveRowsCommand>();

	const auto commandArgs = definitionManager->create<RemoveRowsCommandArgument>();
	commandArgs->setModel( model );
	commandArgs->setStartPos(startPos, type);
	commandArgs->setCount(count);
	commandArgs->setParent(pParent);

	auto command = commandManager->queueCommand(commandId, commandArgs);
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

} // end namespace ItemModelController_Detail

ItemModelController::ItemModelController(IComponentContext& context)
    : commandManager_(context)
    , definitionManager_(context)
{
}

bool ItemModelController::setValue(AbstractItemModel& model,
                                   const AbstractItemModel::ItemIndex& index,
                                   size_t roleId,
                                   const Variant& data) /* override */
{
	// Access is only on the main thread
	assert( std::this_thread::get_id() == commandManager_->ownerThreadId() );

	const auto commandId = getClassIdentifier<SetItemDataCommand>();

	const auto commandArgs = definitionManager_->create<SetItemDataCommandArgument>();
	commandArgs->setModel( model );
	commandArgs->setIndex(index);
	commandArgs->setValue(roleId, data);

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

bool ItemModelController::setModelData(AbstractItemModel& model,
                                       int row,
                                       int column,
                                       size_t roleId,
                                       const Variant& data) /* override */
{
	// Access is only on the main thread
	assert( std::this_thread::get_id() == commandManager_->ownerThreadId() );

	const auto commandId = getClassIdentifier<SetModelDataCommand>();

	const auto commandArgs = definitionManager_->create<SetModelDataCommandArgument>();
	commandArgs->setModel( model );
	commandArgs->setIndex(row, column);
	commandArgs->setValue(roleId, data);

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

bool ItemModelController::insertRows(AbstractItemModel& model,
                                     int row,
                                     int count,
                                     const AbstractItem* pParent) /* override */
{
	return ItemModelController_Detail::insertRows(commandManager_,
	                                              definitionManager_,
	                                              model,
	                                              row,
	                                              InsertRowsCommandArgument::Type::ROW,
	                                              count,
	                                              pParent);
}

bool ItemModelController::insertColumns(AbstractItemModel& model,
                                        int column,
                                        int count,
                                        const AbstractItem* pParent) /* override */
{
	return ItemModelController_Detail::insertRows(commandManager_,
	                                              definitionManager_,
	                                              model,
	                                              column,
	                                              InsertRowsCommandArgument::Type::COLUMN,
	                                              count,
	                                              pParent);
}

bool ItemModelController::removeRows(AbstractItemModel& model,
                                     int row,
                                     int count,
                                     const AbstractItem* pParent) /* override */
{
	return ItemModelController_Detail::removeRows(commandManager_,
	                                              definitionManager_,
	                                              model,
	                                              row,
	                                              RemoveRowsCommandArgument::Type::ROW,
	                                              count,
	                                              pParent);
}

bool ItemModelController::removeColumns(AbstractItemModel& model,
                                        int column,
                                        int count,
                                        const AbstractItem* pParent) /* override */
{
	return ItemModelController_Detail::removeRows(commandManager_,
	                                              definitionManager_,
	                                              model,
	                                              column,
	                                              RemoveRowsCommandArgument::Type::COLUMN,
	                                              count,
	                                              pParent);
}

bool ItemModelController::moveRows(AbstractItemModel& model,
                                   const AbstractItem* sourceParent,
                                   int sourceRow,
                                   int count,
                                   const AbstractItem* destParent,
                                   int destRow) /* override */
{
	// Access is only on the main thread
	assert( std::this_thread::get_id() == commandManager_->ownerThreadId() );

	const auto commandId = getClassIdentifier<MoveItemDataCommand>();

	const auto commandArgs = definitionManager_->create<MoveItemDataCommandArgument>();
	commandArgs->setDirection(MoveItemDataCommandArgument::Direction::ROW);
	commandArgs->setModel( model );
	commandArgs->setStartParent(sourceParent);
	commandArgs->setStartPos(sourceRow);
	commandArgs->setCount( count );

	commandArgs->setEndPos(destRow);
	commandArgs->setEndParent(destParent);

	auto command = commandManager_->queueCommand( commandId, commandArgs );
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	assert(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}

bool ItemModelController::insertItem(CollectionModel& model,
                                     const Variant& key) /* override */
{
	// Access is only on the main thread
	assert(std::this_thread::get_id() == commandManager_->ownerThreadId());

	const auto commandId = getClassIdentifier<InsertDefaultCommand>();

	const auto commandArgs = definitionManager_->create<InsertDefaultCommandArgument>();
	commandArgs->setModel(model);
	commandArgs->setKey(key);

	auto command = commandManager_->queueCommand(commandId, commandArgs);
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

bool ItemModelController::insertItem(CollectionModel& model,
                                     const Variant& key,
                                     const Variant& data) /* override */
{
	// Access is only on the main thread
	assert(std::this_thread::get_id() == commandManager_->ownerThreadId());

	const auto commandId = getClassIdentifier<InsertItemCommand>();

	const auto commandArgs = definitionManager_->create<InsertItemCommandArgument>();
	commandArgs->setModel( model );
	commandArgs->setKey(key);
	commandArgs->setValue(data);

	auto command = commandManager_->queueCommand(commandId, commandArgs);
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

bool ItemModelController::removeItem(CollectionModel& model,
                                     const Variant& key) /* override */
{
	// Access is only on the main thread
	assert(std::this_thread::get_id() == commandManager_->ownerThreadId());

	const auto commandId = getClassIdentifier<RemoveItemCommand>();

	const auto commandArgs = definitionManager_->create<RemoveItemCommandArgument>();
	commandArgs->setModel( model );
	commandArgs->setKey(key);

	auto command = commandManager_->queueCommand(commandId, commandArgs);
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
