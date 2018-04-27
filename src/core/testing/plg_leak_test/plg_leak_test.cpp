#include "core_generic_plugin/interfaces/i_application.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class LeakApplication : public Implements<IApplication>
{
public:
	int startApplication() override
	{
		auto memoryLeak = new int[1024];
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
 *	A plugin which creates a memory leak on purpose to check if it is detected
 *	by the allocator.
 *
 * @ingroup plugins
 */
class LeakTestPlugin : public PluginMain
{
public:
	LeakTestPlugin(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		contextManager.registerInterface(new LeakApplication());
		return true;
	}
};

PLG_CALLBACK_FUNC(LeakTestPlugin)
} // end namespace wgt
