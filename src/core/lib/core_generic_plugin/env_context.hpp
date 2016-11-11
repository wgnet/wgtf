#ifndef ENV_CONTEXT_HPP
#define ENV_CONTEXT_HPP

#include "core_common/platform_env.hpp"
#include "common_include/env_pointer.hpp"

namespace wgt
{
class IComponentContext;
namespace
{
const char* ENV_VAR_NAME = "PLUGIN_CONTEXT_PTR";

void setEnvContext(IComponentContext* context)
{
	setPointer(ENV_VAR_NAME, context);
}

IComponentContext* getEnvContext()
{
	return getPointerT<IComponentContext>(ENV_VAR_NAME);
}
}
} // end namespace wgt
#endif // ENV_CONTEXT_HPP
