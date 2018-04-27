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

#include "core_common/assert.hpp"
#include "core_command_system/i_command_manager.hpp"

namespace wgt
{
struct ItemModelController::Impl : public Depends<ICommandManager>
{
	bool insertRows(AbstractItemModel& model, int startPos, InsertRowsCommandArgument::Type type, int count,
	                const AbstractItem* pParent)
	{
		auto commandManager = get<ICommandManager>();

		// Access is only on the main thread
		TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

        auto commandArgs = ManagedObject<InsertRowsCommandArgument>::make_iunique_fn(
            [&model, startPos, type, count, pParent](auto& commandArgs)
        {
            commandArgs.setModel(model);
            commandArgs.setStartPos(startPos, type);
            commandArgs.setCount(count);
            commandArgs.setParent(pParent);
        });

        const auto commandId = getClassIdentifier<InsertRowsCommand>();
		auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
		// Failed to queue
		if (command == nullptr)
		{
			return false;
		}

		// The thread affinity of SetReferenceCommand should cause
		// queueCommand() to execute the command immediately
		TF_ASSERT(command->isComplete());

		// Completion is required to get the return value
		return wgt::isCommandSuccess(command->getErrorCode());
}

bool removeRows(AbstractItemModel& model, int startPos, RemoveRowsCommandArgument::Type type, int count,
	            const AbstractItem* pParent)
{
	auto commandManager = get<ICommandManager>();

	// Access is only on the main thread
	TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

    auto commandArgs = ManagedObject<RemoveRowsCommandArgument>::make_iunique_fn(
        [&model, startPos, type, count, pParent](auto& commandArgs)
    {
        commandArgs.setModel(model);
        commandArgs.setStartPos(startPos, type);
        commandArgs.setCount(count);
        commandArgs.setParent(pParent);
    });

	const auto commandId = getClassIdentifier<RemoveRowsCommand>();
	auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	TF_ASSERT(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}
};

ItemModelController::ItemModelController() : impl_(new Impl())
{
}

ItemModelController::~ItemModelController()
{
}

bool ItemModelController::setValue(AbstractItemModel& model, const AbstractItemModel::ItemIndex& index, size_t roleId,
                                   const Variant& data) /* override */
{
	auto commandManager = impl_->get<ICommandManager>();

	// Access is only on the main thread
	TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

    auto commandArgs = ManagedObject<SetItemDataCommandArgument>::make_iunique_fn(
        [&model, &data, index, roleId](auto& commandArgs)
    {
        commandArgs.setModel(model);
        commandArgs.setIndex(index);
        commandArgs.setValue(roleId, data);
    });

	const auto commandId = getClassIdentifier<SetItemDataCommand>();
	auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	TF_ASSERT(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}

bool ItemModelController::setModelData(AbstractItemModel& model, int row, int column, size_t roleId,
                                       const Variant& data) /* override */
{
	auto commandManager = impl_->get<ICommandManager>();

	// Access is only on the main thread
	TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

    auto commandArgs = ManagedObject<SetModelDataCommandArgument>::make_iunique_fn(
        [&model, &data, row, column, roleId](auto& commandArgs)
    {
        commandArgs.setModel(model);
        commandArgs.setIndex(row, column);
        commandArgs.setValue(roleId, data);
    });

	const auto commandId = getClassIdentifier<SetModelDataCommand>();
	auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	TF_ASSERT(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}

bool ItemModelController::insertRows(AbstractItemModel& model, int row, int count,
                                     const AbstractItem* pParent) /* override */
{
	return impl_->insertRows(model, row, InsertRowsCommandArgument::Type::ROW, count, pParent);
}

bool ItemModelController::insertColumns(AbstractItemModel& model, int column, int count,
                                        const AbstractItem* pParent) /* override */
{
	return impl_->insertRows(model, column, InsertRowsCommandArgument::Type::COLUMN, count, pParent);
}

bool ItemModelController::removeRows(AbstractItemModel& model, int row, int count,
                                     const AbstractItem* pParent) /* override */
{
	return impl_->removeRows(model, row, RemoveRowsCommandArgument::Type::ROW, count, pParent);
}

bool ItemModelController::removeColumns(AbstractItemModel& model, int column, int count,
                                        const AbstractItem* pParent) /* override */
{
	return impl_->removeRows(model, column, RemoveRowsCommandArgument::Type::COLUMN, count, pParent);
}

bool ItemModelController::moveRows(AbstractItemModel& model, const AbstractItem* sourceParent, int sourceRow, int count,
                                   const AbstractItem* destParent, int destRow) /* override */
{
	auto commandManager = impl_->get<ICommandManager>();

	// Access is only on the main thread
	TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

    auto commandArgs = ManagedObject<MoveItemDataCommandArgument>::make_iunique_fn(
        [&model, sourceParent, sourceRow, count, destRow, destParent](auto& commandArgs)
    {
        commandArgs.setDirection(MoveItemDataCommandArgument::Direction::ROW);
        commandArgs.setModel(model);
        commandArgs.setStartParent(sourceParent);
        commandArgs.setStartPos(sourceRow);
        commandArgs.setCount(count);
        commandArgs.setEndPos(destRow);
        commandArgs.setEndParent(destParent);
    });

	const auto commandId = getClassIdentifier<MoveItemDataCommand>();
	auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	TF_ASSERT(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}

bool ItemModelController::insertItem(CollectionModel& model, const Variant& key) /* override */
{
	auto commandManager = impl_->get<ICommandManager>();

	// Access is only on the main thread
	TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

    auto commandArgs = ManagedObject<InsertDefaultCommandArgument>::make_iunique_fn(
        [&model, &key](auto& commandArgs)
    {
        commandArgs.setModel(model);
        commandArgs.setKey(key);
    });

	const auto commandId = getClassIdentifier<InsertDefaultCommand>();
	auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	TF_ASSERT(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}

bool ItemModelController::insertItem(CollectionModel& model, const Variant& key, const Variant& data) /* override */
{
	auto commandManager = impl_->get<ICommandManager>();

	// Access is only on the main thread
	TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

    auto commandArgs = ManagedObject<InsertItemCommandArgument>::make_iunique_fn(
        [&model, &key, &data](auto& commandArgs)
    {
        commandArgs.setModel(model);
        commandArgs.setKey(key);
        commandArgs.setValue(data);
    });

	const auto commandId = getClassIdentifier<InsertItemCommand>();
	auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	TF_ASSERT(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}

bool ItemModelController::removeItem(CollectionModel& model, const Variant& key) /* override */
{
	auto commandManager = impl_->get<ICommandManager>();

	// Access is only on the main thread
	TF_ASSERT(std::this_thread::get_id() == commandManager->ownerThreadId());

    auto commandArgs = ManagedObject<RemoveItemCommandArgument>::make_iunique_fn(
        [&model, &key](auto& commandArgs)
    {
        commandArgs.setModel(model);
        commandArgs.setKey(key);
    });

	const auto commandId = getClassIdentifier<RemoveItemCommand>();
	auto command = commandManager->queueCommand(commandId, std::move(commandArgs));
	// Failed to queue
	if (command == nullptr)
	{
		return false;
	}

	// The thread affinity of SetReferenceCommand should cause
	// queueCommand() to execute the command immediately
	TF_ASSERT(command->isComplete());

	// Completion is required to get the return value
	return wgt::isCommandSuccess(command->getErrorCode());
}

} // end namespace wgt
