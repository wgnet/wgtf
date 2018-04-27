#include "env_system.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "interfaces/core_viewport/i_viewport.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include <algorithm>
#include <map>
#include <mutex>
#include <thread>

namespace wgt
{
EnvironmentId IEnvManager::defaultEnviromentId = "default_environment";
class Environment
{
public:
	explicit Environment(const EnvironmentId& id, IViewport* viewport = nullptr) 
		: viewport_(viewport)
	{
		id_ = id.empty() ? RefObjectId::generate().toString() : id;
	}

	~Environment()
	{
		fini();
	}

	const EnvironmentId& getId() const
	{
		return id_;
	}

	const IViewport* getViewport() const
	{
		return viewport_;
	}

	void init()
	{
		if (viewport_)
		{
			viewport_->initialise(id_);
		}
	}

	void reset()
	{
		envComponentStateMap_.clear();
	}

	void fini()
	{
		if (viewport_)
		{
			viewport_->finalise();
			viewport_ = nullptr;
		}
		envComponentStateMap_.clear();
	}

	void registerComponentState(IEnvComponent* envComponent)
	{
		TF_ASSERT(envComponent != nullptr);
		auto componentId = envComponent->getEnvComponentId();
		auto&& findIt = envComponentStateMap_.find(componentId);
		if (findIt != envComponentStateMap_.end())
		{
			// already registered
			return;
		}
		auto state = envComponent->createState();
		TF_ASSERT(state != nullptr);
		envComponentStateMap_[componentId] = std::move(state);
	}

	void deregisterComponentState(IEnvComponent* envComponent)
	{
		TF_ASSERT(envComponent != nullptr);
		auto componentId = envComponent->getEnvComponentId();
		auto&& findIt = envComponentStateMap_.find(componentId);
		if (findIt == envComponentStateMap_.end())
		{
			return;
		}
		envComponentStateMap_.erase(findIt);
	}

	IEnvComponentState* getComponentState(const IEnvComponent* envComponent)
	{
		TF_ASSERT(envComponent != nullptr);
		auto componentId = envComponent->getEnvComponentId();
		auto&& findIt = envComponentStateMap_.find(componentId);
		TF_ASSERT(findIt != envComponentStateMap_.end());
		return findIt->second.get();
	}

	void onSelected(bool selected)
	{
		if (viewport_)
		{
			viewport_->onSelected(selected);
		}
	}

private:
	std::string id_;
	IViewport* viewport_ = nullptr;
	std::map<std::string, std::unique_ptr<IEnvComponentState>> envComponentStateMap_;
};

struct EnvManager::EnvManagerImpl : Depends<IUIApplication>
{
public:
	EnvManagerImpl()
	    : mainThreadId_(std::this_thread::get_id())
		, defaultEnv_(new Environment(IEnvManager::defaultEnviromentId)), activeEnv_(defaultEnv_.get())
	{
	}

	~EnvManagerImpl()
	{
		envMap_.clear();
		envComponentMap_.clear();
		switchEnvironment(nullptr);
	}

	void initialise()
	{
		// Shutdown must happen first before any plugins are finalised
		if (auto uiApplication = get<IUIApplication>())
		{
			connections_.push_back(uiApplication->signalExit.connect(
				std::bind(&EnvManagerImpl::shutdownEnvironments, this)));
		}
	}

	void finalise()
	{
		for (auto& connection : connections_)
		{
			connection.disconnect();
		}
	}

	Environment* createNewEnvironment(const EnvironmentId& customId, IViewport* viewport)
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		auto env = std::make_unique<Environment>(customId, viewport);
		const auto envId = env->getId();
		auto newEnv = env.get();

		TF_ASSERT(envMap_.find(envId) == envMap_.end());
		envMap_[envId] = std::move(env);

		for (auto it : envComponentMap_)
		{
			newEnv->registerComponentState(it.second);
		}

		newEnv->init();
		fireEnvCreated(envId);
		return newEnv;
	}

	void removeEnvironment(const EnvironmentId& envId)
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		bool switchToDefault = false;

		auto&& findIt = envMap_.find(envId);
		if (findIt == envMap_.end())
		{
			return;
		}
		fireEnvRemoved(envId);

		if (findIt->second.get() == activeEnv_)
		{
			switchToDefaultEnvironment();
		}

