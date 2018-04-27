#ifndef DEPENDS_HPP
#define DEPENDS_HPP

#include <cassert>
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_variant/type_id.hpp"
#include "core_common/holder_collection.hpp"
#include <memory>
#include <mutex>

#define INTERFACE_REQUEST(type, var, holder, retOnFalse) \
	type* var##Pointer = holder->get<type>();            \
	if (var##Pointer == nullptr)                         \
		return retOnFalse;                               \
	type& var = *var##Pointer;

namespace wgt
{

/**
* Utility template used by DependsBaseT to store pointers for all the interface types given as arguments.
*
* The class stores a pointer to the first type passed as an argument and recursively instantiates itself
* for the remaining types. The pointer is initialized upon construction and updated when registration
* and de-registration events are sent by the context, so we always return the correct value from the
* get() method.
*/
template<typename... Args>
class DependsImpl;

template <typename T1, typename... Args>
class DependsImpl<T1, Args...> : public DependsImpl<Args...>
{
public:
    typedef DependsImpl<Args...> Base;
    
    template <typename T>
    struct HasType
    {
        const static bool value = std::is_same<T1, T>::value || Base::template HasType<T>::value;
    };

protected:
	DependsImpl()
	{
	}

	//--------------------------------------------------------------------------
	DependsImpl& operator=(const DependsImpl& that)
	{
		pValue_ = that.pValue_;
		Base::operator=(that);
		return *this;
	}

	//--------------------------------------------------------------------------
	template <typename T, bool = false>
	struct GetHelper
	{
		template <typename Source>
		static T* get(IComponentContext& context, Source* pValue, const DependsImpl& pThis)
		{
			return pThis.Base::template get<T>(context);
		}
	};

	//--------------------------------------------------------------------------
	template <bool _dummy>
	struct GetHelper<T1, _dummy>
	{
		template <typename Source>
		static T1* get(IComponentContext& context, Source* pValue, const DependsImpl& pThis)
		{
			return pValue;
		}
	};

	//--------------------------------------------------------------------------
	template <typename T>
	T* get(IComponentContext& context) const
	{
		return GetHelper<T>::get(context, pValue_, *this);
	}

	//--------------------------------------------------------------------------
	template <typename T>
	void get(IComponentContext& context, std::vector<T*>& interfaces) const
	{
		if (std::is_same<T1, T>::value)
		{
			context.queryInterface<T>(interfaces);
			return;
		}
		return Base::template get<T>(context, interfaces);
	}

	//--------------------------------------------------------------------------
	void onInterfaceRegistered(IComponentContextListener::InterfaceCaster& caster)
	{
		static auto s_TypeId = TypeId::getType<T1>();
		T1* pInterface = static_cast<T1*>(caster(s_TypeId));
		if (pInterface)
		{
			pValue_ = pInterface;
		}
		Base::onInterfaceRegistered(caster);
	}

	//--------------------------------------------------------------------------
	void onInterfaceDeregistered(IComponentContextListener::InterfaceCaster& caster)
	{
		static auto s_TypeId = TypeId::getType<T1>();
		T1* pInterface = static_cast<T1*>(caster(s_TypeId));
		if (pInterface && pInterface == pValue_)
		{
			pValue_ = nullptr;
		}
		Base::onInterfaceDeregistered(caster);
	}

	//--------------------------------------------------------------------------
	void initValue(IComponentContext* context)
	{
		pValue_ = context ? context->queryInterface<T1>() : nullptr;
		Base::initValue(context);
	}

private:
	T1* pValue_ = nullptr;
};

template<typename T1>
class DependsImpl<T1>
{
public:
    template <typename T>
    struct HasType
    {
        const static bool value = std::is_same<T1, T>::value;
    };

protected:
	DependsImpl()
	{
	}

	//--------------------------------------------------------------------------
	DependsImpl& operator=(const DependsImpl& that)
	{
		pValue_ = that.pValue_;
		return *this;
	}

	//--------------------------------------------------------------------------
	template <typename T>
	T* get(IComponentContext& context) const
	{
		return pValue_;
	}

	//--------------------------------------------------------------------------
	template <typename T>
	void get(IComponentContext& context, std::vector<T*>& interfaces) const
	{
		if (std::is_same<T1, T>::value)
		{
			context.queryInterface<T>(interfaces);
		}
	}

	//--------------------------------------------------------------------------
	void onInterfaceRegistered(IComponentContextListener::InterfaceCaster& caster)
	{
		static auto s_TypeId = TypeId::getType<T1>();
		T1* pInterface = static_cast<T1*>(caster(s_TypeId));
		if (pInterface)
		{
			pValue_ = pInterface;
		}
	}

	//--------------------------------------------------------------------------
	void onInterfaceDeregistered(IComponentContextListener::InterfaceCaster& caster)
	{
		static auto s_TypeId = TypeId::getType<T1>();
		T1* pInterface = static_cast<T1*>(caster(s_TypeId));
		if (pInterface && pInterface == pValue_)
		{
			pValue_ = nullptr;
		}
	}

	//--------------------------------------------------------------------------
	void initValue(IComponentContext* context)
	{
		pValue_ = context ? context->queryInterface<T1>() : nullptr;
	}

private:
	T1* pValue_ = nullptr;
};

/**
* Helper class used by DependsBaseT to register for interface lifetime events.
*
* This class provides access to the module context, which is stored in a static variable
* initialized when the module is loaded, through the WGTStaticExecutor mechanism (see
* depends.cpp for the implementation).
*
* The constructor adds the object as a listener for interface registration and de-registration
* events on the context.
*/
class DependsBase : public IComponentContextListener
{
public:
	DependsBase();
	virtual ~DependsBase();
	IComponentContext* getContext() const;
	static bool hasContext();

	operator IComponentContext&();

private:
	HolderCollection<IComponentContext::ConnectionHolder> connections_;
};

/**
* Shared interface holder used by Depends.
*
* This combines the functionality of DependsImpl, which stores interface pointers for all the
* types given as arguments to the template, with that of DependsBase, which provides access to
* the module context and registration for interface lifetime events.
*/
template <typename... Args>
class DependsBaseT : public DependsBase, public DependsImpl<Args...>
{
public:
	typedef DependsImpl<Args...> Base;

	//--------------------------------------------------------------------------
	DependsBaseT()
	{
		// call init after registration to avoid races
		Base::initValue(getContext());
	}

	//--------------------------------------------------------------------------
	virtual ~DependsBaseT()
	{
	}

	//--------------------------------------------------------------------------
	template <typename T>
	typename std::enable_if<Base::template HasType<T>::value, T*>::type get() const
	{
		auto context = getContext();
		if (context == nullptr)
		{
			return nullptr;
		}
		return Base::template get<T>(*context);
	}

	//--------------------------------------------------------------------------
	template <typename T>
	void get(std::vector<T*>& interfaces) const
	{
		auto context = getContext();
		if (context == nullptr)
		{
			return;
		}
		Base::template get<T>(*context, interfaces);
	}

	//--------------------------------------------------------------------------
	void onInterfaceRegistered(InterfaceCaster& caster) override
	{
		Base::onInterfaceRegistered(caster);
	}

	//--------------------------------------------------------------------------
	void onInterfaceDeregistered(InterfaceCaster& caster) override
	{
		Base::onInterfaceDeregistered(caster);
	}
};

/**
* This class implements automatic dependency injection.
*
* Derive from it and use the get() method it adds to obtain pointers to the interfaces you need:
*
* @code
*	class MyClass : Depends<ILoggingSystem, IUIFramework>
*	{
*		void method()
*		{
*			ILoggingSystem* logSys = get<ILoggingSystem>();
*			IUIFramework* uiFramework = get<IUIFramework>();
*
*			if(logSys)
*			{
*				// Use logSys.
*			}
*
*			if(uiFramework)
*			{
*				// Use uiFramework.
*			}
*		}
*	};
* @endcode
*
* <b>Do not store and reuse interface pointers returned by get()</b>, because the plug-in which provides
* an interface can be unloaded at any time. Always obtain the pointer right before using it and check for NULL.
*
* @section impl Implementation Details
* All the instantiations of this template for a given list of types share a pointer to a holder object (DependsBaseT)
* which stores the necessary interface pointers, so there's one such holder for each module (DLL) and each combination
* of interfaces. When it is constructed, the holder object queries and stores the currently available interfaces and
* adds itself as a listener for interface registration and de-registration events. This way, the stored interface
* pointers are always kept current.
*
* Interface implementations are registered in a context object (IComponentContext) by deriving from the
* Implements class and calling IComponentContext::registerInterface. The context object implements
* registration callbacks and provides a queryInterface() method that the holder object uses during construction.
*
* Contexts are hierarchical. Usually there's a global context for the entire application, with child contexts for
* each plug-in. Queries and registration events are propagated up the hierarchy, and the contexts themselves listen
* for registration events on their parents, which ensures that interface implementations are visible across modules.
*
* Most interface implementations are meant to be globally visible, but there are a few cases where module-local
* objects are needed, and the context hierarchy allows this to be implemented. For example, each module needs its
* private PluginMemoryAllocator. A mechanism based on IComponentContextCreator is used to inject a separate
* allocator object into each module context that is created.
*/
template <typename... Args>
class Depends
{
	typedef DependsBaseT<Args...> BaseHolder;

public:
	Depends() : instance_(nullptr)
	{
		// Only create one instance per library and share that.
		// We can't share over all libraries because the statics do not cross
		// boundaries, so we need to use a member variable instead.
		auto currentInstance = s_Instance.lock();
		if (currentInstance == nullptr)
		{
			instance_ = std::make_shared<BaseHolder>();
			s_Instance = instance_;
		}
		else
		{
			instance_ = currentInstance;
		}
	}

	//--------------------------------------------------------------------------
	template <typename T>
	typename std::enable_if<BaseHolder::template HasType<T>::value, T*>::type get() const
	{
		return instance_->template get<T>();
	}

	//--------------------------------------------------------------------------
	template <typename T>
	typename std::enable_if<BaseHolder::template HasType<T>::value, void>::type get(std::vector<T*>& interfaces) const
	{
		instance_->template get<T>(interfaces);
	}

	typedef std::weak_ptr<BaseHolder> BaseHolderStaticType;

	operator IComponentContext&()
	{
		return *instance_;
	}

protected:
	virtual ~Depends()
	{
		instance_ = nullptr;
	}

private:
	static BaseHolderStaticType s_Instance;
	std::shared_ptr<BaseHolder> instance_;
};

/**
* DependsLocal should be used when you need to obtain interface pointers and deriving from Depends
* is not appropriate.
*
* The main use cases are inside global functions and static methods. The class is identical to Depends,
* but using a different name makes the intention clearer in the calling code.
*/
template <typename... Args>
class DependsLocal : public Depends<Args...>
{
public:
};

template <typename... Args>
typename Depends<Args...>::BaseHolderStaticType Depends<Args...>::s_Instance;

} // end namespace wgt
#endif // DEPENDS_HPP
