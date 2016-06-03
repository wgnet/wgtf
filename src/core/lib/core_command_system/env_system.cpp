#include "env_system.hpp"
#include <assert.h>
#include <algorithm>

namespace wgt
{
void EnvState::add(IEnvState::IEnvComponentPtr ec)
{
	assert(std::find_if(components_.begin(), components_.end(), 
		[&](const IEnvComponentPtr& p) { return p->getGUID() == ec->getGUID(); }) == components_.end());
	components_.push_back(std::move(ec));
}

IEnvState::IEnvComponentPtr EnvState::remove(const ECGUID& guid)
{
	auto it = std::find_if(components_.begin(), components_.end(), 
		[&](const IEnvComponentPtr& p) { return p->getGUID() == guid; });
	assert(it != components_.end());
	IEnvComponentPtr ptr = std::move( *it );
	components_.erase(it);
	return ptr;
}

IEnvComponent* EnvState::query(const ECGUID& guid) const 
{
	auto it = std::find_if(components_.begin(), components_.end(), 
		[&](const IEnvComponentPtr& p) { return p->getGUID() == guid; });
	assert(it != components_.end());
	return it->get();
}

void EnvManager::registerListener( IEnvEventListener* listener )
{
	assert(std::find(listeners_.begin(), listeners_.end(), listener) == listeners_.end());
	listeners_.push_back(listener);
}

void EnvManager::deregisterListener(IEnvEventListener* listener)
{
	auto it = std::find(listeners_.begin(), listeners_.end(), listener);
	assert(it != listeners_.end());
	listeners_.erase(it);
}

int EnvManager::addEnv( const char* description )
{
	stateVec_.emplace_back( StateVec::value_type( idx_, IEnvStatePtr( new EnvState(description) ) ) );
	for (auto& l : listeners_)
	{
		l->onAddEnv( stateVec_.back().second.get() );
	}
	return idx_++;
}

void EnvManager::removeEnv(int id)
{
	auto it = find_if(stateVec_.begin(), stateVec_.end(), [=](const StateVec::value_type& x) { return x.first == id; });
	assert(it != stateVec_.end());
	for (auto& l : listeners_)
	{
		l->onRemoveEnv( it->second.get() );
	}
	stateVec_.erase(it);
}

void EnvManager::selectEnv(int id)
{
	auto it = find_if(stateVec_.begin(), stateVec_.end(), [=](const StateVec::value_type& x) { return x.first == id; });
	assert(it != stateVec_.end());
	for (auto& l : listeners_)
	{
		l->onSelectEnv( it->second.get() );
	}
}

void EnvManager::saveEnvState( int id )
{
    auto it = find_if(stateVec_.begin(), stateVec_.end(), [=](const StateVec::value_type& x) { return x.first == id; });
    assert(it != stateVec_.end());
    for (auto& l : listeners_)
    {
        l->onSaveEnvState( it->second.get() );
    }
}

void EnvManager::loadEnvState( int id )
{
    auto it = find_if(stateVec_.begin(), stateVec_.end(), [=](const StateVec::value_type& x) { return x.first == id; });
    assert(it != stateVec_.end());
    for (auto& l : listeners_)
    {
        l->onLoadEnvState( it->second.get() );
    }
}
} // end namespace wgt
