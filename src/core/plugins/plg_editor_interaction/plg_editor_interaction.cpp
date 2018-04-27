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
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/invoke_reflected_method_command.hpp"
#include "core_reflection_utils/commands/reflected_collection_insert_command.hpp"
#include "core_reflection_utils/commands/reflected_collection_erase_command.hpp"
#include "core_reflection_utils/reflection_controller.hpp"
#include "core_reflection_utils/commands/custom_command.hpp"
#include "core/interfaces/editor/i_editor.hpp"
#include "core_reflection_utils/reflection_auto_reg.mpp"
#include "core_data_model_cmds/reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include "private/background_worker.hpp"

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
	std::vector<std::unique_ptr<Command>> reflectedCommands_;
	InterfacePtrs types_;

public:
	//==========================================================================
	EditorInteractionPlugin(IComponentContext& contextManager)
	{
		types_.emplace_back(contextManager.registerInterface(new BackgroundWorker));
		auto controller = new ReflectionController;
		types_.emplace_back(contextManager.registerInterface(controller));
		types_.emplace_back(contextManager.registerInterface(new ItemModelController));
		registerCallback(
			[this](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
		registerCallback(
			[this](ICommandManager & commandManager, IDefinitionManager & defManager)
		{
			reflectedCommands_.emplace_back(
				new SetReflectedPropertyCommand( defManager ) );
			commandManager.registerCommand(reflectedCommands_.back().get());

			reflectedCommands_.emplace_back(
				new InvokeReflectedMethodCommand( defManager ));
			commandManager.registerCommand(reflectedCommands_.back().get());

			reflectedCommands_.emplace_back(
				new ReflectedCollectionInsertCommand( defManager ));
			commandManager.registerCommand(reflectedCommands_.back().get());

			reflectedCommands_.emplace_back(
				new ReflectedCollectionEraseCommand( defManager ));
			commandManager.registerCommand(reflectedCommands_.back().get());
		}, [this](ICommandManager & commandManager, IDefinitionManager & defManager)
		{
			for (auto& command : reflectedCommands_)
			{
				commandManager.deregisterCommand(command->getId());
			}
			reflectedCommands_.clear();
		});
		registerCallback(
			[ this, controller](ICommandManager & commandManager)
		{
			commands_.emplace_back(new MoveItemDataCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new InsertDefaultCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new InsertItemCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new InsertRowsCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new RemoveItemCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new RemoveRowsCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new SetItemDataCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new SetModelDataCommand);
			commandManager.registerCommand(commands_.back().get());

			commands_.emplace_back(new CustomCommand());
			commandManager.registerCommand(commands_.back().get());

			controller->init( commandManager );
		},
			[ this, controller ](ICommandManager & commandManager)
		{
			for (auto& command : commands_)
			{
				commandManager.deregisterCommand(command->getId());
			}
			commands_.clear();
			controller->fini();
		});
		registerCallback([controller](IEditor& editor)
		{
			controller->setEditor(&editor);
		},
		[controller](IEditor& editor)
		{
			controller->setEditor(nullptr);
		});
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}
};

PLG_CALLBACK_FUNC(EditorInteractionPlugin)
} // end namespace wgt
