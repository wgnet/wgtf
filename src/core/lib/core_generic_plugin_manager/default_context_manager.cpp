#include "default_context_manager.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_dependency_system/i_interface.hpp"
#include <unordered_map>
#include <algorithm>

namespace wgt
{
namespace
{
static const TypeId getComponentContextCreatorTypeId()
{
	// Must be function to avoid static initialization order problem
	static const TypeId s_ComponentContextCreatorTypeId = TypeId::getType<IComponentContextCreator>();
	return s_ComponentContextCreatorTypeId;
}
}

class RTTIHelper
{
public:
	//==========================================================================
	RTTIHelper(std::shared_ptr<IInterface> pImpl) : pImpl_(pImpl)
	{
	}

	//==========================================================================
	void* queryInterface(const TypeId& name)
	{
		{
			wg_read_lock_guard readGuard(lock_);
			auto&& findIt = typeCache_.find(name);
			if (findIt != typeCache_.end())
			{
				return findIt->second;
			}
		}
		void* found = pImpl_->queryInterface(name);
		{
			wg_write_lock_guard writeGuard(lock_);
			typeCache_.insert(std::make_pair(name, found));
		}
		return found;
	}

	IInterface* getImpl() const
	{
		return pImpl_.get();
	}

private:
	wg_read_write_lock lock_;
	std::shared_ptr<IInterface> pImpl_;
	std::unordered_map<const TypeId, void*> typeCache_;
};

//==============================================================================
DefaultComponentContext::DefaultComponentContext(const std::wstring& name, IComponentContext* parentContext)
    : name_(name), parentContext_(parentContext),
      disconnectSig_(std::make_shared<std::function<void(IComponentContextListener&)>>(
      [this](IComponentContextListener& listener) { deregisterListener(listener); }))
{
}

//==============================================================================
DefaultComponentContext::~DefaultComponentContext()
{
	{
		wg_write_lock_guard writeGuard(lock_);
		for (auto& interface : interfaces_)
		{
			auto iter = std::find_if(registeredInterfaces_.begin(), registeredInterfaces_.end(), [&](const InterfacePtr& i)
			{
				return i.get() == interface.second->getImpl();
			});
			if(iter != registeredInterfaces_.end())
			{
				registeredInterfaces_.erase(iter);
			}
		}
		interfaces_.clear();
	}

	if (parentContext_ == nullptr)
	{
		return;
	}

	{
		wg_read_lock_guard readGuard(lock_);
		for (auto& parentInterface : registeredInterfaces_)
		{
			parentContext_->deregisterInterface(parentInterface.get());
		}
	}

	for (auto itr = registeredInterfaces_.begin(); itr != registeredInterfaces_.end(); itr = registeredInterfaces_.begin())
	{
		registeredInterfaces_.erase(itr);
	}

	TF_ASSERT(listeners_.empty());
}

//==============================================================================
InterfacePtr DefaultComponentContext::registerInterfaceImpl(const TypeId& id, InterfacePtr pImpl,
                                                           ContextRegState regState)
{
	std::shared_ptr<RTTIHelper> rttiHelper;
	{
		wg_write_lock_guard writeGuard(lock_);
		// Use pointer as unique id
		registeredInterfaces_.insert(pImpl);
	}
	if (regState == Reg_Parent && parentContext_ != nullptr)
	{
		return parentContext_->registerInterfaceImpl(id, pImpl, Reg_Local);
	}
	{
		wg_write_lock_guard writeGuard(lock_);
		rttiHelper = interfaces_.insert(std::make_pair(id, std::make_shared<RTTIHelper>(pImpl)))->second;
	}

	auto contextCreator =
	static_cast<IComponentContextCreator*>(pImpl->queryInterface(getComponentContextCreatorTypeId()));
	if (contextCreator)
	{
		ComponentContextListeners copyListeners;
		{
			wg_read_lock_guard readGuard(lock_);
			copyListeners = listeners_;
		}
		
		for (auto& listener : copyListeners)
		{
			listener->onContextCreatorRegistered(contextCreator);
		}
	}
	InterfaceCaster func = [rttiHelper](const wgt::TypeId& typeId){ return rttiHelper->queryInterface(typeId); };
	onInterfaceRegistered(func);
	return pImpl;
}

//------------------------------------------------------------------------------
void DefaultComponentContext::onInterfaceRegistered(InterfaceCaster& caster)
{
	std::set< IComponentContextListener* > called;
	onInterfaceRegistered( caster, called );
}


//------------------------------------------------------------------------------
void DefaultComponentContext::onInterfaceRegistered(
	InterfaceCaster& caster, 
	std::set< IComponentContextListener * > & called )
{
	ComponentContextListeners listenersCopy;
	{
		wg_read_lock_guard readGuard(lock_);
		listenersCopy = listeners_;
	}
	for (auto& listener : listenersCopy)
	{
		if (called.find(listener) == called.end())
		{
			listener->onInterfaceRegistered(caster);
				called.insert(listener);
		}
	}
	{
		wg_read_lock_guard readGuard(lock_);
		if (listenersCopy == listeners_)
		{
			return;
		}
	}
	onInterfaceRegistered( caster, called );
}

//------------------------------------------------------------------------------
void DefaultComponentContext::onInterfaceDeregistered(InterfaceCaster& caster)
{
	ComponentContextListeners listenersCopy;
	{
		wg_read_lock_guard readGuard(lock_);
		listenersCopy = listeners_;
	}
	for (auto& listener : listenersCopy)
	{
		listener->onInterfaceDeregistered(caster);
	}
}

//==============================================================================
bool DefaultComponentContext::deregisterInterface(IInterface* pImpl)
{
	if(pImpl == nullptr)
	{
		TF_ASSERT(!"Shouldn't be calling deregister interface with a null pointer");
		return false;
	}

	std::shared_ptr<RTTIHelper> helper;
	{
		decltype(interfaces_.begin()) it;
		wg_read_lock_guard readGuard(lock_);
		using entry = decltype(*interfaces_.begin());
		it = std::find_if(interfaces_.begin(), interfaces_.end(), [&](const entry& i)
		{
			return pImpl == i.second->getImpl();
		});

		if(it != interfaces_.end())
		{
			helper = it->second;
		}
	}

	if(helper != nullptr)
	{
		ComponentContextListeners copyListeners;
		{
			wg_read_lock_guard readGuard(lock_);
			copyListeners = listeners_;
		}
		auto contextCreator =
		static_cast<IComponentContextCreator*>(pImpl->queryInterface(getComponentContextCreatorTypeId()));
		if (contextCreator)
		{
			for (auto& listener : copyListeners)
			{
				listener->onContextCreatorDeregistered(contextCreator);
			}
		}
		InterfaceCaster function = [helper](const TypeId& typeId)
		{
			return helper->queryInterface(typeId);
		};
		for (auto& listener : copyListeners)
		{
			listener->onInterfaceDeregistered(function);
		}

		wg_write_lock_guard writeGuard(lock_);
		using entry = decltype(*interfaces_.begin());
		auto it = std::find_if(interfaces_.begin(), interfaces_.end(), [&](const entry& i)
		{
			return pImpl == i.second->getImpl();
		});
		if(it !=  interfaces_.end())
		{
			interfaces_.erase(it);
		}
		auto iter = std::find_if(registeredInterfaces_.begin(), registeredInterfaces_.end(), [&](const InterfacePtr& i)
		{
			return i.get() == pImpl;
		});
		if(iter != registeredInterfaces_.end())
		{
			registeredInterfaces_.erase(iter);
		}
		return true;
	}
	if (parentContext_ == nullptr)
	{
		return false;
	}
	bool deregistered = parentContext_->deregisterInterface(pImpl);
	if (deregistered)
	{
		wg_write_lock_guard writeGuard(lock_);
		auto iter = std::find_if(registeredInterfaces_.begin(), registeredInterfaces_.end(), [&](const InterfacePtr& i)
		{
			return i.get() == pImpl;
		});
		if(iter != registeredInterfaces_.end())
		{
			registeredInterfaces_.erase(iter);
		}
	}
	return deregistered;
}

//==============================================================================
void* DefaultComponentContext::queryInterface(const TypeId& name)
{
	wg_read_lock_guard readGuard(lock_);
	for (auto& interfaceIt : interfaces_)
	{
		void* found = interfaceIt.second->queryInterface(name);
		if (found)
		{
			return found;
		}
	}
	if (parentContext_ == nullptr)
	{
		return nullptr;
	}
	return parentContext_->queryInterface(name);
}

//==============================================================================
void DefaultComponentContext::queryInterface(const TypeId& name, std::vector<void*>& o_Impls)
{
	wg_read_lock_guard readGuard(lock_);
	for (auto& interfaceIt : interfaces_)
	{
		void* found = interfaceIt.second->queryInterface(name);
		if (found)
		{
			o_Impls.push_back(found);
		}
	}
	if (parentContext_ == nullptr)
	{
		return;
	}
	return parentContext_->queryInterface(name, o_Impls);
}

//==============================================================================
void DefaultComponentContext::triggerCallbacks( IComponentContextListener& listener )
{
	std::vector<InterfaceCaster> callbacks;
	{
		wg_read_lock_guard readGuard(lock_);
		callbacks.reserve(interfaces_.size());
		for (auto pInterface : interfaces_)
		{
			callbacks.emplace_back([rttiHelper = pInterface.second](const TypeId& typeId){
				return rttiHelper->queryInterface(typeId);
			});
		}
	}
	for(auto& callback : callbacks)
	{
		listener.onInterfaceRegistered(callback);
	}
	if (parentContext_)
	{
		parentContext_->triggerCallbacks( listener );
	}
}

//==============================================================================
IComponentContext::ConnectionHolder DefaultComponentContext::registerListener(IComponentContextListener& listener)
{
	{
		wg_write_lock_guard writeGuard(lock_);
		listeners_.push_back(&listener);
	}
	triggerCallbacks( listener );

	ConnectionHolder holder(listener, disconnectSig_);
	return std::move(holder);
}

//==============================================================================
void DefaultComponentContext::deregisterListener(IComponentContextListener& listener)
{
	wg_write_lock_guard writeGuard(lock_);
	auto&& listenerIt = std::find(listeners_.begin(), listeners_.end(), &listener);
	TF_ASSERT(listenerIt != listeners_.end());
	listeners_.erase(listenerIt);
}
} // end namespace wgt
