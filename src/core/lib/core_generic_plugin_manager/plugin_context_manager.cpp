#include "plugin_context_manager.hpp"
#include "default_context_manager.hpp"

#include <cassert>

namespace wgt
{
PluginContextManager::PluginContextManager() : globalContext_(new DefaultComponentContext()), executablepath_(nullptr)
{
	globalContext_->registerListener(*this);
}

PluginContextManager::~PluginContextManager()
{
	for (auto& it : contexts_)
	{
		delete it.second.first;
	}
	globalContext_->deregisterListener(*this);
}

IComponentContext* PluginContextManager::createContext(const PluginId& id, const std::wstring& path)
{
	// Create context
	auto pluginContext = new DefaultComponentContext(globalContext_.get());

	globalContext_->registerListener(*pluginContext);

	// Insert in context list
	contexts_.insert(std::make_pair(id, std::make_pair(pluginContext, path)));

	// Create ContextCreators and register them
	for (auto& it : contextCreators_)
	{
		auto contextCreator = it.second;

		// Create context
		IInterface* pInterface = contextCreator->createContext(id.c_str());
		const char* type = contextCreator->getType();

		// Register
		pluginContext->registerInterfaceImpl(type, pInterface, IComponentContext::Reg_Local);
		childContexts_[contextCreator].push_back(pInterface);
	}
	return pluginContext;
}

IComponentContext* PluginContextManager::getContext(const PluginId& id) const
{
	auto findIt = contexts_.find(id);
	if (findIt != contexts_.end())
	{
		return findIt->second.first;
	}
	return NULL;
}

IComponentContext* PluginContextManager::getGlobalContext() const
{
	return globalContext_.get();
}

void PluginContextManager::destroyContext(const PluginId& id)
{
	auto findIt = contexts_.find(id);
	if (findIt != contexts_.end())
	{
		globalContext_->deregisterListener(*findIt->second.first);
		delete findIt->second.first;
		contexts_.erase(findIt);
	}
}

void PluginContextManager::onContextCreatorRegistered(IComponentContextCreator* contextCreator)
{
	// Add ContextCreator to list
	assert(contextCreators_.find(contextCreator->getType()) == contextCreators_.end());
	contextCreators_.insert(std::make_pair(contextCreator->getType(), contextCreator));

	// Register interface for ContextCreator
	for (auto context : contexts_)
	{
		IInterface* child = context.second.first->registerInterfaceImpl(
		contextCreator->getType(), contextCreator->createContext(context.second.second.c_str()),
		IComponentContext::Reg_Local);
		childContexts_[contextCreator].push_back(child);
	}

	IInterface* child = globalContext_->registerInterfaceImpl(
	contextCreator->getType(), contextCreator->createContext(L"globalContext"), IComponentContext::Reg_Local);
	childContexts_[contextCreator].push_back(child);
}

void PluginContextManager::onContextCreatorDeregistered(IComponentContextCreator* contextCreator)
{
	// Remove ContextCreator from list
	for (auto it = contextCreators_.begin(); it != contextCreators_.end(); ++it)
	{
		if (contextCreator != it->second)
		{
			continue;
		}
		auto findIt = childContexts_.find(contextCreator);
		assert(findIt != childContexts_.end());
		for (auto& child : findIt->second)
		{
			for (auto& contextIt : contexts_)
			{
				if (contextIt.second.first->deregisterInterface(child))
				{
					break;
				}
			}
		}
		childContexts_.erase(findIt);
		contextCreators_.erase(it);
		return;
	}
}

void PluginContextManager::setExecutablePath(const char* path)
{
	executablepath_ = path;
}

const char* PluginContextManager::getExecutablePath() const
{
	return executablepath_;
}
} // end namespace wgt
