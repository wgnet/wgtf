#ifndef I_ENV_SYSTEM
#define I_ENV_SYSTEM

#include "core_common/assert.hpp"
#include "core_common/wg_read_write_lock.hpp"
#include "core_reflection/object_handle.hpp"

#include <type_traits>
#include <memory>
#include <string>

namespace wgt
{
class IDataStream;
class IViewport;
typedef std::string EnvironmentId;

template <typename T>
class LockedStateT;

class IEnvComponentState
{
public:
	//--------------------------------------------------------------------------
	virtual ~IEnvComponentState()
	{
	}

	//--------------------------------------------------------------------------
	virtual void saveState(IDataStream& stream)
	{
	}

	//--------------------------------------------------------------------------
	virtual void loadState(IDataStream& stream)
	{
	}
};

class EmptyEnvComponentState : public IEnvComponentState
{
};

class LockedState
{
public:
	//--------------------------------------------------------------------------
	LockedState(IEnvComponentState* state, wg_read_write_lock& lock) : state_(state), guard_(lock)
	{
	}

	//--------------------------------------------------------------------------
	LockedState(IEnvComponentState* state, wg_read_lock_guard& guard) : state_(state), guard_(guard)
	{
	}

	//--------------------------------------------------------------------------
	LockedState(LockedState&& other) : state_(other.state_), guard_(std::move(other.guard_))
	{
	}

	//--------------------------------------------------------------------------
	IEnvComponentState* envComponentState() const
	{
		return state_;
	}

	//--------------------------------------------------------------------------
	wg_read_lock_guard guard() const
	{
		return guard_;
	}

protected:
	IEnvComponentState* state_;
private:
	LockedState(const LockedState& other);
	LockedState& operator=(const LockedState& other);
	mutable wg_read_lock_guard guard_;
};

template <class T>
class LockedStateT : public LockedState
{
public:
	//--------------------------------------------------------------------------
	LockedStateT(T* state, wg_read_write_lock& lock) : LockedState(state, lock)
	{
		static_assert(std::is_base_of<IEnvComponentState, T>::value, "Type needs to inherit from IEnvComponentState");
	}

	//--------------------------------------------------------------------------
	LockedStateT(T* state, wg_read_lock_guard&& guard) : LockedState(state, std::move(guard))
	{
		static_assert(std::is_base_of<IEnvComponentState, T>::value, "Type needs to inherit from IEnvComponentState");
	}

	//--------------------------------------------------------------------------
	LockedStateT(LockedStateT&& other) : LockedState(std::move(other))
	{
	}

	//--------------------------------------------------------------------------
	~LockedStateT()
	{
	}

	//--------------------------------------------------------------------------
	T& operator*() const
	{
		return *static_cast<T*>(state_);
	}

	//--------------------------------------------------------------------------
	T* operator->() const
	{
		return static_cast<T*>(state_);
	}

private:
	friend class LockedState;
	LockedStateT(const LockedStateT& other);
};

template <typename T>
LockedStateT<T> safeCast(const LockedState& other)
{
	auto state = dynamic_cast<T*>(other.envComponentState());
	TF_ASSERT(state != nullptr);
	return LockedStateT<T>(state, other.guard());
}

class IEnvComponent
{
public:
	virtual ~IEnvComponent() = default;
	virtual const char* getEnvComponentId() const = 0;
	virtual std::unique_ptr<IEnvComponentState> createState() = 0;
	virtual LockedState getActiveState() = 0;
	virtual LockedState getStateByEnvId(const EnvironmentId& envId) = 0;
	virtual void onPreEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) {}
	virtual void onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) {}
	virtual void onEnvironmentRemoved(const EnvironmentId& id) {}
	virtual void onEnvironmentCreated(const EnvironmentId& id) {}
};

class IEnvManager
{
public:
	//--------------------------------------------------------------------------
	virtual ~IEnvManager()
	{
	}

	//--------------------------------------------------------------------------
	virtual void saveEnvironmentState(const char* envId)
	{
	}

	//--------------------------------------------------------------------------
	virtual void loadEnvironmentState(const char* envId)
	{
	}

