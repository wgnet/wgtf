#include "core_generic_plugin/generic_plugin.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "perforce_version_control.hpp"

namespace wgt
{
class PerforcePlugin : public PluginMain
{
public:
	PerforcePlugin(IComponentContext& contextManager)
	{
		types_.emplace_back(contextManager.registerInterface<PerforceVersionControl>(new PerforceVersionControl()));
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}

private:
	InterfacePtrs types_;
};

PLG_CALLBACK_FUNC(PerforcePlugin)
} // end namespace wgt
