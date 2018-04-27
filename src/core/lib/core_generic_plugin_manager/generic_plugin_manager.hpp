#ifndef GENERIC_PLUGIN_MANAGER_HPP
#define GENERIC_PLUGIN_MANAGER_HPP

#include "core_common/platform_dll.hpp"
#include "core_generic_plugin/interfaces/i_memory_allocator.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <functional>

namespace wgt
{
class IPluginContextManager;

class GenericPluginManager
{
public:
	typedef std::vector<HMODULE> PluginList;
	typedef std::vector<std::wstring> PluginNameList;
	typedef std::unordered_map<std::wstring, GenericPluginLoadState> PluginStateMap;

	GenericPluginManager(bool applyDebugPostfix = true,
		bool applyHybridPostfix = true);
	virtual ~GenericPluginManager();

	void loadPlugins(const PluginNameList& plugins);
	void unloadPlugins(const PluginNameList& plugins);
	void unloadPlugins(const PluginList& plugins);

	void runLoadStep(const PluginNameList& plugins);
	void runInitiliseStep(const PluginNameList& pluginNames);
	// these finalize and unload in reverse order
	void runFinaliseStep(const PluginNameList& plugins);
	void runUnloadStep(const PluginNameList& plugins);
	void runDestroyStep(const PluginNameList& plugins, bool destroyGlobal = false);

	IPluginContextManager& getContextManager() const;

	template <class T>
	T* queryInterface()
	{
		return reinterpret_cast<T*>(queryInterface(typeid(T).name()));
	}

private:
	void* queryInterface(const char* name) const;
	PluginList generateList(PluginNameList pluginNames, bool reverse);
	int pluginCountInState(GenericPluginLoadState state, bool findNotInState = false);

	typedef std::function<bool(HMODULE)> NotifyFunction;
	void notifyPlugins(const PluginList& plugins, NotifyFunction func);

	HMODULE loadPlugin(const std::wstring& filename);
	bool unloadPlugin(HMODULE hPlugin);
	void unloadContext(HMODULE hPlugin);

	typedef std::unordered_map<std::wstring, HMODULE> PluginMap;
	PluginMap::iterator findPlugin(HMODULE hPlugin);

	std::wstring processPluginFilename(const std::wstring& filename);

	PluginMap plugins_;
	PluginNameList pluginLoadOrder_;
	PluginStateMap pluginStates_;

	std::map<std::wstring, IMemoryAllocator*> memoryContext_;
	std::unique_ptr<IPluginContextManager> contextManager_;
	bool applyDebugPostfix_;
	bool applyHybridPostfix_;
};
} // end namespace wgt
#endif // GENERIC_PLUGIN_MANAGER_HPP
