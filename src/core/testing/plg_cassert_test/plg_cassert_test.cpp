#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/i_interface.hpp"

#include "core_common/assert.hpp"

namespace wgt
{
class CAssertTestPluginApplication : public Implements<IApplication>
{
public:
	int startApplication() override
	{
		TF_ASSERT(false && "This assert is meant to fail");
		return 0;
	}

	void quitApplication() override
	{
	}

	TimerId startTimer(int, TimerCallback) override
	{
		TF_ASSERT(!"This assert is meant to fail");
		return 0;
	}

	void killTimer(TimerId)
	{
		TF_ASSERT(!"This assert is meant to fail");
	}

	void setAppSettingsName(const char* appName) override
	{
		assert(!"This assert is meant to fail");
	}

	const char* getAppSettingsName() override
	{
		assert(!"This assert is meant to fail");
		static const char name = '\0';
		return name;
	}
};

/**
 *	A plugin which does a cassert on purpose to check if it is detected
 *	by the automated testing framework.
 *
 * @ingroup plugins
 */
class CAssertTestPluginTestPlugin : public PluginMain
{
public:
	CAssertTestPluginTestPlugin(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		contextManager.registerInterface(new CAssertTestPluginApplication());
		return true;
	}
};

PLG_CALLBACK_FUNC(CAssertTestPluginTestPlugin)

} // end namespace wgt
