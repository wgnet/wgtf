#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class A
{
public:
	int i_;
};

class NullApplication
: public Implements<IApplication>
{
public:
	int startApplication() override
	{
		A* ptr = nullptr;
		ptr->i_ = 5;
		return 0;
	}

	void quitApplication() override
	{
	}
};

/**
 *	A plugin which de-references null on purpose to check if it is detected
 *	by the automated testing framework.
 *
 * @ingroup plugins
 */
class NullTestPlugin
: public PluginMain
{
public:
	NullTestPlugin(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		contextManager.registerInterface(new NullApplication());
		return true;
	}
};

PLG_CALLBACK_FUNC(NullTestPlugin)
} // end namespace wgt
