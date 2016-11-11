#ifndef ENVIRONMENT_HPP_INCLUDED
#define ENVIRONMENT_HPP_INCLUDED

#include <cstddef>

// TODO: Environment objects store and manage whole environment. Can be initialized
// from current process environment, can replace current process environment.

namespace wgt
{
/**
Access process environment variables.

Currently just basic access to individual variables is implemented.
*/
class Environment
{
public:
	/**
	Get environment variable value.

	@arg @c name - environment variable name
	@arg @c value - pointer to a buffer that will receive environment variable
	    value
	@arg @c valueSize - size of buffer pointed by @a value in chars

	@return @c true if variable was found and successfully read to @a buffer,
	@c false otherwise. If @a valueSize is insufficient to hold value then
	function fails.

	This version is reentrant and doesn't require dynamic memory, i.e. it
	doesn't call malloc or operator new.
	*/
	static bool getValue(const char* name, char* value, size_t valueSize);

	/**
	Get environment variable value.

	Wrapper for convenience.
	*/
	template <size_t N>
	static bool getValue(const char* name, char (&value)[N])
	{
		return getValue(name, value, N);
	}

	/**
	Set environment variable value.
	*/
	static bool setValue(const char* name, const char* value);

	/**
	Remove variable from environment.
	*/
	static bool unsetValue(const char* name);
};
} // end namespace wgt
#endif
