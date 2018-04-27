#ifndef ENV_CONTEXT_HPP
#define ENV_CONTEXT_HPP

#include "common_include/env_pointer.hpp"
#include <functional>

namespace wgt
{
class IComponentContext;

namespace
{
const char* PLUGIN_INIT_VAR_NAME = "PLUGIN_INIT_DELEGATE";

typedef std::function<void(std::function<void(IComponentContext&)>)> PluginInitDelegate;

//------------------------------------------------------------------------------
void setPluginInitDelegate(PluginInitDelegate* initFunc)
{
	return setPointer(PLUGIN_INIT_VAR_NAME, initFunc);
}

//------------------------------------------------------------------------------
PluginInitDelegate* getPluginInitDelegate()
{
	return getPointerT<PluginInitDelegate>(PLUGIN_INIT_VAR_NAME);
}
}
} // end namespace wgt
#endif // ENV_CONTEXT_HPP
