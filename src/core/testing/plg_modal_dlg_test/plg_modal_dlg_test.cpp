#include "core_dependency_system/i_interface.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include <vector>


namespace wgt
{
/**
* A plugin which creates a button which when pressed creates a dialog window with Ok and Cancel buttons
*
* @ingroup plugins
* @image html plg_modal_dlg_test.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ModalDlgTestPlugin
	: public PluginMain
	, public Depends< IViewCreator >
{
private:
	std::unique_ptr< IAction > testModalDialog_;
	std::unique_ptr< IWindow > modalDialog_;

	void showModalDialog( IAction * action )
	{
		if (modalDialog_ == nullptr)
		{
			return;
		}
		modalDialog_->showModal();
	}

public:
	//==========================================================================
	ModalDlgTestPlugin(IComponentContext & contextManager )
		: Depends( contextManager )
	{
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( (uiFramework != nullptr) && (uiApplication != nullptr) );

		uiFramework->loadActionData( 
			":/plg_modal_dlg_test/actions.xml",
			IUIFramework::ResourceType::File );
		
		using namespace std::placeholders;
		testModalDialog_ = uiFramework->createAction(
			"ShowModalDialog", 
			std::bind( &ModalDlgTestPlugin::showModalDialog, this, _1 ) );

		auto viewCreator = get< IViewCreator >();
		viewCreator->createWindow( 
			"plg_modal_dlg_test/test_custom_dialog.qml",ObjectHandle(),
			[ this ]( std::unique_ptr< IWindow > & window )
		{
			modalDialog_ = std::move( window );
			if (modalDialog_ != nullptr)
			{
				modalDialog_->hide();
			}
		});
		uiApplication->addAction( *testModalDialog_ );
	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		uiApplication->removeAction( *testModalDialog_ );
		uiApplication->removeWindow( *modalDialog_ );
		testModalDialog_ = nullptr;
		modalDialog_ = nullptr;
		return true;
	}
	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}
};


PLG_CALLBACK_FUNC( ModalDlgTestPlugin )
} // end namespace wgt
