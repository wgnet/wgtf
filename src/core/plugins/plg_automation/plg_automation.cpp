#include "core_automation/interfaces/automation_interface.hpp"

#include <ctime>

#include "core_generic_plugin/generic_plugin.hpp"

namespace wgt
{
class Automation : public Implements< AutomationInterface >
{
public:
	Automation()
		: startTime_( std::clock() )
	{
	}
	virtual ~Automation() {}

	virtual bool timedOut() override
	{
		const auto currentTime = std::clock();
		const auto dTime = currentTime - startTime_;
		const auto dTimeS = dTime / CLOCKS_PER_SEC;
		// TODO make this configurable
		const double TIMEOUT_SECONDS = 30.0;
		return (dTimeS > TIMEOUT_SECONDS);
	}

private:
	const clock_t startTime_;
};

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
		contextManager.registerInterface( new Automation() );
		return true;
	}
};

PLG_CALLBACK_FUNC( AutomationPlugin )
} // end namespace wgt
