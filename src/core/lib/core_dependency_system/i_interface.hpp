#ifndef I_INTERFACE_HPP
#define I_INTERFACE_HPP

#include <vector>
#include "interface_helpers.hpp"
#include "core_variant/type_id.hpp"
#include <type_traits>

namespace wgt
{
class IInterface;

namespace Context
{
	extern void * queryInterface( const TypeId & );
	extern void queryInterface( const TypeId &, std::vector< void * > & );
}

//==============================================================================
class IInterface
{
public:
	virtual ~IInterface() {}
	virtual void * queryInterface( const TypeId & id ) = 0;
};


//==============================================================================
//Used to as null class type
struct EmptyType { };

//==============================================================================
// Change this to support as many classes as we need to inherit, until we have
// support for variadic templates
//==============================================================================
template<	typename T1,
			typename T2 = EmptyType,
			typename T3 = EmptyType,
			typename T4 = EmptyType,
			typename T5 = EmptyType>
class Implements
	: public T1
	, public virtual Implements< T2, T3, T4, T5 >
{
public:
	void * queryInterface( const TypeId & id )
	{
		static const TypeId selfType = TypeId::getType< T1 >();
		auto pT1 = static_cast< T1 * >( this );
		if (selfType == id)
		{
			return pT1;
		}
		void * t1Result = queryInterface( pT1, &id );
		if (t1Result)
		{
			return t1Result;
		}
		return Implements< T2, T3, T4, T5 >::queryInterface( id );
	}

private:
	template< typename U >
	decltype( std::declval< U >().queryInterface( std::declval< const TypeId& >() ) )
		queryInterface( U * /*pThis*/, const TypeId * id )
	{
		return U::queryInterface( *id );
	}

	void * queryInterface( ... )
	{
		return nullptr;
	}
};

//==============================================================================
// Handle the empty type
//==============================================================================
template<>
class Implements< EmptyType >
{
public:
	void * queryInterface( const TypeId & /*id*/ )
	{
		return nullptr;
	}
};

struct query_interface
{
	template<class T>
	static typename std::enable_if<std::is_base_of<Implements<EmptyType>, T>::value>::type*
		execute( T* pImpl_, const TypeId& id )
	{
		return pImpl_->queryInterface(id);
	}

	template<class T>
	static typename std::enable_if<!std::is_base_of<Implements<EmptyType>, T>::value>::type*
		execute(T* pImpl_, const TypeId& id)
	{
		static const TypeId selfType = TypeId::getType< T >();
		if ( selfType == id )
		{
			return pImpl_;
		}
		return nullptr;
	}
};

template< class T >
class InterfaceHolder
	: public IInterface
{
public:
	InterfaceHolder( T * pImpl, bool owns )
		: pImpl_( pImpl )
		, owns_( owns )
	{
	}

	virtual ~InterfaceHolder()
	{
		if(owns_)
		{
			delete pImpl_;
		}
	}

	void * queryInterface( const TypeId & id ) override
	{
		return query_interface::execute(pImpl_, id );
	}

private:
	T * pImpl_;
	bool owns_;
};

namespace Context
{

//==============================================================================
template< class T >
T * queryInterface()
{
	return reinterpret_cast< T * >( Context::queryInterface(
		typeid( T ).name() ) );
}

template< class T >
void queryInterface( std::vector< T * > & o_Impls )
{
	Context::queryInterface(
		typeid( T ).name(),
		reinterpret_cast< std::vector< void * > & >( o_Impls ) );
}

} //namespace Context
} // end namespace wgt
#endif //I_INTERFACE_HPP
