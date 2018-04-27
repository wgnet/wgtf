#include "log_listener.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_logging_system/interfaces/i_logger.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"
#include "core_logging_system/log_message.hpp"

namespace wgt
{
struct LogListener::Impl
	: Depends<ILoggingSystem>
	, ILogger
{
	Impl(LogLevel logLevel)
		: logLevel_(logLevel)
	{
		ILoggingSystem* logsys = get<ILoggingSystem>();
		if (logsys)
		{
			logsys->registerLogger(this);
		}
	}

	Impl(const Impl& rhs)
		: logLevel_(rhs.logLevel_)
		, logReport_(rhs.logReport_)
	{
		ILoggingSystem* logsys = get<ILoggingSystem>();
		if (logsys)
		{
			logsys->registerLogger(this);
		}
	}

	~Impl()
	{
		ILoggingSystem* logsys = get<ILoggingSystem>();
		if (logsys)
		{
			logsys->unregisterLogger(this);
		}
	}

	LogListener::LogReport getLogReport() const
	{
		auto logSys = get<ILoggingSystem>();
		logSys->flush();
		return logReport_;
	}
	
	void out(ILogMessage* message)
	{
		if (message != nullptr && message->getLevel() <= logLevel_)
		{
			logReport_.push_back(*static_cast<LogMessage*>(message));
		}
	}
	LogLevel logLevel_;
	LogListener::LogReport logReport_;
};

void LogListener::startListening(LogLevel logLevel /* = LogLevel::LOG_ERROR */)
{
	impl_ = std::make_unique<Impl>(logLevel);
}

LogListener::LogReport LogListener::stopListening()
{	
	LogListener::LogReport ret;
	if (impl_ != nullptr)
	{
		ret = impl_->getLogReport();
		impl_.reset();
	}
	return ret;
}

bool LogListener::isListening() const
{
	return impl_ != nullptr;
}

LogListener::LogListener()
{
}

LogListener::~LogListener()
{
}

LogListener::LogListener(const LogListener& rhs)
{
	if (rhs.impl_ != nullptr)
	{
		impl_ = std::make_unique<Impl>(*rhs.impl_);
	}
}

LogListener& LogListener::operator=(const LogListener& rhs)
{
	impl_.reset();
	if (rhs.impl_)
	{
		impl_ = std::make_unique<Impl>(*rhs.impl_);
	}
	return *this;
}

}