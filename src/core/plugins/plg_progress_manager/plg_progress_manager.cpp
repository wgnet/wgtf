#include "progress_manager.hpp"

#include "core_generic_plugin/generic_plugin.hpp"


namespace wgt
{
class ProgressManagerUIPlugin
	: public PluginMain
{
public:

	//==========================================================================
	ProgressManagerUIPlugin( IComponentContext & contextManager )
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
		progressManager_ = new ProgressManager();
		progressManager_->init( contextManager );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		progressManager_->fini();
		delete progressManager_;
		progressManager_ = nullptr;

		return true;
	}

private:
	ProgressManager * progressManager_;

};

PLG_CALLBACK_FUNC( ProgressManagerUIPlugin )
} // end namespace wgt
