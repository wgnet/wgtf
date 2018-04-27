#include "logging_system.hpp"

#include "log_message.hpp"
#include "interfaces/i_logger.hpp"
#include "interfaces/i_logging_model.hpp"
#include "interfaces/metadata/i_logging_model.mpp"
#include "alerts/alert_manager.hpp"
#include "alerts/basic_alert_logger.hpp"
#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include <cstdarg>
#include <cstdio>

namespace wgt
{
LoggingSystem::LoggingSystem()
    : alertManager_(new AlertManager()), basicAlertLogger_(nullptr), hasAlertManagement_(false), running_(true)
{
	processor_ = new std::thread(&LoggingSystem::process, this);
	get<IDefinitionManager>()->registerDefinition<TypeClassDefinition<ILoggingModel>>();
}

LoggingSystem::~LoggingSystem()
{
	// Kill specialized alert management and presentation
	disableAlertManagement();
	if (alertManager_ != nullptr)
	{
		delete alertManager_;
		alertManager_ = nullptr;
	}
}

void LoggingSystem::enableAlertManagement()
{
	if (!hasAlertManagement_)
	{
		basicAlertLogger_ = new BasicAlertLogger(*alertManager_);
		registerLogger(basicAlertLogger_);
		hasAlertManagement_ = true;
	}
}

void LoggingSystem::disableAlertManagement()
{
	if (basicAlertLogger_ != nullptr)
	{
		unregisterLogger(basicAlertLogger_);
		delete basicAlertLogger_;
		basicAlertLogger_ = nullptr;
		hasAlertManagement_ = false;
	}
}

void LoggingSystem::shutdown()
{	
	if (processor_ != nullptr)
	{
		while (!messages_.empty())
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}

		{
			tMessageLock guard(messageMutex_);
			running_ = false;
		}
		processorCV_.notify_one();

		processor_->join();
		delete processor_;
		processor_ = nullptr;
	}
}

bool LoggingSystem::registerLogger(ILogger* logger)
{
	std::lock_guard<std::mutex> guard(loggerMutex_);

	if (std::find(loggers_.begin(), loggers_.end(), logger) != loggers_.end())
	{
		// Logger already registered
		return false;
	}

	loggers_.push_back(logger);

	return true;
}

bool LoggingSystem::unregisterLogger(ILogger* logger)
{
	std::lock_guard<std::mutex> guard(loggerMutex_);

	std::vector<ILogger*>::iterator itrLogger = std::find(loggers_.begin(), loggers_.end(), logger);

	if (itrLogger != loggers_.end())
	{
		loggers_.erase(itrLogger);
		return true;
	}

	// Logger not found
	return false;
}

void LoggingSystem::log(LogLevel level, const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	ILogMessage* message = new LogMessage(level, format, arguments);
	va_end(arguments);

	log(message);
}

void LoggingSystem::log(ILogMessage* message)
{
	{
		tMessageLock guard(messageMutex_);
		messages_.push(message);
	}
	processorCV_.notify_one();
}

void LoggingSystem::flush()
{
	if (std::this_thread::get_id() == processorID_)
	{
		return;
	}
	tMessageLock guard(messageMutex_);
	if(!messages_.empty())
	{
		flushCV_.wait(guard);
	}
}

void LoggingSystem::process()
{
	processorID_ = std::this_thread::get_id();
	while (true)
	{
		bool isEmpty = true;
		{
			tMessageLock guard(messageMutex_);
			isEmpty = messages_.empty();
		}

		if (!isEmpty)
		{
			// Pop a message and broadcast it
			ILogMessage* currentMessage = nullptr;
			{
				tMessageLock guard(messageMutex_);
				currentMessage = messages_.front();
			}

			if (currentMessage != nullptr)
			{
				{
					std::lock_guard<std::mutex> guard(loggerMutex_);

					for (auto& logger : loggers_)
					{
						TF_ASSERT(logger != nullptr);
						logger->out(currentMessage);
					}
				}

				delete currentMessage;
				currentMessage = nullptr;
			}

			{
				tMessageLock guard(messageMutex_);
				messages_.pop();
			}
		}
		else
		{
			if (!running_)
				break;
			{
				flushCV_.notify_all();
				tMessageLock lock(messageMutex_);
				processorCV_.wait(lock, [this]() { return !running_ || !messages_.empty(); });
			}
		}
	}
}
} // end namespace wgt
