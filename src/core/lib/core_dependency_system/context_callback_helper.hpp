#ifndef CONTEXT_CALLBACK_HELPER_HPP
#define CONTEXT_CALLBACK_HELPER_HPP

#include "core_dependency_system/wgt_static_init.hpp"
#include "core_common/function_traits.hpp"

namespace wgt
{

/**
* Use this class to be notified when to perform initialisation which depends on external interfaces.
*
* Unloading or reloading plug-ins can cause interfaces to be removed and re-added several times
* during the lifetime of an application. Objects which use those interfaces for initialisation
* (e.g. register commands, panels etc. with them) must therefore be able to perform cleanup and
* re-initialisation. The registerCallback method obtained by inheriting from ContextCallBackHelper
* provides a convenient way of doing this:
*
* @code
*	class MyPlugin : PluginMain
*	{
*	public:
*		MyPlugin() : myComponent_(nullptr)
*		{
*			// PluginMain derives from ContextCallBackHelper, so we can use registerCallback().
*			registerCallback(
*				[this](ISomeSystem& sys1, ISomeOtherSystem& sys2)
*				{
*					// Called when both ISomeSystem and ISomeOtherSystem become available. We
*					// can create and add our component.
*					myComponent_ = new MyComponent(sys2);
*					sys1.registerSomeComponent(myComponent_);
*				},
*				[this](ISomeSystem& sys1, ISomeOtherSystem& sys2)
*				{
*					// Called when either ISomeSystem or ISomeOtherSystem become unavailable
*					// because the providing plug-in is being unloaded. Our component, which
*					// uses both, won't work anymore, so it must be removed and destroyed.
*					if(myComponent_)
*					{
*						sys1.deregisterSomeComponent(myComponent_);
*						delete myComponent_;
*						myComponent_ = nullptr;
*					}
*				}
*			);
*		}
*
*		bool Finalise(IComponentContext& contextManager) override
*		{
*			// Our plug-in is being unloaded, so if we managed to add the component before,
*			// we must remove and destroy it now. The fini callback registered above won't
*			// take care of cleanup in this case, since that runs when other plug-ins are
*			// being removed, not when our plug-in is going away.
*			if(myComponent_)
*			{
*				ISomeSystem* sys1 = contextManager.queryInterface<ISomeSystem>();
*				if(sys1)
*					sys1->deregisterSomeComponent(myComponent_);
*				delete myComponent_;
*				myComponent_ = nullptr;
*			}
*			return true;
*		}
*	private:
*		MyComponent* myComponent_;
*	};
* @endcode
*
* The init function passed to registerCallback will be called when <b>all</b> the interfaces specified
* as arguments to the function become available. The fini function, if one is provided, will be called when
* <b>any</b> of the interfaces is being de-registered (before it's removed, so you can still use it).
* Implementing init/cleanup this way transparently supports plug-in hotloading, as opposed to obtaining
* interface pointers and performing initialisation in the Initialise method of your plug-in.
*
* The mechanism is implemented through WGTStaticExecutor. This object checks if the interfaces are available
* when it's constructed, and runs the init callback immediately if they are. Otherwise, it is marked as
* not satisfied, and a listener on the register event will check it every time a new interface is added.
* Conversely, a listener for the de-register event will trigger the fini function when any of the listed
* interfaces is about to be removed, and mark the object as not satisfied, so that subsequent registration
* events will trigger the init function again.
*/
struct ContextCallBackHelper
{
	template< typename T>
	void registerCallback(T initFunc)
	{
		typedef function_traits< T >::args_type Args;
		typedef repack_args< Args >::type CallbackType;
		callbacks_.emplace_back(std::make_unique< CallbackType >(initFunc));
	}

	template< typename T, typename U>
	void registerCallback(T initFunc, U finiFunc)
	{
		typedef function_traits< T >::args_type Args;
		typedef repack_args< Args >::type CallbackType;
		callbacks_.emplace_back(std::make_unique< CallbackType >(initFunc, finiFunc));
	}

	std::vector < std::unique_ptr< WGTStaticExecutor > > callbacks_;
};

} //end namespace wgt

#endif //CONTEXT_CALLBACK_HELPER_HPP