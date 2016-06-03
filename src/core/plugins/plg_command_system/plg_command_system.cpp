#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/ui_connection.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"

#include "core_command_system/command.hpp"
#include "core_command_system/command_system.hpp"
#include "core_command_system/command_manager.hpp"
#include "core_variant/variant.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/i_file_system.hpp"
#include <fstream>
#include "core_common/ngt_windows.hpp"
#include "core_command_system/env_system.hpp"

namespace wgt
{
class CommandSystemPlugin
	: public PluginMain
{
private:
	std::unique_ptr< CommandManager >						commandManager_;
	std::unique_ptr< EnvManager >								envManager_;

public:
	CommandSystemPlugin( IComponentContext & contextManager )
		: commandManager_( nullptr )
	{
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{
		IDefinitionManager * defManager = contextManager.queryInterface< IDefinitionManager >();
		if (defManager == nullptr)
		{
			return false;
		}
		CommandSystem::initReflectedTypes( *defManager );

		commandManager_.reset( new CommandManager( *defManager ) );
		assert( commandManager_ != NULL);
		if (commandManager_ == NULL)
		{
			return false;
		}
		types_.push_back(
			contextManager.registerInterface( commandManager_.get(), false ) );

		envManager_.reset( new EnvManager() );
		assert( envManager_ != NULL);
		if (envManager_ == NULL)
		{
			return false;
		}
		types_.push_back(
			contextManager.registerInterface( envManager_.get(), false ) );

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );

		IApplication * application = contextManager.queryInterface< IApplication >();
		assert( application != nullptr );
		IEnvManager * envManager = contextManager.queryInterface< IEnvManager >();
		assert( envManager != nullptr );
		IFileSystem * fileSystem = contextManager.queryInterface< IFileSystem >();
		assert(fileSystem != nullptr);
		IReflectionController * controller = contextManager.queryInterface< IReflectionController >();
		assert(controller != nullptr);
		commandManager_->init( *application, *envManager, fileSystem, controller );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		if(commandManager_ != nullptr)
		{
			commandManager_->fini();
		}
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type : types_ )
		{
			 contextManager.deregisterInterface( type );
		}
		commandManager_ = nullptr;
	}

	std::vector< IInterface * > types_;
};


PLG_CALLBACK_FUNC( CommandSystemPlugin )
} // end namespace wgt
