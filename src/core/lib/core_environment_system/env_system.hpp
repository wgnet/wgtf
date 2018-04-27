#ifndef ENV_SYSTEM
#define ENV_SYSTEM

#include "i_env_system.hpp"
#include "core_dependency_system/i_interface.hpp"
#include <memory>

namespace wgt
{
class EnvManager : public Implements<IEnvManager>
{
public:
	EnvManager();
	~EnvManager();
	void initialise();
	void finalise();

	virtual const EnvironmentId& createNewEnvironment(const EnvironmentId& customId, IViewport* viewport) override;
	virtual const EnvironmentId& getActiveEnvironmentId() const override;
	virtual std::string getEnvironmentPreferenceFileName(const EnvironmentId& envId) const override;
	virtual void switchEnvironment(const EnvironmentId& newEnvId) override;
	virtual void switchToDefaultEnvironment() override;
	virtual void removeEnvironment(const EnvironmentId& envId) override;
	virtual void resetCurrentEnvironment() override;
	virtual void registerEnvComponent(IEnvComponent* envComponent) override;
	virtual void deregisterEnvComponent(IEnvComponent* envComponent) override;
	virtual LockedState getActiveStateForEnvComponent(const IEnvComponent* envComponent) const override;
	virtual LockedState getStateForEnvComponentByEnvId(const EnvironmentId& envId, const IEnvComponent* envComponent) const override;
	virtual IEnvComponent* findEnvironmentComponent(const char* extension) override;

private:
	struct EnvManagerImpl;
	std::unique_ptr<EnvManagerImpl> pImpl_;
};
} // end namespace wgt
#endif // ENV_SYSTEM
