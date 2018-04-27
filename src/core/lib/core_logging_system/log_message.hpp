#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include "log_level.hpp"
#include "interfaces/i_log_message.hpp"
#include <string>
#include <vector>

namespace wgt
{
class LogMessage : public ILogMessage
{
public:
	LogMessage(LogLevel level, const char* format, ...);
	LogMessage(LogLevel level, const char* format, va_list arguments);
	LogMessage(LogLevel level, std::string message);
	virtual ~LogMessage()
	{
	}

	LogMessage(const LogMessage&) = default;
	LogMessage(LogMessage&&) = default;
	LogMessage& operator=(const LogMessage&) = default;
	LogMessage& operator=(LogMessage&&) = default;

	const char* c_str() const override;
	const std::string& str() const override;

	LogLevel getLevel() const override;
	const char* getLevelString() const override;
	static const char* getLevelString(LogLevel level);

	std::string getAsHtml() const override;
	static std::string getAsHtml(std::string remark, LogLevel level);

	bool addTag(std::string tag) override;
	bool hasTag(const char* needle) const override;

private:
	LogMessage();
	void initMessageFromArguments(const char* format, va_list arguments);

private:
	// Cached value of the original log message after it has had variable
	// arguments printed to it.
	//
	// Note: Additional formatting is the responsibility of ILogger
	//       implementations. NGT will not force a logger output format on any
	//       developers.
	std::string message_;

	// Log Level
	// Used to filter severity of log messages.
	LogLevel level_;

	// Tags
	// A list of tags associated with this message.
	// ILogger implementations can use tags for additional filtering that isn't
	// satisfied by existing LogLevel options.
	std::vector<std::string> tags_;
};
} // end namespace wgt
#endif // LOG_MESSAGE_HPP
