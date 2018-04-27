#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "test_ui/context.hpp"
#include "test_ui/test_ui.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "test_datasource.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"

#include "pages/metadata/test_page.mpp"
#include "pages/metadata/test_polymorphism.mpp"
#include "test_ui/metadata/context.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include <vector>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates a menu option to open and close environments, each
* environment changing what is displayed on a single Property Tree Test Panel.
*
* @ingroup plugins
* @image html plg_ui_main_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - ReflectionObjectTestPlugin
*/
class MainUITestPlugin : public PluginMain
{
private:
	std::unique_ptr<TestUI> testUI_;
	std::unique_ptr<TestDataSourceManager> dataSrc_;
	InterfacePtrs types_;

public:
	//==========================================================================
	MainUITestPlugin(IComponentContext& contextManager)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		// register test data source
		dataSrc_.reset(new TestDataSourceManager());
		types_.push_back(contextManager.registerInterface(dataSrc_.get(), false));

		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		// register reflected type definition
		dataSrc_->init(contextManager);

		auto envManager = contextManager.queryInterface<IEnvManager>();
		assert(envManager);
		testUI_ = std::make_unique<TestUI>(*envManager);
		testUI_->init();
	}
	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		testUI_->fini();
		testUI_.reset();

		assert(dataSrc_);
		dataSrc_->fini();

		return true;
	}
	//==========================================================================
	void Unload(IComponentContext& contextManager)
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
		dataSrc_ = nullptr;
	}
};

PLG_CALLBACK_FUNC(MainUITestPlugin)
} // end namespace wgt
