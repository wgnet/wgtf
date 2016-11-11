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
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		types_.emplace_back(contextManager.registerInterface<PerforceVersionControl>(new PerforceVersionControl()));
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
			contextManager.deregisterInterface(type);
		}
	}

private:
	std::vector<IInterface*> types_;
};

PLG_CALLBACK_FUNC(PerforcePlugin)
} // end namespace wgt
