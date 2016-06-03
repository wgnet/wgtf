#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "test_ui/test_ui.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "test_datasource.hpp"
#include "core_variant/variant.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"


#include "pages/metadata/test_page.mpp"
#include "pages/metadata/test_polymorphism.mpp"

#include <vector>


namespace wgt
{
//==============================================================================
class MainUITestPlugin
	: public PluginMain
{
private:
	TestUI testUI_;
	std::unique_ptr< TestDataSourceManager > dataSrc_;
	std::vector<IInterface*> types_;
public:
	//==========================================================================
	MainUITestPlugin(IComponentContext & contextManager )
		: testUI_( contextManager )
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		// register test data source
		dataSrc_.reset( new TestDataSourceManager() );
		types_.push_back( contextManager.registerInterface( dataSrc_.get(), false ) );

		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );
		// register reflected type definition
		IDefinitionManager* defManager =
			contextManager.queryInterface< IDefinitionManager >();
		assert(defManager != nullptr);

		this->initReflectedTypes( *defManager );

		dataSrc_->init( contextManager );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();

		testUI_.init( *uiApplication, *uiFramework );
	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		testUI_.fini();

		assert( dataSrc_ );
		dataSrc_->fini();

		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
		dataSrc_ = nullptr;
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
		REGISTER_DEFINITION( TestPolyCheckBox )
		REGISTER_DEFINITION( TestPolyTextField )
		REGISTER_DEFINITION( TestPolyComboBox )
		REGISTER_DEFINITION( TestPolyColor3 )
		REGISTER_DEFINITION( TestPage )
		REGISTER_DEFINITION( TestPage2 )
	}

};


PLG_CALLBACK_FUNC( MainUITestPlugin )
} // end namespace wgt
