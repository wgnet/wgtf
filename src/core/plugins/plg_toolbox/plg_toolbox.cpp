#include "toolbox.hpp"

#include "core_common/assert.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "toolbox_panel.hpp"

#include "reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
//==============================================================================
class ToolboxPlugin : public PluginMain
{
private:
	std::unique_ptr<ToolboxPanel> toolboxPanel_;
	std::unique_ptr<Toolbox> toolbox_;
	InterfacePtrs types_;

public:
	//==========================================================================
	ToolboxPlugin(IComponentContext& contextManager)
	{
		registerCallback(
		[this](IDefinitionManager& defManager) { ReflectionAutoRegistration::initAutoRegistration(defManager); });
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		TF_ASSERT(definitionManager != nullptr);
		if (definitionManager == nullptr)
			return false;

		toolboxPanel_.reset(new ToolboxPanel);
		toolbox_.reset(new Toolbox(toolboxPanel_.get()));

		types_.push_back(contextManager.registerInterface<IToolbox>(toolbox_.get(), false));
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		toolbox_->init();
		toolboxPanel_->init();
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		toolboxPanel_->fini();
		toolbox_->fini();
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager)
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
		toolbox_ = nullptr;
	}
};

PLG_CALLBACK_FUNC(ToolboxPlugin)
} // end namespace wgt
