#include "core_generic_plugin/interfaces/i_application.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{

/**
* @cond HIDDEN_SYMBOLS
*/
class A
{
public:
	int i_;
};
/**
* @endcond
*/

class NullApplication : public Implements<IApplication>
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

	TimerId startTimer(int, TimerCallback) override
	{
		TF_ASSERT(!"Not Implemented");
		return 0;
	}

	void killTimer(TimerId)
	{
		TF_ASSERT(!"Not Implemented");
	}

	void setAppSettingsName(const char* appName) override
	{
		assert(!"Not Implemented");
	}

	const char* getAppSettingsName() override
	{
		assert(!"Not Implemented");
		static const char name = '\0';
		return name;
	}
};

/**
 *	A plugin which de-references null on purpose to check if it is detected
 *	by the automated testing framework.
 *
 * @ingroup plugins
 */
class NullTestPlugin : public PluginMain
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
