#ifndef LOGGING_HPP
#define LOGGING_HPP

#include "log_level.hpp"
#include "wg_logging_dll.hpp"
#include <functional>

#define NGT_MSG(format, ...)                           \
    ::wgt::logMessage(LOG_DEBUG, format, ##__VA_ARGS__)

#define NGT_LARGE_MSG(format, ...)                         \
    ::wgt::logMessageLarge(LOG_DEBUG, format, ##__VA_ARGS__)

#define NGT_TRACE_MSG(format, ...)                                             \
    ::wgt::logMessage(LOG_DEBUG, __FILE__, (int)__LINE__, format, ##__VA_ARGS__)

#define NGT_DEBUG_MSG(format, ...)                                             \
    ::wgt::logMessage(LOG_DEBUG, __FILE__, (int)__LINE__, format, ##__VA_ARGS__)

#define NGT_INFO_MSG(format, ...)                                             \
    ::wgt::logMessage(LOG_INFO, __FILE__, (int)__LINE__, format, ##__VA_ARGS__)

#define NGT_WARNING_MSG(format, ...)                                             \
    ::wgt::logMessage(LOG_WARNING, __FILE__, (int)__LINE__, format, ##__VA_ARGS__)

#define NGT_ERROR_MSG(format, ...)                                             \
    ::wgt::logMessage(LOG_ERROR, __FILE__, (int)__LINE__, format, ##__VA_ARGS__)

#define NGT_FLUSH_MSG() ::wgt::flushMessage();

namespace wgt
{
	typedef std::function<void(LogLevel level, const char* message)> LoggingFn;
	WG_LOGGING_DLL void setCustomLoggingHandle(LoggingFn fn);
	WG_LOGGING_DLL void logMessage(LogLevel level, const char* format, ...);
	WG_LOGGING_DLL void logMessage(LogLevel level, const char* file, int line, const char* format, ...);
	WG_LOGGING_DLL void logMessage(LogLevel level, const char* format, va_list args);
	WG_LOGGING_DLL void logMessageLarge(LogLevel level, const char* format, ...);
	WG_LOGGING_DLL void flushMessage();

} // end namespace wgt
#endif // LOGGING_HPP
