#ifndef BASIC_ALERT_LOGGER_HPP
#define BASIC_ALERT_LOGGER_HPP

#include "core_logging_system/interfaces/i_logger.hpp"

namespace wgt
{
class AlertManager;
class ILogMessage;

class BasicAlertLogger : public ILogger
{
public:
	static const unsigned int sAlertMaxLength = 150;

public:
	BasicAlertLogger(AlertManager& alertManager);

	virtual ~BasicAlertLogger();

	virtual void out(ILogMessage* message);

private:
	AlertManager& alertManager_;
};
} // end namespace wgt
#endif // BASIC_ALERT_LOGGER_HPP
