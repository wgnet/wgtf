#include "command_instance.hpp"
#include "command.hpp"
#include "i_command_manager.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_object/i_managed_object.hpp"
#include "core_reflection/generic/generic_object.hpp"
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
#include "batch_command.hpp"

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
void CommandInstance::setArguments(const std::nullptr_t&)
{
    arguments_ = nullptr;
    argumentsStorage_ = nullptr;
}

//==============================================================================
void CommandInstance::setArguments(const ObjectHandle& arguments)
{
	arguments_ = arguments;
	argumentsStorage_ = nullptr;
}

//==============================================================================
void CommandInstance::setArguments(ManagedObjectPtr arguments)
{
    argumentsStorage_ = std::move(arguments);
    arguments_ = argumentsStorage_->getHandle();
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
	TF_ASSERT(pCmdSysProvider_ != nullptr);
	Command* pCommand = pCmdSysProvider_->findCommand(commandId_.c_str());
	TF_ASSERT(pCommand != nullptr);
	return pCommand;
}

//==============================================================================
const Command* CommandInstance::getCommand() const
{
	TF_ASSERT(pCmdSysProvider_ != nullptr);
	Command* pCommand = pCmdSysProvider_->findCommand(commandId_.c_str());
	TF_ASSERT(pCommand != nullptr);
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
bool CommandInstance::undo()
{
	bool returnValue = true;
	for (auto it = undoRedoData_.rbegin(); it != undoRedoData_.rend(); ++it)
	{
		if(!(*it)->undo())
		{
			returnValue = false;
			break;
		}
	}
	const Command* command = getCommand();
	command->fireCommandExecuted(*this, CommandOperation::UNDO);
	return returnValue;
}

//==============================================================================
bool CommandInstance::redo()
{
	bool returnValue = true;
	for (auto it = undoRedoData_.begin(); it != undoRedoData_.end(); ++it)
	{
		if (!(*it)->redo())
		{
			returnValue = false;
			break;
		}
	}
	const Command* command = getCommand();
	command->fireCommandExecuted(*this, CommandOperation::REDO);
	return returnValue;
}

//==============================================================================
void CommandInstance::execute()
{
	const Command* command = getCommand();
	Variant result;
	if (command->customUndo())
	{
		result = command->execute(arguments_);
		undoRedoData_.emplace_back(new CustomUndoRedoData(*this));
	}
	else
	{
		auto undoRedoData = new ReflectionUndoRedoData(*this);
		undoRedoData->connect();
		result = command->execute(arguments_);
		undoRedoData->disconnect();
		undoRedoData_.emplace_back(undoRedoData);
	}
	command->fireCommandExecuted(*this, CommandOperation::EXECUTE);

	if (!result.tryCast<CommandErrorCode>(errorCode_))
	{
		// Not returning a CommandErrorCode assumes the code is COMMAND_NO_ERROR.
		// @see Command::execute()
        errorCode_ = CommandErrorCode::COMMAND_NO_ERROR;
		returnValue_ = result;
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
Collection CommandInstance::getChildren() const
{
	return Collection(children_);
}

//==============================================================================
ObjectHandle CommandInstance::setCommandDescription(CommandDescription description) const
{
    if (description != nullptr)
    {
        if (description_ == nullptr)
        {
            description_ = GenericObject::create();
        }
        *description_ = description.getHandleT();
        return description_.getHandle();
    }
    return nullptr;
}

//==============================================================================
ObjectHandle CommandInstance::getCommandDescription() const
{
	auto command = this->getCommand();

	auto description = command->getCommandDescription(getArguments());
	if (description != nullptr)
	{
        return setCommandDescription(std::move(description));
	}

	if (undoRedoData_.empty())
	{
		return nullptr;
	}

	// Single command
	// OR
	// Batch command with one child
	// OR
	// Macro command
	// - getCommandDescription() returns the same as a single command

	const auto commandCount = undoRedoData_.size();
	auto batchCommand = dynamic_cast<const BatchCommand*>(command);
	if ((commandCount == 1) || (commandCount == 2))
	{
        return setCommandDescription(undoRedoData_.begin()->get()->getCommandDescription());
	}

	// Batch command with multiple children
	// This is expected to have a "Children" collection attached
	auto itr = undoRedoData_.begin();

	// First item is the batch itself
	auto batchHandle = itr->get()->getCommandDescription();
    auto pBatchDescription = batchHandle.getHandleT();
	TF_ASSERT(pBatchDescription != nullptr);

	// Get copy of children
    typedef std::vector<ManagedObjectPtr> ContainerType;
	Collection childrenCollection;
	const bool success = pBatchDescription->get("Children", childrenCollection);
    auto container = childrenCollection.container<ContainerType>();
    if (!success || !container)
    {
        TF_ASSERT(!"Children must be in container of Managed Objects");
        return nullptr;
    }

	// Latter items are children of the batch
    ++itr;
    for (; itr != undoRedoData_.end(); ++itr)
    {
        auto childObject = itr->get()->getCommandDescription();
        container->push_back(std::make_unique<ManagedObject<GenericObject>>(std::move(childObject)));
    }

	// Copy back to batch
	const auto setCollection = pBatchDescription->set("Children", childrenCollection);
	TF_ASSERT(setCollection);

    return setCommandDescription(std::move(batchHandle));
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
				TF_ASSERT(!helper->isMethod());
				TF_ASSERT(!childHelper->isMethod());
				auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());
				auto childPropertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(childHelper.get());
				propertyHelper->postValue_ = childPropertyHelper->postValue_;
			}
			else
			{
				TF_ASSERT(!childHelper->isMethod());
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

//==============================================================================
void CommandInstance::consolidateChildren()
{
	auto commands_compressible = [](const CommandInstancePtr& command_a, const CommandInstancePtr& command_b) {
		auto command_a_type_ok =
		strcmp(command_a->getCommandId(), getClassIdentifier<SetReflectedPropertyCommand>()) == 0;
		auto command_b_type_ok =
		strcmp(command_b->getCommandId(), getClassIdentifier<SetReflectedPropertyCommand>()) == 0;
		auto command_a_argument = command_a->getArguments().getBase<ReflectedPropertyCommandArgument>();
		auto command_b_argument = command_b->getArguments().getBase<ReflectedPropertyCommandArgument>();

		bool commands_has_correct_type =
		command_a_type_ok && command_b_type_ok && command_a_argument && command_b_argument;
		return commands_has_correct_type &&
		(command_a_argument->getContextId() == command_b_argument->getContextId()) &&
		(command_a_argument->getContextId() == command_b_argument->getContextId());
	};

	size_t j = 0;
	for (size_t i = 0; i < children_.size(); ++i)
	{
		if (commands_compressible(children_[j], children_[i]) || children_[i] == children_[j])
		{
			children_[j] = children_[i];
		}
		else
		{
			children_[++j] = children_[i];
		}
	}

	children_.resize(std::min(children_.size(), j + 1));
}

} // end namespace wgt
