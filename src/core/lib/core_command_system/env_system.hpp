#ifndef ENV_SYSTEM
#define ENV_SYSTEM

#include "i_env_system.hpp"
#include "core_dependency_system/i_interface.hpp"
#include <vector>
#include <string>

namespace wgt
{
class EnvState : public IEnvState
{
public:
	EnvState( const char* description ) : description_(description) {}
	virtual ~EnvState() {}

	virtual void add( IEnvComponentPtr ec ) override;
	virtual IEnvComponentPtr remove( const ECGUID& guid ) override;
	virtual IEnvComponent* query( const ECGUID& guid ) const override;
	virtual const char* description() const override { return description_.c_str(); }

private:
	typedef std::vector<IEnvComponentPtr> Components;
	Components components_;
	std::string description_;
};

class EnvManager : public Implements< IEnvManager >
{
public:
	EnvManager() : idx_(0) {}
	virtual ~EnvManager() {}

	virtual void registerListener( IEnvEventListener* listener ) override;
	virtual void deregisterListener( IEnvEventListener* listener ) override;

	virtual int addEnv( const char* description ) override;
	virtual void removeEnv( int id ) override;
	virtual void selectEnv( int id ) override;
    virtual void saveEnvState( int id ) override;
    virtual void loadEnvState( int id ) override;

private:
	typedef std::unique_ptr<IEnvState> IEnvStatePtr;
	typedef std::vector<std::pair<int, IEnvStatePtr>> StateVec;
	StateVec stateVec_;
	int idx_;

	typedef std::vector<IEnvEventListener*> Listeners;
	Listeners listeners_;
};
} // end namespace wgt
#endif // ENV_SYSTEM
