#ifndef I_INTERFACE_HPP
#define I_INTERFACE_HPP

#include <vector>
#include "interface_helpers.hpp"
#include "core_variant/type_id.hpp"
#include <type_traits>

namespace wgt
{
class IInterface;

//==============================================================================
class IInterface
{
public:
	virtual ~IInterface()
	{
	}
	virtual void* queryInterface(const TypeId& id) = 0;
};

struct EmptyType { };

/**
* Helper class for supporting TypeId-based queries in an interface implementation.
*
* This implements the queryInterface method for the first type in the template argument list,
* and recursively instantiates itself for the remaining types.
*/
template <typename T1, typename... Args>
class ImplementsImpl
	: public T1
	, public virtual ImplementsImpl< Args... >
{
public:
	inline void* queryInterface(const TypeId& id)
	{
		static const TypeId selfType = TypeId::getType<T1>();
		auto pT1 = static_cast<T1*>(this);
		if (selfType == id)
		{
			return pT1;
		}
		void* t1Result = queryInterface(pT1, &id);
		if (t1Result)
		{
			return t1Result;
		}
		return ImplementsImpl<Args...>::queryInterface(id);
	}
private:
	template <typename U>
	decltype(std::declval<U>().queryInterface(std::declval<const TypeId&>())) queryInterface(U* /*pThis*/,
		const TypeId* id)
	{
		return U::queryInterface(*id);
	}

	inline void* queryInterface(...)
	{
		return nullptr;
	}
};


//==============================================================================
// Handle the empty type
//==============================================================================
template <>
class ImplementsImpl< EmptyType >
{
public:
	template< typename U >
	inline void* queryInterface( const U & dummy )
	{
		return nullptr;
	}
};


/**
* Base class for interface implementations.
*
* Classes which implement one or more interfaces should derive from this helper class to
* get the queryInterface method which is required by the dependency system for registration:
*
* @code
*	class Example : public Implements<IExample>
*	{
*		// ...
*	};
* @endcode
*
* The interface type will be a base type of the Implements instantiation, so there's no need to
* derive from the interface explicitly.
*
* Implementations need to be registered explicitly with the module context. They also need to be
* de-registered when the provider is unloaded, so the usual procedure is to instance and register
* the implementation objects in the constructor of a plug-in, store the handles returned by
* IComponentContext::registerInterface, and de-register everything in PluginMain::Unload:
*
* @code
*	class MyPlugin : public PluginMain
*	{
*	public:
*		MyPlugin(IComponentContext& contextManager)
*		{
*			types_.push_back(contextManager.registerInterface(new MyImplementation1()));
*			types_.push_back(contextManager.registerInterface(new MyImplementation2()));
*		}
*
*		void Unload(IComponentContext& contextManager) override
*		{
*			for (auto type : types_)
*			{
*				contextManager.deregisterInterface(type.get());
*			}
*		}
*	};
*	PLG_CALLBACK_FUNC(MyPlugin)
* @endcode
*
* Calling registerInterface will automatically inject the implementation in all the code which depends
* on the interface. See the documentation for the Depends class for details on how to take advantage
* of this mechanism, and how it is implemented.
*
*/
template <typename... Args>
class Implements
	: public ImplementsImpl< Args..., EmptyType >
{
};


template< typename T >
struct query_interface
{
	template< typename... Args >
	static T * execute( T *, const TypeId& id, ImplementsImpl< Args... > * pImpl )
	{
		return static_cast< T * >( pImpl->queryInterface( id ) );
	}

	static typename T * execute(T * pImpl, const TypeId& id, ... )
	{
		static const TypeId selfType = TypeId::getType<T>();
		if (selfType == id)
		{
			return pImpl;
		}
		return nullptr;
	}
};

/**
* Wrapper used by dependency contexts to store registered interface implementations.
*
* Bridges the runtime dispatch queryInterface mechanism used by the contexts to the
* metaprogramming-based dispatch used by ImplementsImpl. 
*/
template <class T>
class InterfaceHolder : public IInterface
{
public:
	InterfaceHolder(T* pImpl, bool owns) : pImpl_(pImpl), owns_(owns)
	{
	}

	virtual ~InterfaceHolder()
	{
		if (owns_)
		{
			delete pImpl_;
		}
	}

	inline void* queryInterface(const TypeId& id) override
	{
		return query_interface< T >::execute(pImpl_, id, pImpl_ );
	}

private:
	T* pImpl_;
	bool owns_;
};

} // end namespace wgt
#endif // I_INTERFACE_HPP
