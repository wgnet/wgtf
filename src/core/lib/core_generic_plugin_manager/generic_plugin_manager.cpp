#include "generic_plugin_manager.hpp"

#include "core_dependency_system/i_interface.hpp"

#include "core_generic_plugin_manager/plugin_static_initializer.hpp"

#include "core_generic_plugin/env_context.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_generic_plugin/interfaces/i_memory_allocator.hpp"
#include "notify_plugin.hpp"
#include "plugin_context_manager.hpp"

#include "core_common/assert.hpp"
#include "core_common/platform_dbg.hpp"
#include "core_common/platform_env.hpp"
#include "core_common/platform_dll.hpp"
#include "core_common/platform_path.hpp"
#include "common_include/i_static_initializer.hpp"

#include "core_logging/logging.hpp"

#include <algorithm>
#include <iterator>
#include <cstdint>

#ifdef _WIN32
#include <shlwapi.h>
#endif // _WIN32

namespace wgt
{

//==============================================================================
class PluginStaticInitializerContextCreator : public Implements<IComponentContextCreator>
{
public:
	InterfacePtr createContext(const wchar_t* contextId)
	{
		return std::make_shared<InterfaceHolder<PluginStaticInitializer>>(new PluginStaticInitializer, true);
	}

	const char* getType() const
	{
		return typeid(IStaticInitalizer).name();
	}
};

//==============================================================================
GenericPluginManager::GenericPluginManager(bool applyDebugPostfix,
	bool applyHybridPostfix)
    : contextManager_(new PluginContextManager())
	, applyDebugPostfix_(applyDebugPostfix)
	, applyHybridPostfix_(applyHybridPostfix)
{
	contextManager_->getGlobalContext()->registerInterface(new PluginStaticInitializer);
	contextManager_->getGlobalContext()->registerInterface(new PluginStaticInitializerContextCreator);
}

//==============================================================================
GenericPluginManager::~GenericPluginManager()
{
	TF_ASSERT(pluginCountInState(Initialise, true) == 0);
	auto toUnload = pluginLoadOrder_;
	runFinaliseStep(toUnload);
	runUnloadStep(toUnload);
	runDestroyStep(toUnload, true);
}

//==============================================================================
void GenericPluginManager::loadPlugins(const PluginNameList& pluginNames)
{
	runLoadStep(pluginNames);
	runInitiliseStep(pluginNames);
}

//==============================================================================
void GenericPluginManager::unloadPlugins(const PluginNameList& pluginNames)
{
	runFinaliseStep(pluginNames);
	runUnloadStep(pluginNames);
	runDestroyStep(pluginNames, false);
}

//==============================================================================
void GenericPluginManager::runLoadStep(const PluginNameList& pluginNames)
{
	PluginList plgs;
	std::transform(std::begin(pluginNames), std::end(pluginNames), std::back_inserter(plgs),
	               std::bind(&GenericPluginManager::loadPlugin, this, std::placeholders::_1));

	notifyPlugins(plgs, NotifyPlugin(*this, GenericPluginLoadState::Create));

	notifyPlugins(plgs, NotifyPluginPostLoad(*this));

	for (const auto& name : pluginNames)
	{
		pluginStates_[name] = PostLoad;
	}
}

//==============================================================================
void GenericPluginManager::runInitiliseStep(const PluginNameList& pluginNames)
{
	PluginList plgs = generateList(pluginNames, false);
	notifyPlugins(plgs, NotifyPlugin(*this, GenericPluginLoadState::Initialise));

	for (const auto& name : pluginNames)
	{
		TF_ASSERT(pluginStates_.find(name) != pluginStates_.end() && pluginStates_[name] == PostLoad);

		pluginStates_[name] = Initialise;
		pluginLoadOrder_.push_back(name);
	}
}
//==============================================================================
void GenericPluginManager::unloadPlugins(const PluginList& plugins)
{
	if (plugins.empty())
	{
		return;
	}

	PluginNameList pluginNames;

	for (const auto& handle : plugins)
	{
		for (const auto& pluginPair : plugins_)
		{
			if (handle == pluginPair.second)
			{
				pluginNames.push_back(pluginPair.first);
			}
		}
	}
	unloadPlugins(pluginNames);
}

//==============================================================================
void GenericPluginManager::runFinaliseStep(const PluginNameList& pluginNames)
{
	if (pluginNames.empty())
	{
		return;
	}

	PluginList list = generateList(pluginNames, true);

	for (int state = Finalise; state < Unload; ++state)
	{
		notifyPlugins(list, NotifyPlugin(*this, (GenericPluginLoadState)state));
	}

	for (const auto& name : pluginNames)
	{
		auto it = std::find(pluginLoadOrder_.begin(), pluginLoadOrder_.end(), name);
		if (it != pluginLoadOrder_.end())
		{
			pluginLoadOrder_.erase(it);
			pluginStates_[name] = Finalise;
		}
		else
		{
			TF_ASSERT(false && "Attempting to unload plugin that isn't loaded");
		}
	}
}
//==============================================================================
void GenericPluginManager::runUnloadStep(const PluginNameList& pluginNames)
{
	if (pluginNames.empty())
	{
		return;
	}
	PluginList list = generateList(pluginNames, true);

	for (int state = Unload; state < Destroy; ++state)
	{
		notifyPlugins(list, NotifyPlugin(*this, (GenericPluginLoadState)state));
	}

	for (const auto& name : pluginNames)
	{
		TF_ASSERT(pluginStates_.find(name) != pluginStates_.end() && pluginStates_[name] == Finalise);
		pluginStates_[name] = Unload;
	}
}
//==============================================================================
void GenericPluginManager::runDestroyStep(const PluginNameList& pluginNames, bool destroyGlobal)
{
	if (pluginNames.empty())
	{
		return;
	}

	PluginList list = generateList(pluginNames, true);

	// Notify plugins of destroy - Matches Create notification
	notifyPlugins(list, NotifyPlugin(*this, Destroy));

	// Do in reverse order of load
	std::for_each(std::begin(list), std::end(list),
	              std::bind(&GenericPluginManager::unloadContext, this, std::placeholders::_1));

	if (destroyGlobal)
	{
		unloadContext(NULL);
	}

	for (auto it = memoryContext_.begin(); it != memoryContext_.end(); ++it)
	{
		it->second->fini();
	}

	// Calls FreeLibrary - matches loadPlugin() LoadLibraryW
	std::for_each(std::begin(list), std::end(list),
	              std::bind(&GenericPluginManager::unloadPlugin, this, std::placeholders::_1));

	for (auto it = memoryContext_.begin(); it != memoryContext_.end(); ++it)
	{
		::OutputDebugString(it->first.c_str());
		::OutputDebugString(L"\n");
		delete it->second;
	}

	for (const auto& name : pluginNames)
	{
		TF_ASSERT(pluginStates_.find(name) != pluginStates_.end() && pluginStates_[name] == Unload);
		plugins_.erase(name);
		pluginStates_.erase(name);
	}

	memoryContext_.clear();
}

//==============================================================================
void GenericPluginManager::notifyPlugins(const PluginList& plugins, NotifyFunction func)
{
	std::for_each(std::begin(plugins), std::end(plugins), func);
}

//==============================================================================
HMODULE GenericPluginManager::loadPlugin(const std::wstring& filename)
{
	std::string errorMsg;
	auto processedFileName = processPluginFilename(filename);

	auto pluginContext = contextManager_->createContext(processedFileName, filename);
    PluginInitDelegate initDelegate =
              [pluginContext]( std::function<void(IComponentContext&)> initFunc)
              {
                  initFunc(*pluginContext);
              };
	setPluginInitDelegate(&initDelegate);
	HMODULE hPlugin = ::LoadLibraryW(processedFileName.c_str());
	// Must get last error before doing anything else
	const bool hadError = FormatLastErrorMessage(errorMsg);
	setPluginInitDelegate(nullptr);

	if (hPlugin != nullptr)
	{
		plugins_[processedFileName] = hPlugin;
	}
	else
	{
		contextManager_->destroyContext(processedFileName);

		NGT_ERROR_MSG("Could not load plugin %S (from %S): %s\n", filename.c_str(), processedFileName.c_str(),
		              hadError ? errorMsg.c_str() : "Unknown error");

#if defined(_DEBUG)
		// Fail automated tests
		const auto pCommandLine = this->queryInterface<ICommandLineParser>();
		const auto requireAllSpecifiedPlugins = (pCommandLine == nullptr) ? true : pCommandLine->getFlag("-unattended");

		if (requireAllSpecifiedPlugins)
		{
			TF_ASSERT(false && "Could not load plugin");
		}
#endif // defined( _DEBUG )
	}
	return hPlugin;
}

//==============================================================================
GenericPluginManager::PluginMap::iterator GenericPluginManager::findPlugin(HMODULE hPlugin)
{
	return std::find_if(plugins_.begin(), plugins_.end(),
	                    [&](PluginMap::value_type& p) { return hPlugin == p.second; });
}

//==============================================================================
void GenericPluginManager::unloadContext(HMODULE hPlugin)
{
	IComponentContext* contextManager = contextManager_->getGlobalContext();
	IPluginContextManager::PluginId pluginId;

	if (hPlugin != NULL)
	{
		PluginMap::iterator it = findPlugin(hPlugin);
		if (it == std::end(plugins_))
		{
			return;
		}
		contextManager = contextManager_->getContext(it->first);
		pluginId = it->first;
	}

	IMemoryAllocator* memoryAllocator = contextManager->queryInterface<IMemoryAllocator>();
	contextManager_->destroyContext(pluginId);
	memoryContext_.insert(std::make_pair(pluginId, memoryAllocator));
}

//==============================================================================
bool GenericPluginManager::unloadPlugin(HMODULE hPlugin)
{
	if (!hPlugin)
	{
		return false;
	}

	PluginMap::iterator it = findPlugin(hPlugin);
	TF_ASSERT(it != std::end(plugins_));

	::FreeLibrary(hPlugin);
	plugins_.erase(it);

	return true;
}

//==============================================================================
IPluginContextManager& GenericPluginManager::getContextManager() const
{
	return *contextManager_.get();
}

//==============================================================================
void* GenericPluginManager::queryInterface(const char* name) const
{
	return getContextManager().getGlobalContext()->queryInterface(name);
}

//==============================================================================
std::wstring GenericPluginManager::processPluginFilename(const std::wstring& filename)
{
	// PathCanonicalize does not convert '/' to '\\'
	wchar_t normalisedPath[MAX_PATH];
	std::copy(filename.c_str(), filename.c_str() + filename.size(), normalisedPath);
	normalisedPath[filename.size()] = L'\0';

#ifdef _WIN32
	std::replace(normalisedPath, normalisedPath + filename.size(), L'/', L'\\');
#elif __APPLE__
	std::replace(normalisedPath, normalisedPath + filename.size(), L'\\', L'/');
	wchar_t file[MAX_PATH];
	PathFileName(file, normalisedPath);
	PathRemoveFileSpec(normalisedPath);
	PathAppend(normalisedPath, L"lib");
	PathAppend(normalisedPath, file);
#endif

	wchar_t temp[MAX_PATH];

	if (PathIsRelative(normalisedPath))
	{
#ifdef __APPLE__
		wcpcpy(temp, normalisedPath);
#elif _WIN32
		wchar_t exePath[MAX_PATH];
		if (contextManager_->getExecutablePath())
		{
			mbstowcs(exePath, contextManager_->getExecutablePath(), strlen(contextManager_->getExecutablePath()) + 1);
		}
		else
		{
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
		}
		PathAppend(exePath, normalisedPath);
		PathCanonicalize(temp, exePath);
#endif
	}
	else
	{
		PathCanonicalize(temp, normalisedPath);
	}

	PathRemoveExtension(temp);

#ifdef _DEBUG
	if (applyDebugPostfix_)
	{
		const size_t len = ::wcsnlen(temp, MAX_PATH);
		if (::wcsncmp(temp + len - 2, L"_d", 2) != 0)
		{
			wcscat(temp, L"_d");
		}
	}
#endif
#ifdef _HYBRID
	if (applyHybridPostfix_)
	{
		const size_t len = ::wcsnlen(temp, MAX_PATH);
		if (::wcsncmp(temp + len - 2, L"_h", 2) != 0)
		{
			wcscat(temp, L"_h");
		}
	}
#endif

	AddDllExtension(temp);

	return temp;
}

GenericPluginManager::PluginList GenericPluginManager::generateList(PluginNameList pluginNames, bool reverse)
{
	PluginList result;

	if (reverse)
	{
		for (auto it = pluginNames.crbegin(); it != pluginNames.crend(); ++it)
		{
			result.push_back(plugins_[processPluginFilename(*it)]);
		}
	}
	else
	{
		for (auto it = pluginNames.cbegin(); it != pluginNames.cend(); ++it)
		{
			result.push_back(plugins_[processPluginFilename(*it)]);
		}
	}

	return result;
}

int GenericPluginManager::pluginCountInState(GenericPluginLoadState state, bool findNotInState)
{
	int result = 0;
	for (const auto& statePair : pluginStates_)
	{
		if ((!findNotInState && statePair.second == state) || (findNotInState && statePair.second != state))
		{
			++result;
		}
	}
	return result;
}

} // end namespace wgt