		// cannot remove default environment
		TF_ASSERT(findIt->second.get() != nullptr && findIt->second.get() != defaultEnv_.get());
		findIt->second->fini();
		envMap_.erase(findIt);
	}

	const EnvironmentId& getActiveEnvironmentId()
	{
		wg_read_lock_guard guard(activeEnvironmentLock_);
		return activeEnv_ ? activeEnv_->getId() : defaultEnv_->getId();
	}

	std::string getEnvironmentPreferenceFileName(const EnvironmentId& envId) const
	{
		wg_read_lock_guard guard(activeEnvironmentLock_);
		auto itr = envMap_.find(envId);
		if (itr != envMap_.end() && itr->second)
		{
			if (auto viewport = itr->second->getViewport())
			{
				auto name = FilePath::getFileWithExtension(viewport->getName());
				std::replace(name.begin(), name.end(), '.', '_');
				return name;
			}
		}
		return "";
	}

	void shutdownEnvironments()
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		switchToDefaultEnvironment();

		std::vector<EnvironmentId> ids;
		for (auto const& pair : envMap_)
		{
			if (pair.first != defaultEnv_->getId())
			{
				ids.push_back(pair.first);
			}
		}
		for (auto& id : ids)
		{
			removeEnvironment(id);
		}
	}

	LockedState getActiveStateForEnvComponent(const IEnvComponent* envComponent)
	{
		wg_read_lock_guard guard(activeEnvironmentLock_);
		TF_ASSERT(activeEnv_ != nullptr);
		auto state = activeEnv_->getComponentState(envComponent);
		TF_ASSERT(state != nullptr);
		return LockedState(state, guard);
	}

	LockedState getStateForEnvComponentByEnvId(const EnvironmentId& envId, const IEnvComponent* envComponent)
	{
		wg_read_lock_guard guard(activeEnvironmentLock_);
		auto& found = envMap_.find(envId);
		TF_ASSERT(found != envMap_.end());
		TF_ASSERT(found->second != nullptr);
		auto state = found->second->getComponentState(envComponent);
		TF_ASSERT(state != nullptr);
		return LockedState(state, guard);
	}

	void switchToDefaultEnvironment()
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		TF_ASSERT(defaultEnv_ != nullptr && activeEnv_ != nullptr);
		EnvironmentId oldId = activeEnv_->getId();
		EnvironmentId newId = defaultEnv_->getId();
		if (defaultEnv_.get() == activeEnv_)
		{
			return;
		}
		firePreEnvChange(oldId, newId);
		{
			switchEnvironment(defaultEnv_.get());
		}
		firePostEnvChange(oldId, newId);
	}

	void switchEnvironment(const EnvironmentId& newId)
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		EnvironmentId oldId = activeEnv_->getId();
		if (activeEnv_->getId() == newId)
		{
			return;
		}
		firePreEnvChange(oldId, newId);
		{
			Environment* newEnv = nullptr;
			if (newId == defaultEnv_->getId())
			{
				newEnv = defaultEnv_.get();
			}
			else
			{
				auto&& findIt = envMap_.find(newId);
				TF_ASSERT(findIt != envMap_.end());
				newEnv = findIt->second.get();
			}

			TF_ASSERT(activeEnv_ != nullptr && newEnv != nullptr);
			if (activeEnv_ == newEnv)
			{
				return;
			}
			switchEnvironment(newEnv);
		}
		firePostEnvChange(oldId, newId);
	}

	void resetCurrentEnvironment()
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		EnvironmentId id = activeEnv_->getId();
		firePreEnvChange(id, id);
		{
			auto&& findIt = envMap_.find(id);
			TF_ASSERT(findIt != envMap_.end());
			wg_write_lock_guard guard(activeEnvironmentLock_);
			findIt->second->reset();
			for (auto it : envComponentMap_)
			{
				findIt->second->registerComponentState(it.second);
			}
		}
		firePostEnvChange(id, id);
	}

	void registerEnvComponent(IEnvComponent* envComponent)
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		TF_ASSERT(envComponent != nullptr);
		auto id = envComponent->getEnvComponentId();
		
		auto findIt = envComponentMap_.find(id);
		if (findIt != envComponentMap_.end())
		{
			// already registered
			return;
		}

		envComponentMap_[id] = envComponent;

		wg_write_lock_guard guard(activeEnvironmentLock_);
		// default env
		defaultEnv_->registerComponentState(envComponent);

		// user created envs
		for (auto&& it : envMap_)
		{
			TF_ASSERT(it.second != nullptr);
			it.second->registerComponentState(envComponent);
		}
	}

	void deregisterEnvComponent(IEnvComponent* envComponent)
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		TF_ASSERT(envComponent != nullptr);
		auto id = envComponent->getEnvComponentId();
		auto findIt = envComponentMap_.find(id);
		if (findIt == envComponentMap_.end())
		{
			return;
		}
		envComponentMap_.erase(findIt);

		wg_write_lock_guard guard(activeEnvironmentLock_);
		// default env
		defaultEnv_->deregisterComponentState(envComponent);

		// user created envs
		for (auto&& it : envMap_)
		{
			TF_ASSERT(it.second != nullptr);
			it.second->deregisterComponentState(envComponent);
		}
	}

	IEnvComponent* findEnvironmentComponent(const char* path)
	{
		TF_ASSERT(mainThreadId_ == std::this_thread::get_id());

		for (auto it : envComponentMap_)
		{
			if (it.first == path)
			{
				return it.second;
			}
		}
		return nullptr;
	}

	IComponentContext* context_;

