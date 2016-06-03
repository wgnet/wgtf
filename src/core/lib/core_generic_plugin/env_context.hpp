#ifndef ENV_CONTEXT_HPP
#define ENV_CONTEXT_HPP

#include "core_common/platform_env.hpp"

namespace wgt
{
namespace
{
	const char * ENV_VAR_NAME = "PLUGIN_CONTEXT_PTR";

	void setEnvContext( IComponentContext* context )
	{
		if (context)
		{
			auto ptr = reinterpret_cast< uintptr_t >( context );
			char buf[33] = {};
			size_t i = sizeof(buf) - 2;
			while (true)
			{
				char digit = ptr % 16;

				if (digit < 10)
				{
					buf[i] = '0' + digit;
				}
				else
				{
					buf[i] = 'a' + digit - 10;
				}

				ptr = ptr / 16;

				if (ptr == 0 || i == 0)
				{
					break;
				}

				--i;
			}

			Environment::setValue( ENV_VAR_NAME, buf + i );
		}
		else
		{
			Environment::unsetValue( ENV_VAR_NAME );
		}
	}

	IComponentContext * getEnvContext()
	{
		char buf[33] = {};
		if (Environment::getValue( ENV_VAR_NAME, buf ))
		{
			// convert hex string to pointer value
			uintptr_t ptr = 0;
			for (const char* pc = buf; *pc; ++pc )
			{
				// to lower case
				char c = *pc | 0x20;
				uintptr_t digit = 0;
				if (c >= '0' && c <= '9')
				{
					digit = c - '0';
				}
				else if (c >= 'a' && c <= 'f')
				{
					digit = c - 'a' + 10;
				}
				else
				{
					break;
				}

				ptr = ptr * 16 + digit;
			}

			return reinterpret_cast< IComponentContext* >( ptr );
		}
		return nullptr;
	}
}
} // end namespace wgt
#endif//ENV_CONTEXT_HPP
