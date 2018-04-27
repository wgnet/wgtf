#include "core_generic_plugin/generic_plugin.hpp"
#include "grabber_manager.hpp"

#include <memory>

namespace wgt
{
	class GrabberManagerPlugin : public PluginMain
	{
	public:
		GrabberManagerPlugin(IComponentContext& contextManager)
		{
		}

		~GrabberManagerPlugin()
		{
		}

		bool PostLoad(IComponentContext& contextManager)
		{
			grabberManager_ = std::make_unique<GrabberManager>();
			grabberManagerInterface_ = contextManager.registerInterface(grabberManager_.get(), false);
			grabberManager_->postLoad();
			return true;
		}

		void Initialise(IComponentContext& contextManager)
		{
			grabberManager_->init();
		}

		bool Finalise(IComponentContext& contextManager)
		{
			grabberManager_->fini();
			return true;
		}

		void Unload(IComponentContext& contextManager)
		{
			contextManager.deregisterInterface(grabberManagerInterface_.get());
			grabberManager_.reset();
		}

	private:
		std::unique_ptr<IGrabberManager> grabberManager_;
		InterfacePtr grabberManagerInterface_;
	};

PLG_CALLBACK_FUNC(GrabberManagerPlugin)
} // end namespace wgt