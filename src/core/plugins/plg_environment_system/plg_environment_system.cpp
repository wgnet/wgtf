#include "core_generic_plugin/generic_plugin.hpp"
#include "core_environment_system/env_system.hpp"

namespace wgt
{
/**
* A plugin which registers IEnvManager interfaces to allow other plugins to register EnvironmentComponent.
*
* @ingroup plugins
* @ingroup coreplugins
*       Requires Plugins:
*       - @ref coreplugins
*/
class EnvironmentSystemPlugin : public PluginMain
{
public:
	EnvironmentSystemPlugin( IComponentContext & context )
		: envManager_(std::make_unique<EnvManager>())
	{
		types_.push_back(context.registerInterface(envManager_.get(), false));
	}

	void Initialise(IComponentContext& /*contextManager*/)
	{
		envManager_->initialise();
	}

	bool Finalise(IComponentContext& /*contextManager*/)
	{
		envManager_->finalise();
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
		envManager_.reset();
	}

	std::unique_ptr<EnvManager> envManager_;
	InterfacePtrs types_;
};

PLG_CALLBACK_FUNC(EnvironmentSystemPlugin)
} // end namespace wgt
