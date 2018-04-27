#include "log_message.hpp"
#include <cstdarg>
#include <cstdio>
#include <sstream>
#include <regex>

namespace wgt
{
LogMessage::LogMessage(LogLevel level, const char* format, ...) : level_(level)
{
	va_list arguments;
	va_start(arguments, format);
	initMessageFromArguments(format, arguments);
	va_end(arguments);
}

LogMessage::LogMessage(LogLevel level, const char* format, va_list arguments) : level_(level)
{
	initMessageFromArguments(format, arguments);
}

LogMessage::LogMessage(LogLevel level, std::string message) : message_(message), level_(level)
{
}

std::string LogMessage::getAsHtml() const
{
	return getAsHtml(str(), getLevel());
}

std::string LogMessage::getAsHtml(std::string remark, LogLevel level)
{
	// Trim tailing new line each remark is in a <div>
	if (!remark.empty() && *remark.rbegin() == '\n')
	{
		remark.erase(--remark.end());
	}
	// Remove carriage returns
	remark = regex_replace(remark, std::regex("\r"), std::string(" "));
	// Replace double quotes with singles to allow assigning innerHTML in javascript
	remark = regex_replace(remark, std::regex("\""), std::string("'"));
	// Replace new lines with breaks
	remark = regex_replace(remark, std::regex("\n"), std::string("<br>"));

	// Rather than modify all lines of code that use <color> tags we replace them here with the supported <font>
	// tags
	remark = regex_replace(remark, std::regex("<color\\svalue=['\"](.*)['\"]>"), std::string("<font color='$1'>"));
	remark = regex_replace(remark, std::regex("</color>"), std::string("</font>"));

	// In order to keep formatted whitespace we wrap the line in a div
	std::stringstream ss;
	ss << "<div style='white-space: pre;'>";

	switch (level)
	{
	case LOG_FATAL:
	case LOG_ERROR:
		ss << "<font class='error'>(" << getLevelString(level) << ") ";
		break;
	case LOG_WARNING:
		ss << "<font class='warning'>(" << getLevelString(level) << ") ";
		break;
	case LOG_ALERT:
		ss << "<font class='alert'>(" << getLevelString(level) << ") ";
		break;
	case LOG_INFO:
		ss << "<font class='info'>";
		break;
	case LOG_HEADER:
		ss << "<font class='header'>";
		break;
	case LOG_DEBUG:
	default:
		ss << "<font>";
	}
	ss << remark << "</font></div>";

	return ss.str();
}

void LogMessage::initMessageFromArguments(const char* format, va_list arguments)
{
	const size_t bufferSize = 4095;
	char buffer[bufferSize];

	vsnprintf(buffer, sizeof(buffer) - 1, format, arguments);
	buffer[sizeof(buffer) - 1] = '\0';

	message_ = buffer;
}

const char* LogMessage::getLevelString() const
{
	return getLevelString(level_);
}

const char* LogMessage::getLevelString(LogLevel level)
{
	switch (level)
	{
	case LOG_FATAL:
		return "FATAL";
	case LOG_ERROR:
		return "ERROR";
	case LOG_WARNING:
		return "WARNING";
	case LOG_ALERT:
		return "ALERT";
	case LOG_INFO:
		return "INFO";
	case LOG_DEBUG:
		return "DEBUG";
	case LOG_HEADER:
		return "HEADER";
	default:
		return "UNKNOWN";
	}
}

const char* LogMessage::c_str() const
{
	return message_.c_str();
}

const std::string& LogMessage::str() const
{
	return message_;
}

LogLevel LogMessage::getLevel() const
{
	return level_;
}

bool LogMessage::addTag(std::string tag)
{
	bool exists = hasTag(tag.c_str());
	if (exists)
	{
		return false;
	}

	tags_.push_back(tag);

	return true;
}

bool LogMessage::hasTag(const char* needle) const
{
	if (std::find(tags_.begin(), tags_.end(), needle) != tags_.end())
	{
		return true;
	}

	return false;
}
} // end namespace wgt
