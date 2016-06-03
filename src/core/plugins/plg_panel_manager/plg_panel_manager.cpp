#include "panel_manager.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_variant/variant.hpp"
#include <vector>

namespace wgt
{
class PanelManagerPlugin
	: public PluginMain
{
public:

	PanelManagerPlugin( IComponentContext & contextManager )
		: panelManager_( nullptr )
	{
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{		
		auto definitionManager = 
			contextManager.queryInterface<IDefinitionManager>();
		assert( definitionManager != nullptr );
		
		panelManager_ = new PanelManager( contextManager );

		// Let the context manager handle the lifetime of these instances
		types_.push_back( contextManager.registerInterface( 
			panelManager_ ) );

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{		
 		Variant::setMetaTypeManager( 
 			contextManager.queryInterface< IMetaTypeManager >() );
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}

	void Unload( IComponentContext & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
	}

private:

	std::vector< IInterface * > types_;
	PanelManager* panelManager_;
};

PLG_CALLBACK_FUNC( PanelManagerPlugin )
} // end namespace wgt
