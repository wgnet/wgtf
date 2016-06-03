#ifndef DEPENDS_HPP
#define DEPENDS_HPP

#include <cassert>
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_variant/type_id.hpp"

namespace wgt
{
class DummyDependsType {};

//==============================================================================
// Change this to support as many classes as we need to inherit, until we have
// support for variadic templates
//==============================================================================
template<
	typename T1,
	typename T2 = DummyDependsType,
	typename T3 = DummyDependsType,
	typename T4 = DummyDependsType,
	typename T5 = DummyDependsType,
	typename T6 = DummyDependsType>
class DependsImpl
	: public DependsImpl< T2, T3, T4, T5, T6 > 
{
protected:
	typedef DependsImpl< T2, T3, T4, T5, T6 > Base;

	DependsImpl()
		: pValue_( nullptr )
	{
	}


	//--------------------------------------------------------------------------
	template< typename T >
	T * baseGet( IComponentContext & context ) const
	{
		return Base::template get< T >( context );
	}


	template<bool isSameType, bool _dummy = false>
	struct GetHelper
	{
		//--------------------------------------------------------------------------
		template< typename Source, typename Target, typename DependsType >
		static Target * get( IComponentContext & context, Source * & pValue, DependsType & pThis )
		{
			if(pValue == nullptr)
			{
				pValue = context.queryInterface< Source >();
			}
			return pValue;
		}
	};


	template<bool _dummy>
	struct GetHelper<false, _dummy>
	{
		//--------------------------------------------------------------------------
		template< typename Source, typename Target, typename DependsType >
		static Target * get( IComponentContext & context, Source * & pValue, DependsType & pThis )
		{
			return pThis.template baseGet< Target >( context );
		}
	};


	//--------------------------------------------------------------------------
	template< typename T >
	T * get( IComponentContext & context ) const
	{
		return GetHelper< std::is_same< T1, T >::value >::template get< T1, T >( context, pValue_, *this );
	}


	//--------------------------------------------------------------------------
	template< typename T >
	void get( IComponentContext & context, std::vector< T * > & interfaces ) const
	{
		if(std::is_same< T1, T >::value)
		{
			context.queryInterface< T >( interfaces );
			return;
		}
		return Base::template get< T >( context, interfaces );
	}


	//--------------------------------------------------------------------------
	void onInterfaceRegistered( IComponentContextListener::InterfaceCaster & caster )
	{
		static auto s_TypeId = TypeId::getType< T1 >();
		T1 * pInterface =
			static_cast< T1 * >( caster( s_TypeId ) );
		if(pInterface)
		{
			pValue_ = pInterface;
			return;
		}
		Base::onInterfaceRegistered( caster );
	}


	//--------------------------------------------------------------------------
	void onInterfaceDeregistered( IComponentContextListener::InterfaceCaster & caster )
	{
		static auto s_TypeId = TypeId::getType< T1 >();
		T1 * pInterface =
			static_cast< T1 * >( caster( s_TypeId ) );
		if(pInterface && pInterface == pValue_)
		{
			pValue_ = nullptr;
			return;
		}
		Base::onInterfaceDeregistered( caster );
	}
private:
	mutable T1 * pValue_;
};

//==============================================================================
// Handle the empty type
//==============================================================================
template<>
class DependsImpl< DummyDependsType >
{
protected:
	//--------------------------------------------------------------------------
	template< typename T >
	T * get( IComponentContext & context ) const
	{
		return nullptr;
	}


	//--------------------------------------------------------------------------
	template< typename T >
	void get( IComponentContext & context, std::vector< T * > & ) const
	{
		//Do nothing
	}

	//--------------------------------------------------------------------------
	void onInterfaceRegistered( IComponentContextListener::InterfaceCaster & )
	{
		//Do nothing
	}

	//--------------------------------------------------------------------------
	void onInterfaceDeregistered( IComponentContextListener::InterfaceCaster & )
	{
		//Do nothing
	}

};


//==============================================================================
// Change this to support as many classes as we need to inherit, until we have
// support for variadic templates
//==============================================================================
template<
	typename T1,
	typename T2 = DummyDependsType,
	typename T3 = DummyDependsType,
	typename T4 = DummyDependsType,
	typename T5 = DummyDependsType,
	typename T6 = DummyDependsType>
class Depends
	: public IComponentContextListener
	, private DependsImpl< T1, T2, T3, T4, T5, T6 >
{
	typedef DependsImpl< T1, T2, T3, T4, T5, T6 > Base;
public:
	Depends( IComponentContext & context )
		: context_( context )
	{
		context_.registerListener( *this );
	}


	//--------------------------------------------------------------------------
	~Depends()
	{
		context_.deregisterListener( *this );
	}


	//--------------------------------------------------------------------------
	template< typename T >
	T * get() const
	{
		return Base::template get< T >( context_ );
	}


	//--------------------------------------------------------------------------
	template< typename T >
	void get( std::vector< T * > & interfaces ) const
	{
		Base::template get< T >( context_, interfaces );
	}


private:
	//--------------------------------------------------------------------------
	void onInterfaceRegistered( InterfaceCaster & caster ) override
	{
		Base::onInterfaceRegistered( caster );
	}


	//--------------------------------------------------------------------------
	void onInterfaceDeregistered( InterfaceCaster & caster ) override
	{
		Base::onInterfaceDeregistered( caster );
	}

	IComponentContext & context_;
};
} // end namespace wgt
#endif //DEPENDS_HPP
