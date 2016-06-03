#ifndef I_PLUGIN_CONTEXT_MANAGER_HPP
#define I_PLUGIN_CONTEXT_MANAGER_HPP

//#include "dependency_system/i_interface.hpp"
#include <string>

namespace wgt
{
class IComponentContext;

class IPluginContextManager
{
public:
	typedef std::wstring PluginId;

	virtual ~IPluginContextManager() {}
	virtual IComponentContext * createContext(const PluginId & id) = 0;
	virtual IComponentContext * getContext(const PluginId & id) const = 0;
	virtual IComponentContext * getGlobalContext() const = 0;
	virtual void destroyContext(const PluginId & id) = 0;

	virtual void setExecutablePath(const char* path) = 0;
	virtual const char* getExecutablePath() const = 0;
};
} // end namespace wgt
#endif ///I_PLUGIN_CONTEXT_MANAGER_HPP