private:
	void fireEnvRemoved(const EnvironmentId& id)
	{
		for (auto it : envComponentMap_)
		{
			auto envComponent = it.second;
			TF_ASSERT(envComponent != nullptr);
			envComponent->onEnvironmentRemoved(id);
		}
	}

	void firePreEnvChange(const EnvironmentId& oldId, const EnvironmentId& newId)
	{
		for (auto it : envComponentMap_)
		{
			auto envComponent = it.second;
			TF_ASSERT(envComponent != nullptr);
			envComponent->onPreEnvironmentChanged(oldId, newId);
		}
	}

	void firePostEnvChange(const EnvironmentId& oldId, const EnvironmentId& newId)
	{
		for (auto it : envComponentMap_)
		{
			auto envComponent = it.second;
			TF_ASSERT(envComponent != nullptr);
			envComponent->onPostEnvironmentChanged(oldId, newId);
		}
	}

	void fireEnvCreated(const EnvironmentId& id)
	{
		for (auto it : envComponentMap_)
		{
			auto envComponent = it.second;
			TF_ASSERT(envComponent != nullptr);
			envComponent->onEnvironmentCreated(id);
		}
	}

	void switchEnvironment(Environment* env)
	{
		wg_write_lock_guard guard(activeEnvironmentLock_);
		if (activeEnv_)
		{
			activeEnv_->onSelected(false);
		}

		activeEnv_ = env;

		if (activeEnv_)
		{
			activeEnv_->onSelected(true);
		}
	}

	std::thread::id	mainThreadId_;
	std::unique_ptr<Environment> defaultEnv_;
	Environment* activeEnv_;
	std::map<EnvironmentId, std::unique_ptr<Environment>> envMap_;
	std::map<std::string, IEnvComponent*> envComponentMap_;
	mutable wg_read_write_lock activeEnvironmentLock_;
	std::vector<Connection> connections_;
};

EnvManager::EnvManager() : pImpl_(new EnvManagerImpl())
{
}

EnvManager::~EnvManager()
{
}

void EnvManager::initialise()
{
	pImpl_->initialise();
}

void EnvManager::finalise()
{
	pImpl_->finalise();
}

const EnvironmentId& EnvManager::createNewEnvironment(const EnvironmentId& customId, IViewport* viewport)
{
	return pImpl_->createNewEnvironment(customId, viewport)->getId();
}

const EnvironmentId& EnvManager::getActiveEnvironmentId() const
{
	return pImpl_->getActiveEnvironmentId();
}

void EnvManager::switchEnvironment(const EnvironmentId& newEnvId)
{
	pImpl_->switchEnvironment(newEnvId);
}

void EnvManager::switchToDefaultEnvironment()
{
	pImpl_->switchToDefaultEnvironment();
}

void EnvManager::removeEnvironment(const EnvironmentId& envId)
{
	pImpl_->removeEnvironment(envId);
}

void EnvManager::resetCurrentEnvironment()
{
	pImpl_->resetCurrentEnvironment();
}

void EnvManager::registerEnvComponent(IEnvComponent* envComponent)
{
	pImpl_->registerEnvComponent(envComponent);
}

void EnvManager::deregisterEnvComponent(IEnvComponent* envComponent)
{
	pImpl_->deregisterEnvComponent(envComponent);
}

LockedState EnvManager::getActiveStateForEnvComponent(const IEnvComponent* envComponent) const
{
	return pImpl_->getActiveStateForEnvComponent(envComponent);
}

LockedState EnvManager::getStateForEnvComponentByEnvId(const EnvironmentId& envId, const IEnvComponent* envComponent) const
{
	return pImpl_->getStateForEnvComponentByEnvId(envId, envComponent);
}

IEnvComponent* EnvManager::findEnvironmentComponent(const char* extension)
{
	return pImpl_->findEnvironmentComponent(extension);
}

std::string EnvManager::getEnvironmentPreferenceFileName(const EnvironmentId& envId) const
{
	return pImpl_->getEnvironmentPreferenceFileName(envId);
}

} // end namespace wgt