	//--------------------------------------------------------------------------
	virtual const EnvironmentId& createNewEnvironment(const EnvironmentId& customId = "",
	                                                  IViewport* viewport = nullptr) = 0;
	virtual const EnvironmentId& getActiveEnvironmentId() const = 0;
	virtual std::string getEnvironmentPreferenceFileName(const EnvironmentId& envId) const = 0;
	virtual void switchEnvironment(const EnvironmentId& newEnvId) = 0;
	virtual void switchToDefaultEnvironment() = 0;
	virtual void removeEnvironment(const EnvironmentId& envId) = 0;
	virtual void resetCurrentEnvironment() = 0;
	virtual void registerEnvComponent(IEnvComponent* envComponent) = 0;
	virtual void deregisterEnvComponent(IEnvComponent* envComponent) = 0;
	virtual LockedState getActiveStateForEnvComponent(const IEnvComponent* envComponent) const = 0;
	virtual LockedState getStateForEnvComponentByEnvId(const EnvironmentId& envId, const IEnvComponent* envComponent) const = 0;
	virtual IEnvComponent* findEnvironmentComponent(const char* extension) = 0;

	static EnvironmentId defaultEnviromentId;
};

template <class T, bool is_default_constructible = std::is_default_constructible<T>::value>
class EnvComponent : public IEnvComponent
{
public:
	//--------------------------------------------------------------------------
	EnvComponent(IEnvManager& envManager) : envManager_(envManager)
	{
		static_assert(std::is_base_of<IEnvComponentState, T>::value, "Type needs to inherit from IEnvComponentState");
	}

	//--------------------------------------------------------------------------
	virtual ~EnvComponent()
	{
	}

	//--------------------------------------------------------------------------
	virtual std::unique_ptr<IEnvComponentState> createState() override
	{
		return std::unique_ptr<IEnvComponentState>(new T());
	}

	//--------------------------------------------------------------------------
	virtual LockedState getActiveState() override
	{
		return envManager_.getActiveStateForEnvComponent(this);
	}

	//--------------------------------------------------------------------------
	virtual LockedState getStateByEnvId(const EnvironmentId& envId) override
	{
		return envManager_.getStateForEnvComponentByEnvId(envId, this);
	}

protected:
	IEnvManager& envManager_;
};

template <class T>
class EnvComponent<T, false> : public IEnvComponent
{
public:
	//--------------------------------------------------------------------------
	EnvComponent(IEnvManager& envManager) : envManager_(envManager)
	{
		static_assert(std::is_base_of<IEnvComponentState, T>::value, "Type needs to inherit from IEnvComponentState");
	}

	//--------------------------------------------------------------------------
	virtual ~EnvComponent()
	{
	}

	//--------------------------------------------------------------------------
	virtual LockedState getActiveState() override
	{
		return envManager_.getActiveStateForEnvComponent(this);
	}

	//--------------------------------------------------------------------------
	virtual LockedState getStateByEnvId(const EnvironmentId& envId) override
	{
		return envManager_.getStateForEnvComponentByEnvId(envId, this);
	}

protected:
	IEnvManager& envManager_;
};

template <class T>
class EnvComponentT : public EnvComponent<T>
{
public:
	//--------------------------------------------------------------------------
	EnvComponentT(IEnvManager& envManager) : EnvComponent<T>(envManager)
	{
		static_assert(std::is_base_of<IEnvComponentState, T>::value, "Type needs to inherit from IEnvComponentState");
	}

	//--------------------------------------------------------------------------
	virtual ~EnvComponentT()
	{
	}

	//--------------------------------------------------------------------------
	void initEnvComponent()
	{
		this->envManager_.registerEnvComponent(this);
	}

	//--------------------------------------------------------------------------
	void finiEnvComponent()
	{
		this->envManager_.deregisterEnvComponent(this);
	}

	//--------------------------------------------------------------------------
	LockedStateT<T> getActiveStateT()
	{
		auto state = this->envManager_.getActiveStateForEnvComponent(this);
		return safeCast<T>(state);
	}

	//--------------------------------------------------------------------------
	LockedStateT<T> getActiveStateT() const
	{
		auto state = this->envManager_.getActiveStateForEnvComponent(this);
		return safeCast<T>(state);
	}

	//--------------------------------------------------------------------------
	LockedStateT<T> getStateTByEnvId(const EnvironmentId& envId)
	{
		auto state = this->envManager_.getStateForEnvComponentByEnvId(envId, this);
		return safeCast<T>(state);
	}

	//--------------------------------------------------------------------------
	LockedStateT<T> getStateTByEnvId(const EnvironmentId& envId) const
	{
		auto state = this->envManager_.getStateForEnvComponentByEnvId(envId, this);
		return safeCast<T>(state);
	}
};

} // end namespace wgt
#endif // I_ENV_SYSTEM
