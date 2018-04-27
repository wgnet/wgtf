#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/ui_connection.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"

#include "core_common/assert.hpp"
#include "core_command_system/command.hpp"
#include "core_command_system/command_manager.hpp"
#include "core_environment_system/i_env_system.hpp"
#include "core_variant/variant.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/i_file_system.hpp"
#include <fstream>
#include "core_common/ngt_windows.hpp"

#include "core_command_system/reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

namespace wgt
{
/**
* A plugin which registers ICommandManager and IEnvManager interfaces to allow other plugins to queue and register
* commands.
* It maintains and processes a command queue and tracks the history of all executed commands coordinating undo/redo
* operations.
*
* @ingroup plugins
* @ingroup coreplugins
* @note The command queue is important for concurrency. The command manager will perform all data modifications on the
* same thread.
*       All data changes should be wrapped in a command and executed via the command manager to guarantee there are no
* data races.
*       Requires Plugins:
*       - @ref coreplugins
*/
class CommandSystemPlugin : public PluginMain
{
private:
	CommandManager * commandManager_;
public:
	CommandSystemPlugin(IComponentContext& contextManager)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
		registerCallback(
			[ &contextManager, this ](IEnvManager & envManager )
		{
			commandManager_ = new CommandManager(envManager );
			contextManager.registerInterface(commandManager_);
		}, [this](IEnvManager & envManager)
		{
			commandManager_->fini();
		});

		registerCallback(
			[this](
				ICommandManager & commandManager, IApplication & application, IDefinitionManager & defManager)
		{
			TF_ASSERT( &commandManager == commandManager_ );
			dynamic_cast< CommandManager &>( commandManager ).init(application, defManager );
		});
	}
};

PLG_CALLBACK_FUNC(CommandSystemPlugin)
} // end namespace wgt
