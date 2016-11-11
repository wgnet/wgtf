#include "command_instance.hpp"
#include "command.hpp"
#include "i_command_manager.hpp"

#include "core_data_model/collection_model.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_iterator.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "wg_types/binary_block.hpp"
#include "core_logging/logging.hpp"
#include "custom_undo_redo_data.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

namespace wgt
{
namespace RPURU = ReflectedPropertyUndoRedoUtility;

//==============================================================================
CommandInstance::CommandInstance()
    : defManager_(nullptr), status_(Complete), arguments_(nullptr), pCmdSysProvider_(nullptr), commandId_(""),
      contextObject_(nullptr), errorCode_(CommandErrorCode::COMMAND_NO_ERROR)
{
}

//==============================================================================
CommandInstance::~CommandInstance()
{
}

//==============================================================================
void CommandInstance::cancel()
{
}

//==============================================================================
void CommandInstance::waitForCompletion()
{
	std::unique_lock<std::mutex> lock(mutex_);

	while (!completeStatus_.wait_for(lock, std::chrono::milliseconds(1), [this] { return status_ == Complete; }))
	{
		lock.unlock();
		getCommand()->fireProgressMade(*this);
		lock.lock();
	}
}

//==============================================================================
CommandErrorCode CommandInstance::getErrorCode() const
{
	// Non-batch command
	if (errorCode_ != CommandErrorCode::BATCH_NO_ERROR)
	{
		return errorCode_;
	}

	// Empty batch command
	if (children_.empty())
	{
		return CommandErrorCode::ABORTED;
	}

	// Check children in batch command
	for (const auto& child : children_)
	{
		// A batch command is just a group of commands,
		// if one item in the group succeeded, then the batch performed an operation
		// so mark the whole batch as succeeded
		const auto childError = child->getErrorCode();
		if (wgt::isCommandSuccess(childError))
		{
			return childError;
		}
	}

	// Successful batch command
	return CommandErrorCode::BATCH_NO_ERROR;
}

//==============================================================================
bool CommandInstance::hasChildren() const
{
	return !children_.empty();
}

//==============================================================================
void CommandInstance::setArguments(const ObjectHandle& arguments)
{
	arguments_ = arguments;
}

//==============================================================================
void CommandInstance::setDefinitionManager(IDefinitionManager& defManager)
{
	defManager_ = &defManager;
}

//==============================================================================
const char* CommandInstance::getCommandId() const
{
	return commandId_.c_str();
}

//==============================================================================
void CommandInstance::setCommandId(const char* commandName)
{
	commandId_ = commandName;
}

//==============================================================================
Command* CommandInstance::getCommand()
{
	assert(pCmdSysProvider_ != nullptr);
	Command* pCommand = pCmdSysProvider_->findCommand(commandId_.c_str());
	assert(pCommand != nullptr);
	return pCommand;
}

//==============================================================================
const Command* CommandInstance::getCommand() const
{
	assert(pCmdSysProvider_ != nullptr);
	Command* pCommand = pCmdSysProvider_->findCommand(commandId_.c_str());
	assert(pCommand != nullptr);
	return pCommand;
}

//==============================================================================
void CommandInstance::setStatus(ExecutionStatus status)
{
	// Lock is required for CommandInstance::waitForCompletion()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		status_ = status;
	}
	getCommand()->fireCommandStatusChanged(*this);
	if (status == Complete)
	{
		completeStatus_.notify_all();
	}
}

//==============================================================================
void CommandInstance::undo()
{
	for (auto it = undoRedoData_.rbegin(); it != undoRedoData_.rend(); ++it)
	{
		(*it)->undo();
	}
	const Command* command = getCommand();
	command->fireCommandExecuted(*this, CommandOperation::UNDO);
}

//==============================================================================
void CommandInstance::redo()
{
	for (auto it = undoRedoData_.begin(); it != undoRedoData_.end(); ++it)
	{
		(*it)->redo();
	}
	const Command* command = getCommand();
	command->fireCommandExecuted(*this, CommandOperation::REDO);
}

//==============================================================================
void CommandInstance::execute()
{
	const Command* command = getCommand();
	if (command->customUndo())
	{
		returnValue_ = command->execute(arguments_);
		undoRedoData_.emplace_back(new CustomUndoRedoData(*this));
	}
	else
	{
		auto undoRedoData = new ReflectionUndoRedoData(*this);
		undoRedoData->connect();
		returnValue_ = command->execute(arguments_);
		undoRedoData->disconnect();
		undoRedoData_.emplace_back(undoRedoData);
	}
	command->fireCommandExecuted(*this, CommandOperation::EXECUTE);
	auto errorCode = returnValue_.getBase<CommandErrorCode>();
	if (errorCode != nullptr)
	{
		errorCode_ = *errorCode;
	}
	else
	{
		// Not returning a CommandErrorCode assumes the code is COMMAND_NO_ERROR.
		// @see Command::execute()
		errorCode_ = CommandErrorCode::COMMAND_NO_ERROR;
	}
}

