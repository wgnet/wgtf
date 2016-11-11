#include "core_generic_plugin/generic_plugin.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model_cmds/private/commands/insert_default_command.hpp"
#include "core_data_model_cmds/private/commands/insert_item_command.hpp"
#include "core_data_model_cmds/private/commands/insert_rows_command.hpp"
#include "core_data_model_cmds/private/commands/move_item_data_command.hpp"
#include "core_data_model_cmds/private/commands/remove_item_command.hpp"
#include "core_data_model_cmds/private/commands/remove_rows_command.hpp"
#include "core_data_model_cmds/private/commands/set_item_data_command.hpp"
#include "core_data_model_cmds/private/commands/set_model_data_command.hpp"
#include "core_data_model_cmds/private/item_model_controller.hpp"
#include "core_data_model_cmds/private/item_model_types.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/invoke_reflected_method_command.hpp"
#include "core_reflection_utils/commands/reflected_collection_insert_command.hpp"
#include "core_reflection_utils/commands/reflected_collection_erase_command.hpp"
#include "core_reflection_utils/reflected_types.hpp"

namespace wgt
{
/**
* A plugin which allows UI code to communicate with reflected methods and variables.
* Required to add undo/redo history to reflected property panels and custom data models.
*
* @ingroup plugins
* @ingroup coreplugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class EditorInteractionPlugin : public PluginMain
{
private:
	std::vector<std::unique_ptr<Command>> commands_;
	IInterface* pItemModelController_;

public:
	//==========================================================================
	EditorInteractionPlugin(IComponentContext& contextManager) : pItemModelController_(nullptr)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager) override
	{
		pItemModelController_ = contextManager.registerInterface(new ItemModelController(contextManager));
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager) override
	{
		auto defManager = contextManager.queryInterface<IDefinitionManager>();
		if (defManager == nullptr)
		{
			return;
		}
		IDefinitionManager& definitionManager = *defManager;
		Reflection_Utils::initReflectedTypes(definitionManager);
		ItemModelCommands::registerTypes(definitionManager);

		auto commandSystemProvider = contextManager.queryInterface<ICommandManager>();
		if (!commandSystemProvider)
		{
			NGT_ERROR_MSG("Could not register data model commands\n");
			return;
		}

		commands_.emplace_back(new SetReflectedPropertyCommand(definitionManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new MoveItemDataCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new InsertDefaultCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new InsertItemCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new InsertRowsCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new RemoveItemCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new RemoveRowsCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new SetItemDataCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new SetModelDataCommand(contextManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new InvokeReflectedMethodCommand(definitionManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new ReflectedCollectionInsertCommand(definitionManager));
		commandSystemProvider->registerCommand(commands_.back().get());

		commands_.emplace_back(new ReflectedCollectionEraseCommand(definitionManager));
		commandSystemProvider->registerCommand(commands_.back().get());
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager) override
	{
		auto commandSystemProvider = contextManager.queryInterface<ICommandManager>();
		if (!commandSystemProvider)
		{
			return false;
		}

		for (auto& command : commands_)
		{
			commandSystemProvider->deregisterCommand(command->getId());
		}
		commands_.clear();
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager) override
	{
		contextManager.deregisterInterface(pItemModelController_);
		pItemModelController_ = nullptr;
	}
};

PLG_CALLBACK_FUNC(EditorInteractionPlugin)
} // end namespace wgt
