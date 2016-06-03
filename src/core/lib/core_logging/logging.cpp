#include "logging.hpp"

#include <cstdarg>
#include <cstdio>
#include "core_common/ngt_windows.hpp"


namespace wgt
{
int logMessage( const char* format, ... )
{
	const size_t bufferSize = 4095;
	char buffer[ bufferSize ];

	va_list args;
	va_start( args, format );

	const int result = vsnprintf( buffer,
		sizeof( buffer ) - 1,
		format,
		args );
	buffer[ sizeof( buffer ) - 1 ] = '\0';

	va_end( args );

	OutputDebugStringA( buffer );

	return result;
}
} // end namespace wgt
