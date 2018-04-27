#include "logging.hpp"

#include <cstdarg>
#include <cstdio>
#include "core_common/ngt_windows.hpp"

namespace wgt
{
static LoggingFn CUSTOM_LOGGING_HANDLE = nullptr;

const char* getLevelString(LogLevel level)
{
	switch (level)
	{
	case LOG_WARNING:
		return "WARNING: ";
	case LOG_ERROR:
		return "ERROR: ";
	case LOG_FATAL:
		return "FATAL: ";
	default:
		return "";
	}
}

template<int n>
void logToDebugOutput(LogLevel level, const char* message, const char* file, int line)
{
	const int size = n + 512;
	char buffer[size];
	if (file != nullptr && line != -1)
	{
		sprintf(buffer, "%s(%d): %s%s\n",
			file, line, getLevelString(level), message);
	}
	else
	{
		sprintf(buffer, "%s%s\n",
			getLevelString(level), message);
	}
	buffer[size - 1] = '\0';
	OutputDebugStringA(buffer);
}

template<int n>
void logMessage(LogLevel level, const char* format, const char* file, int line, va_list args)
{
	char buffer[n];
	vsnprintf(buffer, sizeof(buffer) - 1, format, args);
	buffer[n - 1] = '\0';

	const auto index = strlen(buffer) - 1;
	if (index >= 0 && index < n)
	{
		if (buffer[index] == '\n')
		{
			buffer[index] = '\0';
		}
		if (index != 0 && buffer[0] == '\n')
		{
			buffer[0] = ' ';
		}
	}

	logToDebugOutput<n>(level, buffer, file, line);

	if (CUSTOM_LOGGING_HANDLE)
	{
		CUSTOM_LOGGING_HANDLE(level, buffer);
	}
}

void logMessage(LogLevel level, const char* format, va_list args)
{
	logMessage<2048>(level, format, nullptr, -1, args);
}

void logMessage(LogLevel level, const char* file, int line, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	logMessage<2048>(level, format, file, line, args);
	va_end(args);
}

void logMessage(LogLevel level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	logMessage<2048>(level, format, nullptr, -1, args);
	va_end(args);
}

void logMessageLarge(LogLevel level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	logMessage<4096>(level, format, nullptr, -1, args);
	va_end(args);
}

void setCustomLoggingHandle(LoggingFn fn)
{
	CUSTOM_LOGGING_HANDLE = fn;
}

void flushMessage()
{
#ifdef _WIN32
	// Do nothing
#elif __APPLE__
	fflush(stdout);
#endif
}
} // end namespace wgt
