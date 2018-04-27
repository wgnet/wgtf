#ifndef I_LOGGING_SYSTEM_HPP
#define I_LOGGING_SYSTEM_HPP

// TODO: move log_level to interface folder
#include "../log_level.hpp"

namespace wgt
{
class AlertManager;
class ILogger;
class ILogMessage;

class ILoggingSystem
{
public:
	virtual ~ILoggingSystem() = default;

	virtual bool registerLogger(ILogger* logger) = 0;
	virtual bool unregisterLogger(ILogger* logger) = 0;
	virtual AlertManager* getAlertManager() = 0;
	virtual void enableAlertManagement() = 0;
	virtual void disableAlertManagement() = 0;
	virtual void log(LogLevel level, const char* format, ...) = 0;
	virtual void log(ILogMessage* message) = 0;
	virtual void shutdown() = 0;
	virtual void process() = 0;
	virtual void flush() = 0;
};

#define TF_LOG_HELPER(level, format, ...) if(auto logger = get<ILoggingSystem>()) { logger->log(level, format, ##__VA_ARGS__ ); }
#define TF_LOG_ERROR(format, ...) TF_LOG_HELPER(LogLevel::LOG_ERROR, format "\n", ##__VA_ARGS__ )
#define TF_LOG_WARNING(format, ...) TF_LOG_HELPER(LogLevel::LOG_WARNING, format "\n", ##__VA_ARGS__ )
#define TF_LOG_INFO(format, ...) TF_LOG_HELPER(LogLevel::LOG_INFO, format "\n", ##__VA_ARGS__ )

} // end namespace wgt
#endif // I_LOGGING_SYSTEM_HPP
