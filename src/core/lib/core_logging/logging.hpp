#ifndef LOGGING_HPP
#define LOGGING_HPP

// NGT-276 TODO: Replace with a logging plugin instead

#define NGT_MSG(format, ...) \
	\
::wgt::logMessageNewline(format, ##__VA_ARGS__)

#define NGT_TRACE_MSG(format, ...)                 \
	\
::wgt::logMessage("%s(%d): ", __FILE__, __LINE__); \
	\
::wgt::logMessageNewline(format, ##__VA_ARGS__)

#define NGT_DEBUG_MSG(format, ...)                 \
	\
::wgt::logMessage("%s(%d): ", __FILE__, __LINE__); \
	\
::wgt::logMessageNewline(format, ##__VA_ARGS__)

#define NGT_WARNING_MSG(format, ...)                        \
	\
::wgt::logMessage("WARNING: %s(%d): ", __FILE__, __LINE__); \
	\
::wgt::logMessageNewline(format, ##__VA_ARGS__)

#define NGT_ERROR_MSG(format, ...)                        \
	\
::wgt::logMessage("ERROR: %s(%d): ", __FILE__, __LINE__); \
	\
::wgt::logMessageNewline(format, ##__VA_ARGS__)

#define NGT_FLUSH_MSG() \
	\
::wgt::flushMessage();

namespace wgt
{
int logMessage(const char* format, ...);
int logMessageNewline(const char* format, ...);
void flushMessage();
} // end namespace wgt
#endif // LOGGING_HPP
