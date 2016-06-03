#ifndef LOGGING_SYSTEM_HPP
#define LOGGING_SYSTEM_HPP

#include "core_dependency_system/i_interface.hpp"
#include "interfaces/i_logging_system.hpp"
#include "log_level.hpp"
#include <queue>
#include <thread>
#include <mutex>
#include "core_common/wg_condition_variable.hpp"

namespace wgt
{
class AlertManager;
class BasicAlertLogger;

class LoggingSystem :
	public Implements< ILoggingSystem >
{
public:

	LoggingSystem();
	virtual ~LoggingSystem();

	virtual bool registerLogger( ILogger* logger );
	virtual bool unregisterLogger( ILogger* logger );
	virtual AlertManager* getAlertManager() { return alertManager_; }
	virtual void enableAlertManagement();
	virtual void disableAlertManagement();
	virtual void log( LogLevel level, const char* format, ... );
	virtual void log( LogMessage* message );
	virtual void shutdown();
	virtual void process();

private:

	typedef std::vector< ILogger* > tLoggerList;
	tLoggerList loggers_;

	typedef std::queue< LogMessage* > tMessageQueue;
	tMessageQueue messages_;

	typedef std::unique_lock< std::mutex > tMessageLock;
	std::thread* processor_;
	std::mutex messageMutex_;
	std::mutex loggerMutex_;
	wg_condition_variable processorCV_;

	AlertManager* alertManager_;
	BasicAlertLogger* basicAlertLogger_;
	bool hasAlertManagement_;

	bool running_;
};
} // end namespace wgt
#endif // LOGGING_SYSTEM_HPP
