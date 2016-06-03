#include "core_generic_plugin/generic_plugin.hpp"
#include "custom_model_interface_test.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/default_meta_type_manager.hpp"

namespace wgt
{
//==============================================================================
class TestDataModelPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TestDataModelPlugin( IComponentContext & contextManager )
		: customModelInterfaceTest( contextManager )
	{
	}

	//==========================================================================
	void Initialise(IComponentContext & contextManager) override
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		customModelInterfaceTest.initialise( contextManager );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager ) override
	{
		customModelInterfaceTest.fini( contextManager );
		return true;
	}

private:
	CustomModelInterfaceTest customModelInterfaceTest;
};

PLG_CALLBACK_FUNC( TestDataModelPlugin )
} // end namespace wgt
