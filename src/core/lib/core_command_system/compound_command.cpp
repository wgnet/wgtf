#include "compound_command.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "macro_object.hpp"
#include "command_instance.hpp"
#include "batch_command.hpp"
#include "core_reflection/i_definition_manager.hpp"

//==============================================================================
namespace wgt
{
CompoundCommand::CompoundCommand() : id_(""), macroObject_(nullptr)
{
}

//==============================================================================
CompoundCommand::~CompoundCommand()
{
	subCommands_.clear();
}

//==============================================================================
void CompoundCommand::initDisplayData(IDefinitionManager& defManager, IReflectionController* controller)
{
	auto cmdSysProvider = getCommandSystemProvider();
	assert(cmdSysProvider != nullptr);
	const auto pDefinition = defManager.getDefinition(getClassIdentifier<MacroObject>());
	assert(pDefinition != nullptr);

	macroObject_ = defManager.create<MacroObject>(false);
	macroObject_->init(*cmdSysProvider, defManager, controller, id_.c_str());
}

//==============================================================================
const char* CompoundCommand::getId() const
{
	return id_.c_str();
}

//==============================================================================
void CompoundCommand::addCommand(const char* commandId, const ObjectHandle& commandArguments)
{
	subCommands_.emplace_back(commandId, commandArguments);
}

//==============================================================================
bool CompoundCommand::validateArguments(const ObjectHandle& arguments) const
{
	ICommandManager* cmdSysProvider = getCommandSystemProvider();
	if (cmdSysProvider == nullptr)
	{
		return false;
	}

	MacroEditObject* ccArgs = arguments.getBase<MacroEditObject>();
	if (ccArgs == nullptr)
	{
		return false;
	}
	if (ccArgs->getArgCount() != subCommands_.size())
	{
		return false;
	}

	for (SubCommandCollection::size_type i = 0; i < subCommands_.size(); ++i)
	{
		Command* command = cmdSysProvider->findCommand(subCommands_[i].first.c_str());

		if (command == nullptr)
		{
			return false;
		}
		if (!command->validateArguments(ccArgs->getCommandArgument(i)))
		{
			return false;
		}
	}

	return true;
}

//==============================================================================
ObjectHandle CompoundCommand::execute(const ObjectHandle& arguments) const
{
	auto cmdSysProvider = getCommandSystemProvider();
	assert(cmdSysProvider != nullptr);
	MacroEditObject* ccArgs = arguments.getBase<MacroEditObject>();
	assert(ccArgs);

	std::vector<CommandInstance*> subInstances;
	subInstances.reserve(subCommands_.size());
	CommandInstancePtr instance;

	for (SubCommandCollection::size_type i = 0; i < subCommands_.size(); ++i)
	{
		ccArgs->resolveDependecy(i, subInstances);
		instance = cmdSysProvider->queueCommand(subCommands_[i].first.c_str(), ccArgs->getCommandArgument(i));
		assert(instance != nullptr);
		cmdSysProvider->waitForInstance(instance);
		subInstances.push_back(instance.get());
	}

	return instance->getReturnValue();
}

//==============================================================================
CommandThreadAffinity CompoundCommand::threadAffinity() const
{
	return CommandThreadAffinity::ANY_THREAD;
}

//==============================================================================
ObjectHandle CompoundCommand::getMacroObject() const
{
	return macroObject_;
}

//==============================================================================
void CompoundCommand::setId(const char* id)
{
	id_ = id;
}

//==============================================================================
const CompoundCommand::SubCommandCollection& CompoundCommand::getSubCommands() const
{
	return subCommands_;
}

void CompoundCommand::serialize(ISerializer& serializer) const
{
	serializer.serialize(getId());
	serializer.serialize(getSubCommands().size());
	for (auto& c : getSubCommands())
	{
		serializer.serialize(c.first);
	}
	getMacroObject().getBase<MacroObject>()->serialize(serializer);
}

void CompoundCommand::deserialize(ISerializer& serializer)
{
	size_t size = 0;
	serializer.deserialize(size);

	std::string id;
	for (size_t i = 0; i < size; ++i)
	{
		serializer.deserialize(id);
		addCommand(id.c_str(), ObjectHandle());
	}
	getMacroObject().getBase<MacroObject>()->deserialize(serializer);
}
} // end namespace wgt
