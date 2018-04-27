#ifndef I_COMPONENT_CONTEXT_CREATOR_HPP
#define I_COMPONENT_CONTEXT_CREATOR_HPP

#include <memory>

namespace wgt
{
class IInterface;
using InterfacePtr = std::shared_ptr<IInterface>;

/**
* Mechanism for injecting local interface implementations into module contexts.
*
* Under normal circumstances, interface implementations should be visible globally, and
* IComponentContext::registerInterface() takes care of this. However, some implementations
* must be instantiated separately for each module. This can be achieved by implementing
* this interface and registering the implementation early during application startup.
*
* Whenever a module context is created, the system will call createContext() on all registered
* IComponentContextCreator implementations and inject the returned interface pointer locally
* into the context. The getType() method must return the name of the interface type as
* obtained from the typeid operator.
*
* @code
*	class MyLocalImplementation : public Implements<ISomeInterface>
*	{
*		// ...
*	};
*	class MyContextCreator : public Implements<IComponentContextCreator>
*	{
*	public:
*		InterfacePtr createContext(const wchar_t* contextId)
*		{
*			return std::make_shared<InterfaceHolder<MyLocalImplementation>>(new MyLocalImplementation(contextId), false);
*		}
*		const char* getType() const
*		{
*			return typeid(ISomeInterface).name();
*		}
*	};
*	
*	// During application initialisation:
*	globalContext->registerInterface(new MyContextCreator);
* @endcode
*
* When a new IComponentContextCreator is registered, it will be applied to all existing module contexts, so
* it is not a hard requirement to register creators during application init. The purpose of the interface,
* rather than the design of the dependency system, dictate when it's appropriate to perform this registration.
*
* At the time of writing, this mechanism is used for the following interfaces:
*	- PluginStaticInitializer (IStaticInitalizer):
*		Manages and runs static initializers registered by WGTStaticExecutor & co, so there needs to be one
*		instance per plug-in.
*
*	- PluginMemoryAllocator (IMemoryAllocator):
*		Each module needs its own memory allocator.
*
*	- QtFramework (IQtFramework):
*		The framework implementation is actually shared between all modules, but the creator mechanism is used
*		as a callback for context creation in this case, to allow modules to inject their own Qt resources into
*		the shared framework.
*
*	- ContextObjectManager (IObjectManager) and ContextDefinitionManager (IDefinitionManager):
*		The reflection system must track objects and definitions individually for each module.
*/
class IComponentContextCreator
{
public:
	virtual ~IComponentContextCreator()
	{
	}
	virtual const char* getType() const = 0;
	virtual InterfacePtr createContext(const wchar_t* contextId) = 0;
};
} // end namespace wgt
#endif /// I_COMPONENT_CONTEXT_CREATOR_HPP
