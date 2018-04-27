#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "main_window/main_window.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates a basic Qt window from a .ui file which other plugins will add panels an components to.
* Mutually exclusive with MayaAdapterPlugin.
*
* @ingroup plugins
* @ingroup coreplugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class TestWindowPlugin : public PluginMain
{
private:
	MainWindow mainWindow_;

public:
	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		mainWindow_.init();
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		mainWindow_.fini();
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager)
	{
	}
};

PLG_CALLBACK_FUNC(TestWindowPlugin)
} // end namespace wgt
