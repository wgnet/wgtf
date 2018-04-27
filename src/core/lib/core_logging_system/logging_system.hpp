#ifndef LOGGING_SYSTEM_HPP
#define LOGGING_SYSTEM_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
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

class LoggingSystem final
	: public Implements<ILoggingSystem>
	, public Depends<class IDefinitionManager>
{
public:
	LoggingSystem();
	virtual ~LoggingSystem();

	virtual bool registerLogger(ILogger* logger) override;
	virtual bool unregisterLogger(ILogger* logger) override;
	virtual AlertManager* getAlertManager() override
	{
		return alertManager_;
	}
	virtual void enableAlertManagement() override;
	virtual void disableAlertManagement() override;
	virtual void log(LogLevel level, const char* format, ...) override;
	virtual void log(ILogMessage* message) override;
	virtual void shutdown() override;
	virtual void process() override;
	virtual void flush() override;

private:
	typedef std::vector<ILogger*> tLoggerList;
	tLoggerList loggers_;

	typedef std::queue<ILogMessage*> tMessageQueue;
	tMessageQueue messages_;

	typedef std::unique_lock<std::mutex> tMessageLock;
	std::thread* processor_;
	std::thread::id processorID_;
	std::mutex messageMutex_;
	std::mutex loggerMutex_;
	wg_condition_variable processorCV_;
	wg_condition_variable flushCV_;	

	AlertManager* alertManager_;
	BasicAlertLogger* basicAlertLogger_;
	bool hasAlertManagement_;

	bool running_;
};
} // end namespace wgt
#endif // LOGGING_SYSTEM_HPP
