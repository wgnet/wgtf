#include "env_system.hpp"
#include <assert.h>
#include <algorithm>

namespace wgt
{
EnvState::EnvState(const char* description, int id) : description_(description), id_(id)
{
}

int EnvState::id() const
{
	return id_;
}

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
	IEnvComponentPtr ptr = std::move(*it);
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

EnvManager::EnvManager() : idx_(0), currentSelected_(INVALID_ID), previousSelected_(INVALID_ID)
{
}

void EnvManager::registerListener(IEnvEventListener* listener)
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

int EnvManager::addEnv(const char* description)
{
	stateVec_.emplace_back(IEnvStatePtr(new EnvState(description, idx_)));
	for (auto& l : listeners_)
	{
		l->onAddEnv(stateVec_.back().get());
	}
	return idx_++;
}

void EnvManager::removeEnv(int id)
{
	auto it =
	find_if(stateVec_.begin(), stateVec_.end(), [id](const StateVec::value_type& x) { return x->id() == id; });
	assert(it != stateVec_.end());
	for (auto& l : listeners_)
	{
		l->onRemoveEnv(it->get());
	}
	stateVec_.erase(it);

	if (currentSelected_ == id)
	{
		currentSelected_ = INVALID_ID;
	}
	if (previousSelected_ == id)
	{
		previousSelected_ = INVALID_ID;
	}
}

void EnvManager::selectEnv(int id)
{
	if (currentSelected_ != id)
	{
		auto it =
		find_if(stateVec_.begin(), stateVec_.end(), [id](const StateVec::value_type& x) { return x->id() == id; });
		assert(it != stateVec_.end());
		for (auto& l : listeners_)
		{
			l->onSelectEnv(it->get());
		}

		previousSelected_ = currentSelected_;
		currentSelected_ = id;
	}
}

void EnvManager::deSelectEnv(int id)
{
	if (currentSelected_ == id)
	{
		currentSelected_ = INVALID_ID;
		if (previousSelected_ != INVALID_ID)
		{
			selectEnv(previousSelected_);
		}
		else
		{
			for (auto& l : listeners_)
			{
				l->onDeselectEnv();
			}
		}
	}
}

void EnvManager::saveEnvState(int id)
{
	auto it =
	find_if(stateVec_.begin(), stateVec_.end(), [id](const StateVec::value_type& x) { return x->id() == id; });
	assert(it != stateVec_.end());
	for (auto& l : listeners_)
	{
		l->onSaveEnvState(it->get());
	}
}

void EnvManager::loadEnvState(int id)
{
	auto it =
	find_if(stateVec_.begin(), stateVec_.end(), [id](const StateVec::value_type& x) { return x->id() == id; });
	assert(it != stateVec_.end());
	for (auto& l : listeners_)
	{
		l->onLoadEnvState(it->get());
	}
}
} // end namespace wgt
