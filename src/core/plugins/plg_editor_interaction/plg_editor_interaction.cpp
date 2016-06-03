#include "core_generic_plugin/generic_plugin.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/invoke_reflected_method_command.hpp"
#include "core_reflection_utils/commands/reflected_collection_insert_command.hpp"
#include "core_reflection_utils/commands/reflected_collection_erase_command.hpp"
#include "core_reflection_utils/reflected_types.hpp"

namespace wgt
{
//==============================================================================
class EditorInteractionPlugin
	: public PluginMain
{
private:
	std::unique_ptr< SetReflectedPropertyCommand > setReflectedPropertyCmd_;
	std::unique_ptr< InvokeReflectedMethodCommand > invokeReflectedMethodCommand_;
	std::unique_ptr< ReflectedCollectionInsertCommand > reflectedCollectionInsertCommand_;
	std::unique_ptr< ReflectedCollectionEraseCommand > reflectedCollectionEraseCommand_;

public:
	//==========================================================================
	EditorInteractionPlugin( IComponentContext & contextManager )
	{
		
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager ) override
	{
		return true;
	}


	//==========================================================================
	void Initialise( IComponentContext & contextManager ) override
	{
		auto metaTypeMgr = contextManager.queryInterface<IMetaTypeManager>();
		assert( metaTypeMgr != nullptr );
		Variant::setMetaTypeManager( metaTypeMgr );

		auto defManager = contextManager.queryInterface< IDefinitionManager >();
		if (defManager == nullptr)
		{
			return;
		}
		IDefinitionManager & definitionManager = *defManager;
		Reflection_Utils::initReflectedTypes( definitionManager );

		auto commandSystemProvider = contextManager.queryInterface< ICommandManager >();
		if (commandSystemProvider)
		{
			setReflectedPropertyCmd_.reset( new SetReflectedPropertyCommand( definitionManager ) );
			commandSystemProvider->registerCommand( setReflectedPropertyCmd_.get() );

			invokeReflectedMethodCommand_.reset( new InvokeReflectedMethodCommand( definitionManager ) );
			commandSystemProvider->registerCommand( invokeReflectedMethodCommand_.get() );

			reflectedCollectionInsertCommand_.reset( new ReflectedCollectionInsertCommand( definitionManager ) );
			commandSystemProvider->registerCommand( reflectedCollectionInsertCommand_.get() );

			reflectedCollectionEraseCommand_.reset( new ReflectedCollectionEraseCommand( definitionManager ) );
			commandSystemProvider->registerCommand( reflectedCollectionEraseCommand_.get() );
		}
	}


	//==========================================================================
	bool Finalise(IComponentContext & contextManager) override
	{
		auto commandSystemProvider = contextManager.queryInterface< ICommandManager >();
		if (commandSystemProvider)
		{
			commandSystemProvider->deregisterCommand( setReflectedPropertyCmd_->getId() );
			setReflectedPropertyCmd_ = nullptr;

			commandSystemProvider->deregisterCommand( invokeReflectedMethodCommand_->getId() );
			invokeReflectedMethodCommand_ = nullptr;

			commandSystemProvider->deregisterCommand( reflectedCollectionInsertCommand_->getId() );
			reflectedCollectionInsertCommand_ = nullptr;

			commandSystemProvider->deregisterCommand( reflectedCollectionEraseCommand_->getId() );
			reflectedCollectionEraseCommand_ = nullptr;
		}

		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager ) override
	{
	}
};

PLG_CALLBACK_FUNC( EditorInteractionPlugin )
} // end namespace wgt
