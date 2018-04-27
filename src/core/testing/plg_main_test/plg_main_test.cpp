#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "../interfaces_test/test_interface.hpp"
#include "../interfaces_test/auto_populate.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class MainApplication : public Implements<IApplication>
{
	IComponentContext& context_;
	std::string name_;

public:
	MainApplication(IComponentContext& context) : context_(context)
	{
	}

	int startApplication() override
	{
		// Query all versions of interface with major version 0
		{
			std::vector<INTERFACE_VERSION(TestInterface, 0, 0)*> interfaces;
			context_.queryInterface(interfaces);
			for (auto& pInterface : interfaces)
			{
				pInterface->test();
			}
		}

		// Query only interfaces supporting version 0.1 or greater
		{
			std::vector<INTERFACE_VERSION(TestInterface, 0, 1)*> interfaces;
			context_.queryInterface(interfaces);
			for (auto& pInterface : interfaces)
			{
				pInterface->test();
				pInterface->test2();
			}
		}

		// Query only interfaces supporting version 1.0 or greater
		{
			std::vector<INTERFACE_VERSION(TestInterface, 1, 0)*> interfaces;
			context_.queryInterface(interfaces);
			int value = 100;
			for (auto& pInterface : interfaces)
			{
				pInterface->test(value);
				// test2() removed
				++value;
			}
		}
		return 0;
	}

	void quitApplication() override
	{
	}

	TimerId startTimer(int, TimerCallback) override
	{
		assert(!"Not Implemented");
		return 0;
	}

	void killTimer(TimerId)
	{
		assert(!"Not Implemented");
	}

	void setAppSettingsName(const char* appName) override
	{
		name_ = appName;
	}

	const char* getAppSettingsName() override
	{
		return name_.c_str();
	}
};

/**
* A plugin which creates and registers an IUIApplication interface for testing plugins that implement TestInterface
*
* @ingroup plugins
*/
class MainTestPlugin : public PluginMain
{
public:
	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		contextManager.registerInterface(new MainApplication(contextManager));
		return true;
	}
};

PLG_CALLBACK_FUNC(MainTestPlugin)
} // end namespace wgt
