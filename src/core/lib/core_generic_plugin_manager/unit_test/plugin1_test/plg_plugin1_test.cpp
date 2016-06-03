#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/reflection_macros.hpp"

#include "plugin_objects.hpp"


namespace wgt
{
class TestPlugin1
	: public PluginMain
{
public:
	TestPlugin1( IComponentContext & contextManager )
		: PluginMain()
	{
	}

	bool PostLoad( IComponentContext & contextManager )
	{
		IObjectManager * pObjectManager = contextManager.queryInterface< IObjectManager >();
		if (pObjectManager == nullptr)
		{
			return false;
		}
		IDefinitionManager * pDefinitionManager =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			return false;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION( TestPlugin1TestObject );
		REGISTER_DEFINITION( TestPlugin1Interface );

		// Create plugin interface
		std::vector< RefObjectId > objects;
		pObjectManager->getContextObjects( &definitionManager, 
			objects );

		// Search for the plugin interface
		for (std::vector< RefObjectId >::iterator it = objects.begin(), 
			end = objects.end(); it != end; ++it)
		{
			auto pObj = pObjectManager->getObject( *it );
			assert( pObj != nullptr );
			if (pObj.getDefinition( *pDefinitionManager ) == 
				definitionManager.getDefinition< TestPlugin1Interface >())
			{
				pInterface_ = safeCast< TestPlugin1Interface >( pObj );
				return true;
			}
		}

		// create new interface object
		pInterface_ = definitionManager.create< TestPlugin1Interface >();
		assert( pInterface_ != nullptr );

		// Do not delete when interface is unregistered
		// Reflection system needs to delete it
		types_.push_back( 
			contextManager.registerInterface( pInterface_.get(), false /*transferOwnership*/ ) );

		return true;
	}

	void Initialise( IComponentContext & contextManager )
	{
		assert( pInterface_ != nullptr );
		auto defManager = 
			contextManager.queryInterface< IDefinitionManager >();
		assert( defManager != nullptr );

		ITestPlugin2 * plugin2 = 
			contextManager.queryInterface< ITestPlugin2 >();
		if (plugin2 != nullptr)
		{
			pInterface_->setObjectFromPlugin2( plugin2->getObject( *defManager ) );
		}
	}


	bool Finalise( IComponentContext & contextManager )
	{
		if( pInterface_ != nullptr )
		{
			pInterface_->setObjectFromPlugin2( nullptr );
		}
		return true;
	}

	void Unload( IComponentContext & contextManager )
	{
		for( auto type : types_ )
		{
			contextManager.deregisterInterface( type );
		}
		pInterface_ = nullptr;
	}

private:
	TestPlugin1InterfacePtr pInterface_;
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC( TestPlugin1 )
} // end namespace wgt
