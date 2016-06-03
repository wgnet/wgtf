#ifndef DI_REF_HPP
#define DI_REF_HPP

#include <cassert>
#include <vector>
#include "core_generic_plugin/interfaces/i_component_context.hpp"


namespace wgt
{
template< class T >
class DIRef
	: public IComponentContextListener
{
public:
	//--------------------------------------------------------------------------
	DIRef( IComponentContext & contextManager )
		: contextManager_( contextManager )
		, pValue_( nullptr )
	{
		contextManager_.registerListener( *this );
	}


	//--------------------------------------------------------------------------
	~DIRef()
	{
		contextManager_.deregisterListener( *this );
	}


	//--------------------------------------------------------------------------
	T * get() const
	{
		if(pValue_== nullptr)
		{
			pValue_ = contextManager_.queryInterface< T >();
		}
		
		return pValue_;
	}


	//--------------------------------------------------------------------------
	void get( std::vector< T * > & interfaces ) const
	{
		contextManager_.queryInterface< T >( interfaces );
	}


	//--------------------------------------------------------------------------
	T * operator->() const
	{
		return get();
	}


	//--------------------------------------------------------------------------
	T & operator*() const
	{
		assert( get() != nullptr );
		return *get();
	}


	//--------------------------------------------------------------------------
	bool operator==( const std::nullptr_t & ) const
	{
		return get() == nullptr;
	}

	//--------------------------------------------------------------------------
	bool operator!=( const std::nullptr_t & ) const
	{
		return get() != nullptr;
	}
private:
	//--------------------------------------------------------------------------
	void onInterfaceRegistered( InterfaceCaster & caster ) override
	{
		T * pInterface =
			static_cast< T * >( caster( TypeId::getType< T >() ) );
		if(pInterface)
		{
			pValue_ = pInterface;
		}
	}


	//--------------------------------------------------------------------------
	void onInterfaceDeregistered( InterfaceCaster & caster ) override
	{
		T * pInterface =
			static_cast< T * >( caster( TypeId::getType< T >() ) );
		if(pInterface && pInterface == pValue_)
		{
			pValue_ = nullptr;
		}
	}

	IComponentContext & contextManager_;
	mutable T * pValue_;
};
} // end namespace wgt
#endif //DI_REF_HPP
