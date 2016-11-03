#include "platform_env.hpp"
#include <stdlib.h>
#include <cstring>

#if defined( _WIN32 )
#include <windows.h>
#endif

namespace wgt
{
bool Environment::getValue( const char* name, char* value, size_t valueSize )
{
	if (!name || !value)
	{
		return false;
	}

	#if defined( _WIN32 )
	size_t len;
	getenv_s(&len, value, valueSize, name);
	return len > 0 && len < valueSize;
	#endif

	#ifdef __APPLE__
	const char* var = getenv( name );
	if (!var || strlen(var) > valueSize)
	{
		return false;
	}

	strcpy(value, var);

	return true;
	#endif // __APPLE__
}


bool Environment::setValue( const char* name, const char* value )
{
	if (!name || !value)
	{
		return false;
	}

	#if defined( _WIN32 )
	int ret = _putenv_s(name, value);
	#endif

	#ifdef __APPLE__
	int ret = setenv( name, value , 1 );
	#endif // __APPLE__

	return ret != 0;
}


bool Environment::unsetValue( const char* name )
{
	if (!name)
	{
		return false;
	}

	#if defined( _WIN32 )
	int ret = SetEnvironmentVariableA( name, NULL );
	#endif

	#ifdef __APPLE__
	int ret = unsetenv( name );
	#endif // __APPLE__

	return ret != 0;
}
} // end namespace wgt