//==============================================================================
bool CommandInstance::isComplete() const
{
	return status_ == Complete;
}

//==============================================================================
ExecutionStatus CommandInstance::getExecutionStatus() const
{
	return status_;
}

//==============================================================================
void CommandInstance::setContextObject(const ObjectHandle& contextObject)
{
	contextObject_ = contextObject;
}

//==============================================================================
ObjectHandle CommandInstance::getCommandDescription() const
{
	auto description = getCommand()->getCommandDescription(getArguments());
	if (description != nullptr)
	{
		return description;
	}

	if (undoRedoData_.empty())
	{
		return nullptr;
	}

	// Single command
	// OR
	// Batch command with one child
	// - getCommandDescription() returns the same as a single command
	const auto commandCount = undoRedoData_.size();
	if ((commandCount == 1) || (commandCount == 2))
	{
		return undoRedoData_.begin()->get()->getCommandDescription();
	}

	// Batch command with multiple children
	// This is expected to have a "Children" collection attached
	auto itr = undoRedoData_.begin();

	// First item is the batch itself
	auto batchHandle = itr->get()->getCommandDescription();
	auto pBatchDescription = batchHandle.getBase<GenericObject>();
	assert(pBatchDescription != nullptr);
	auto& genericObject = *pBatchDescription;

	// Get copy of children
	Collection childrenCollection;
	const auto getCollection = genericObject.get("Children", childrenCollection);
	assert(getCollection);

	// Latter items are children of the batch
	++itr;
	for (; itr != undoRedoData_.end(); ++itr)
	{
		auto childHandle = itr->get()->getCommandDescription();
		auto collectionItem = childrenCollection.insert(childrenCollection.size());
		const auto setValue = collectionItem.setValue(childHandle);
		assert(setValue);
	}

	// Copy back to batch
	const auto setCollection = genericObject.set("Children", childrenCollection);
	assert(setCollection);

	return batchHandle;
}

//==============================================================================
void CommandInstance::setCommandSystemProvider(ICommandManager* pCmdSysProvider)
{
	pCmdSysProvider_ = pCmdSysProvider;
}

//==============================================================================
void CommandInstance::consolidateUndoRedoData(CommandInstance* parentInstance)
{
	if (parentInstance == nullptr)
	{
		for (auto& data : undoRedoData_)
		{
			auto reflectionUndoRedoData = dynamic_cast<ReflectionUndoRedoData*>(data.get());
			if (reflectionUndoRedoData != nullptr)
			{
				reflectionUndoRedoData->consolidate();
			}
		}
		return;
	}

	auto lastParent = parentInstance->undoRedoData_.empty() ?
	nullptr :
	dynamic_cast<ReflectionUndoRedoData*>(parentInstance->undoRedoData_.back().get());
	if (lastParent != nullptr)
	{
		if (lastParent->undoRedoHelperList_.size() == 1 && lastParent->undoRedoHelperList_[0]->isMethod())
		{
			lastParent = nullptr;
		}
	}

	auto firstChild =
	undoRedoData_.empty() ? nullptr : dynamic_cast<ReflectionUndoRedoData*>(undoRedoData_.front().get());
	if (firstChild != nullptr)
	{
		if (firstChild->undoRedoHelperList_.size() == 1 && firstChild->undoRedoHelperList_[0]->isMethod())
		{
			firstChild = nullptr;
		}
	}

	if (lastParent != nullptr && firstChild != nullptr)
	{
		for (auto& childHelper : firstChild->undoRedoHelperList_)
		{
			auto it =
			std::find_if(lastParent->undoRedoHelperList_.begin(), lastParent->undoRedoHelperList_.end(),
			             [&childHelper](std::unique_ptr<RPURU::ReflectedClassMemberUndoRedoHelper>& item) {
				             return childHelper->objectId_ == item->objectId_ && childHelper->path_ == item->path_;
				         });
			if (it != lastParent->undoRedoHelperList_.end())
			{
				auto& helper = *it;
				assert(!helper->isMethod());
				assert(!childHelper->isMethod());
				auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());
				auto childPropertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(childHelper.get());
				propertyHelper->postValue_ = childPropertyHelper->postValue_;
			}
			else
			{
				assert(!childHelper->isMethod());
				lastParent->undoRedoHelperList_.emplace_back(childHelper.release());
			}
		}
		undoRedoData_.erase(undoRedoData_.begin());
	}

	for (auto& data : undoRedoData_)
	{
		parentInstance->undoRedoData_.emplace_back(data.release());
	}
	undoRedoData_.clear();
}
} // end namespace wgt
