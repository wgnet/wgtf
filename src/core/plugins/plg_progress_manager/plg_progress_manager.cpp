#include "progress_manager.hpp"

#include "core_generic_plugin/generic_plugin.hpp"

namespace wgt
{
/**
* A plugin which registers an IProgressManager interface that is responsible for
* providing feedback to users when running commands that take a long amount of time
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ProgressManagerUIPlugin : public PluginMain
{
public:
	//==========================================================================
	ProgressManagerUIPlugin(IComponentContext& contextManager) : progressManager_(new ProgressManager())
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		progressManager_->init();
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		progressManager_->fini();
		progressManager_.reset();
		return true;
	}

private:
	std::unique_ptr<ProgressManager> progressManager_;
};

PLG_CALLBACK_FUNC(ProgressManagerUIPlugin)
} // end namespace wgt
