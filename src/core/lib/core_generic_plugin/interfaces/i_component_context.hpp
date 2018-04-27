#ifndef I_COMPONENT_CONTEXT_HPP
#define I_COMPONENT_CONTEXT_HPP

#include <vector>
#include <functional>
#include "core_dependency_system/i_interface.hpp"
#include "core_variant/type_id.hpp"
#include "core_common/connection_holder.hpp"

namespace wgt
{
class IInterface;
using InterfacePtr = std::shared_ptr<IInterface>;
using InterfacePtrs = std::vector<InterfacePtr>;
class IComponentContextCreator;

/**
* Interface used for receiving notifications from the dependency system.
*
* Classes which need to be notified when interfaces are registered or de-registered can
* inherit from this and use IComponentContext::registerListener(). Most client code
* does not need to use this mechanism directly, since the automatic dependency injection
* system exposed through the Depends class takes care of caching interface pointers and
* updating them when lifecycle events are delivered.
*/
class IComponentContextListener
{
public:
	typedef std::function<void*(const TypeId&)> InterfaceCaster;

	virtual ~IComponentContextListener()
	{
	}
	virtual void onContextCreatorRegistered(IComponentContextCreator*)
	{
	}
	virtual void onContextCreatorDeregistered(IComponentContextCreator*)
	{
	}

	virtual void onInterfaceRegistered(InterfaceCaster&)
	{
	}
	virtual void onInterfaceDeregistered(InterfaceCaster&)
	{
	}
};

/**
* Context object used by the dependency system to register and discover interfaces.
*
* A context stores the currently registered interfaces and makes them available to client
* components through the queryInterface mechanism. To register an interface implementation,
* inherit from the Implements template class (passing the interface as a parameter) and use
* the registerInterface method on the context. You must also de-register the interface when
* the plug-in is unloaded:
*
* @code
*	class MyImplementation1 : public Implements<IInterface1>
*	{
*		// ...
*	};
*
*	class MyImplementation2 : public Implements<IInterface2>
*	{
*		// ...
*	};
*
*	class MyPlugin : public PluginMain
*	{
*	public:
*		MyPlugin(IComponentContext& contextManager)
*		{
*			types_.push_back(contextManager.registerInterface(new MyImplementation1()));
*			types_.push_back(contextManager.registerInterface(new MyImplementation2()));
*		}
*		void Unload(IComponentContext& contextManager) override
*		{
*			for(auto type : types_)
*			{
*				contextManager.deregisterInterface(type.get());
*			}
*		}
*	private:
*		InterfacePtrs types_;
*	};
*	PLG_CALLBACK_FUNC(MyPlugin)
* @endcode
*
* The Depends class implements automatic dependency injection, so you don't need to use
* queryInterface explicitly:
*
* @code
*	class MyDependentClass : Depends<IInterface1, IInterface2>
*	{
*		void method()
*		{
*			// The get() method is added by Depends and takes care of querying, caching and
*			// updating interface pointers using the module context and its interface
*			// registration/de-registration events. Since modules can be unloaded at any time,
*			// you must never store and reuse interface pointers; always call get() when you
*			// need an interface, and check for NULL return values.
*			IInterface1* sys1 = get<IInterface1>();
*			IInterface2* sys2 = get<IInterface2>();
*
*			if(sys1)
*			{
*				sys1->someMethod();
*			}
*
*			if(sys2)
*			{
*				sys2->someOtherMethod();
*			}
*		}
*	};
* @endcode
*
* <b>Note:</b> when implementing a plug-in, your Initialise method will receive a context object,
* so it is tempting to use queryInterface() on it to obtain and call external interfaces. However,
* this will not properly support hotloading of the plug-ins which provide those external interfaces.
* Use ContextCallBackHelper::registerCallback() in your constructor instead and see the documentation
* for ContextCallBackHelper for a more detailed explanation and code examples.
*
* Contexts are hierarchical: there's a root application context with children for each loaded module.
* Interfaces registered in a child context are propagated to the parent and siblings through the
* registration event system, so they are globally visible. However, in a few special cases it is
* necessary to instantiate a private copy of an implementation for each loaded module (e.g. each module
* needs its own PluginMemoryAllocator). A mechanism based on IComponentContextCreator is used in these
* cases to inject a separate instance of the implementation into each module context, so when components
* query the corresponding interface, they get the instance specific to their module.
*/
class IComponentContext : public IComponentContextListener
{
public:
	virtual ~IComponentContext()
	{
	}

	enum ContextRegState
	{
		Reg_Local,
		Reg_Parent,
		Reg_Global = Reg_Parent
	};

	template <class T>
	InterfacePtr registerInterface(T* pImpl, bool transferOwnership = true, ContextRegState regState = Reg_Global)
	{
		return registerInterfaceImpl(
			TypeId::getType<T>(), std::make_shared<InterfaceHolder<T>>(pImpl, transferOwnership), regState);
	}

	virtual InterfacePtr registerInterfaceImpl(const TypeId&, InterfacePtr pImpl, ContextRegState regState) = 0;

	virtual bool deregisterInterface(IInterface* typeId) = 0;

	virtual void* queryInterface(const TypeId&) = 0;

	template <class T>
	T* queryInterface()
	{
		return reinterpret_cast<T*>(queryInterface(typeid(T).name()));
	}

	virtual void queryInterface(const TypeId&, std::vector<void*>& o_Impls) = 0;

	template <class T>
	void queryInterface(std::vector<T*>& o_Impls)
	{
		queryInterface(typeid(T).name(), reinterpret_cast<std::vector<void*>&>(o_Impls));
	}

	typedef ConnectionHolderT<IComponentContextListener> ConnectionHolder;
	virtual ConnectionHolder registerListener(IComponentContextListener& listener) = 0;
	virtual void triggerCallbacks(IComponentContextListener& listener) = 0;
};
} // end namespace wgt
#endif // I_COMPONENT_CONTEXT_HPP
