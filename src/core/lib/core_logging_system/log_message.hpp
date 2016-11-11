#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include "log_level.hpp"
#include <string>
#include <vector>

namespace wgt
{
class LogMessage
{
public:
	LogMessage(LogLevel level, const char* format, ...);
	LogMessage(LogLevel level, const char* format, va_list arguments);
	LogMessage(LogLevel level, std::string message);
	virtual ~LogMessage()
	{
	}

	const char* c_str();
	const std::string& str();

	LogLevel getLevel();
	const char* getLevelString();

	bool addTag(std::string tag);
	bool hasTag(const char* needle);

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
