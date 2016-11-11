#ifndef NOTIFY_PLUGIN_HPP
#define NOTIFY_PLUGIN_HPP

#include "generic_plugin_manager.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

namespace wgt
{
typedef bool (*CallbackFunc)(GenericPluginLoadState loadState);
class NotifyPlugin
{
public:
	NotifyPlugin(GenericPluginManager& pluginManager, GenericPluginLoadState loadState);

	bool operator()(HMODULE hPlugin);

private:
	static CallbackFunc GetPluginCallbackFunc(HMODULE hPlugin);

protected:
	GenericPluginManager& pluginManager_;
	GenericPluginLoadState loadState_;
};

class NotifyPluginPostLoad : public NotifyPlugin
{
public:
	NotifyPluginPostLoad(GenericPluginManager& pluginManager);
	~NotifyPluginPostLoad();

	bool operator()(HMODULE hPlugin);

private:
	std::vector<HMODULE> pluginsToUnload_;
};
} // end namespace wgt
#endif // NOTIFY_PLUGIN_HPP
