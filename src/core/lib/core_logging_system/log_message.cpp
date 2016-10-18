#include "log_message.hpp"
#include <cstdarg>
#include <cstdio>

namespace wgt
{
LogMessage::LogMessage( LogLevel level, const char* format, ... )
	: level_( level )
{
	va_list arguments;
	va_start( arguments, format );
	initMessageFromArguments( format, arguments );
	va_end( arguments );
}

LogMessage::LogMessage( LogLevel level, const char* format, va_list arguments )
	: level_( level )
{
	initMessageFromArguments( format, arguments );
}

LogMessage::LogMessage( LogLevel level, std::string message )
	: message_( message )
	, level_( level )
{
}

void LogMessage::initMessageFromArguments( const char* format, 
										   va_list arguments )
{
	const size_t bufferSize = 4095;
	char buffer[ bufferSize ];

	vsnprintf( buffer, sizeof( buffer ) - 1, format, arguments );
	buffer[ sizeof( buffer ) - 1 ] = '\0';

	message_ = buffer;
}

const char* LogMessage::getLevelString()
{
	switch (level_)
	{
	case LOG_FATAL:
		{
			return "FATAL";
			break;
		}
	case LOG_ERROR:
		{
			return "ERROR";
			break;
		}
	case LOG_WARNING:
		{
			return "WARNING";
			break;
		}
	case LOG_ALERT:
		{
			return "ALERT";
			break;
		}
	case LOG_INFO:
		{
			return "INFO";
			break;
		}
	case LOG_DEBUG:
		{
			return "DEBUG";
			break;
		}
	default:
		{
			return "UNKNOWN";
		}
	}
}

const char* LogMessage::c_str()
{
	return message_.c_str();
}

const std::string& LogMessage::str()
{
	return message_;
}

LogLevel LogMessage::getLevel()
{
	return level_;
}

bool LogMessage::addTag( std::string tag )
{
	bool exists = hasTag( tag.c_str() );
	if (exists)
	{
		return false;
	}

	tags_.push_back( tag );

	return true;
}

bool LogMessage::hasTag( const char* needle )
{
	if (std::find( tags_.begin(), tags_.end(), needle) != tags_.end())
	{
		return true;
	}

	return false;
}
} // end namespace wgt
