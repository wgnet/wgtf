#include "compound_command.hpp"

#include "core_common/assert.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "command_instance.hpp"
#include "batch_command.hpp"
#include "core_reflection/i_definition_manager.hpp"

//==============================================================================
namespace wgt
{
CompoundCommand::CompoundCommand(const char* id) : id_(id), name_(id)
{
	subCommands_ = Collection(subCommandHandles_);
}

//==============================================================================
CompoundCommand::~CompoundCommand()
{
	subCommandStorage_.clear();
	subCommandHandles_.clear();
}

//==============================================================================
const char* CompoundCommand::getId() const
{
	return id_.c_str();
}

//==============================================================================
const char* CompoundCommand::getName() const
{
	return name_.c_str();
}

//==============================================================================
bool CompoundCommand::customUndo() const
{
	return true;
}

//==============================================================================
void CompoundCommand::addCommand(const char* commandId, const ObjectHandle& commandArguments)
{
	ICommandManager* manager = getCommandSystemProvider();
	TF_ASSERT(manager != nullptr);

	if (manager == nullptr)
	{
		return;
	}
		
	auto command = manager->findCommand(commandId);
	auto arguments = command->copyArguments(commandArguments);
	auto argumentsHandle = arguments ? arguments->getHandle() : ObjectHandle();
	subCommandHandles_.emplace_back(commandId, argumentsHandle);
	subCommandStorage_.emplace_back(commandId, std::move(arguments));
}

//==============================================================================
bool CompoundCommand::validateArguments(const ObjectHandle& arguments) const
{
	ICommandManager* cmdSysProvider = getCommandSystemProvider();
	if (cmdSysProvider == nullptr)
	{
		return false;
	}
	return true;
}

//==============================================================================
Variant CompoundCommand::execute(const ObjectHandle& arguments) const
{
	auto cmdSysProvider = getCommandSystemProvider();
	TF_ASSERT(cmdSysProvider != nullptr);

	subInstances_.clear();
	subInstances_.reserve(subCommandHandles_.size());

	for (SubCommandHandles::size_type i = 0; i < subCommandHandles_.size(); ++i)
	{
		auto& subCommand = subCommandHandles_[i];
		auto instance = cmdSysProvider->queueCommand(subCommand.first.c_str(), subCommand.second);
		TF_ASSERT(instance != nullptr);
		cmdSysProvider->waitForInstance(instance);
		auto errorCode = instance->getErrorCode();
		if (errorCode != CommandErrorCode::COMMAND_NO_ERROR)
		{
			return errorCode;
		}
		subInstances_.push_back(instance);
	}

	return CommandErrorCode::COMMAND_NO_ERROR;
}

//==============================================================================
CommandThreadAffinity CompoundCommand::threadAffinity() const
{
	return CommandThreadAffinity::ANY_THREAD;
}

//==============================================================================
bool CompoundCommand::undo(const ObjectHandle& arguments) const
{
	for (auto& ins : subInstances_)
	{
		ins->undo();
	}
	return true;
}

//==============================================================================
bool CompoundCommand::redo(const ObjectHandle& arguments) const
{
	for (auto& ins : subInstances_)
	{
		ins->redo();
	}
	return true;
}

//==============================================================================
void CompoundCommand::setName(const char* name)
{
	name_ = name;
}

//==============================================================================
const Collection& CompoundCommand::getSubCommands() const
{
	return subCommands_;
}

ManagedObjectPtr CompoundCommand::copyArguments(const ObjectHandle& arguments) const
{
	return nullptr;
}
} // end namespace wgt
