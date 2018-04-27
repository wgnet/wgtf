#include "core_generic_plugin/generic_plugin.hpp"
#include "controls_test_panel.hpp"

#include "metadata/controls_test_panel.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include <memory>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates a panel to test various components and subpanels
*
* @ingroup plugins
* @image html plg_controls_test_1.png
* @image html plg_controls_test_2.png
* @image html plg_controls_test_3.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ControlsTestUIPlugin : public PluginMain
{
public:
	ControlsTestUIPlugin(IComponentContext& componentContext)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	bool PostLoad(IComponentContext& componentContext) override
	{
		controlsTestPanel_.reset(new ControlsTestPanel);
		return true;
	}

	void Initialise(IComponentContext& componentContext) override
	{
		controlsTestPanel_->addPanel();
	}

	bool Finalise(IComponentContext& componentContext) override
	{
		controlsTestPanel_->removePanel();
		return true;
	}

	void Unload(IComponentContext& componentContext) override
	{
		controlsTestPanel_.reset();
	}

private:
	std::unique_ptr<ControlsTestPanel> controlsTestPanel_;
};

PLG_CALLBACK_FUNC(ControlsTestUIPlugin)
} // end namespace wgt
