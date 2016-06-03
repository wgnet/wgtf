#ifndef LOGGING_HPP
#define LOGGING_HPP

// NGT-276 TODO: Replace with a logging plugin instead

#define NGT_MSG( format, ... ) \
::wgt::logMessage( format, ## __VA_ARGS__ )

#define NGT_TRACE_MSG( format, ... ) \
::wgt::logMessage( "%s(%d): ", __FILE__, __LINE__ ); \
::wgt::logMessage( format, ## __VA_ARGS__ )

#define NGT_DEBUG_MSG( format, ... ) \
::wgt::logMessage( "%s(%d): ", __FILE__, __LINE__ ); \
::wgt::logMessage( format, ## __VA_ARGS__ )


#define NGT_WARNING_MSG( format, ... ) \
::wgt::logMessage( "%s(%d): ", __FILE__, __LINE__ ); \
::wgt::logMessage( format, ## __VA_ARGS__ )


#define NGT_ERROR_MSG( format, ... ) \
::wgt::logMessage( "%s(%d): ", __FILE__, __LINE__ ); \
::wgt::logMessage( format, ## __VA_ARGS__ )

namespace wgt
{
int logMessage( const char* format, ... );
} // end namespace wgt
#endif // LOGGING_HPP
