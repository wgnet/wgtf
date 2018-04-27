#include "plugin_context_manager.hpp"

#include "default_context_manager.hpp"

#include "core_common/assert.hpp"
namespace wgt
{
PluginContextManager::PluginContextManager()
    : globalContext_(new DefaultComponentContext(L"root")), executablepath_(nullptr)
{
	connections_ += globalContext_->registerListener(*this);
}

PluginContextManager::~PluginContextManager()
{
}

IComponentContext* PluginContextManager::createContext(const PluginId& id, const std::wstring& path)
{
	// Create context
	auto pluginContext = new DefaultComponentContext(id, globalContext_.get());

	// Insert in context list
	auto metaData = std::unique_ptr<ContextMetaData>(new ContextMetaData());
	metaData->context_ = pluginContext;
	metaData->contextName_ = path;
	metaData->connections_ += globalContext_->registerListener(*pluginContext);
	contexts_.insert(std::make_pair(id, std::move(metaData)));

	// Create ContextCreators and register them
	for (auto& it : contextCreators_)
	{
		auto contextCreator = it.second;

		// Create context
		auto pInterface = contextCreator->createContext(id.c_str());
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
		return findIt->second->context_;
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
		contexts_.erase(findIt);
	}
}

void PluginContextManager::onContextCreatorRegistered(IComponentContextCreator* contextCreator)
{
	// Add ContextCreator to list
	TF_ASSERT(contextCreators_.find(contextCreator->getType()) == contextCreators_.end());
	contextCreators_.insert(std::make_pair(contextCreator->getType(), contextCreator));

	// Register interface for ContextCreator
	for (auto& context : contexts_)
	{
		auto child = context.second->context_->registerInterfaceImpl(
		contextCreator->getType(), contextCreator->createContext(context.second->contextName_.c_str()),
		IComponentContext::Reg_Local);
		childContexts_[contextCreator].push_back(child);
	}

	auto child = globalContext_->registerInterfaceImpl(
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
		TF_ASSERT(findIt != childContexts_.end());
		for (auto& child : findIt->second)
		{
			for (auto& contextIt : contexts_)
			{
				auto pChild = child.lock();
				if (pChild == nullptr)
				{
					continue;
				}
				if (contextIt.second->context_->deregisterInterface(pChild.get()))
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
