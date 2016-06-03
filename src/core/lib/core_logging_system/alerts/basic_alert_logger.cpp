#include "basic_alert_logger.hpp"
#include "core_logging_system/alerts/alert_manager.hpp"
#include "core_logging_system/log_message.hpp"
#include "core_logging_system/log_level.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include <stdio.h>

namespace wgt
{
BasicAlertLogger::BasicAlertLogger( AlertManager & alertManager )
	: alertManager_( alertManager )
{
}

BasicAlertLogger::~BasicAlertLogger()
{
}

void BasicAlertLogger::out( LogMessage* message )
{
	if (message->getLevel() == LOG_ALERT)
	{
		alertManager_.add( message->c_str() );
	}
}
} // end namespace wgt
