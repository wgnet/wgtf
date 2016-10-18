#include "core_automation/interfaces/automation_interface.hpp"
#include "core_common/wg_condition_variable.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <memory>
#include <thread>

namespace wgt
{
class Automation : public Implements<AutomationInterface>
                   ,
                   Depends<IUIApplication>
{
public:
	// TODO make this configurable
	static const std::chrono::seconds TIMEOUT_SECONDS;

	Automation(IComponentContext& context);
	virtual ~Automation();

	virtual bool timedOut() override;
	virtual void notifyLoadingDone() override;

	void threadFunc();

	const clock_t startTime_;

	std::thread workerThread_;
	wg_condition_variable workerWakeUp_;
	std::mutex workerMutex_;
	std::atomic<bool> loadingDone_;
};

// TODO make this configurable
/* static */ const std::chrono::seconds Automation::TIMEOUT_SECONDS(30);

Automation::Automation(IComponentContext& context)
    : Depends<IUIApplication>(context)
    , startTime_(std::clock())
    , loadingDone_(false)
{
	workerThread_ = std::thread(&Automation::threadFunc, this);
}

Automation::~Automation()
{
	workerThread_.join();
}

bool Automation::timedOut() /* override */
{
	const auto currentTime = std::clock();
	const auto dTime = currentTime - startTime_;
	const auto dTimeS = dTime / CLOCKS_PER_SEC;
	return (dTimeS > TIMEOUT_SECONDS.count());
}

void Automation::notifyLoadingDone() /* override */
{
	loadingDone_ = true;
	workerWakeUp_.notify_all();
}

void Automation::threadFunc()
{
	std::unique_lock<std::mutex> lock(workerMutex_);

	workerWakeUp_.wait_for(lock,
	                       std::chrono::seconds(TIMEOUT_SECONDS),
	                       [this]
	                       {
		                       return loadingDone_.load();
		                   });

	auto pUIApplication = this->get<IUIApplication>();
	if (pUIApplication != nullptr)
	{
		pUIApplication->quitApplication();
	}
}

/**
* A plugin which automatically quits the application after an amount of time
*
* @ingroup plugins
* @image html plg_automation.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class AutomationPlugin
	: public PluginMain
{
public:
	AutomationPlugin( IComponentContext & contextManager )
	{
	}

	virtual bool PostLoad( IComponentContext & contextManager ) override
	{
		automation_.reset(new Automation(contextManager));
		pAutomationInterface_ = contextManager.registerInterface(automation_.get(),
		                                                         false /* transferOwnership */);

		return true;
	}

	virtual void Unload(IComponentContext& contextManager) override
	{
		// Stop worker thread
		contextManager.deregisterInterface(pAutomationInterface_);
		automation_.reset();
	}

	std::unique_ptr<Automation> automation_;
	IInterface* pAutomationInterface_;
};

PLG_CALLBACK_FUNC( AutomationPlugin )
} // end namespace wgt
