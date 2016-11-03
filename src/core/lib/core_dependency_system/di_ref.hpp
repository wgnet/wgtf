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
	DIRef( IComponentContext & context )
		: context_( &context )
		, pValue_( nullptr )
	{
		context_->registerListener( *this );
		pValue_ = context.queryInterface< T >();
	}


	//--------------------------------------------------------------------------
	DIRef( const DIRef& that )
		: context_( that.context_ )
		, pValue_( nullptr )
	{
		context_->registerListener( *this );
		pValue_ = that.pValue_;
	}


	//--------------------------------------------------------------------------
	~DIRef()
	{
		context_->deregisterListener( *this );
	}


	//--------------------------------------------------------------------------
	DIRef& operator=( const DIRef& that )
	{
		if(context_ != that.context_)
		{
			context_->deregisterListener( *this );
			context_ = that.context_;
			context_->registerListener( *this );
		}

		pValue_ = that.pValue_;

		return *this;
	}


	//--------------------------------------------------------------------------
	T * get() const
	{
		return pValue_;
	}


	//--------------------------------------------------------------------------
	void get( std::vector< T * > & interfaces ) const
	{
		context_->queryInterface< T >( interfaces );
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

	IComponentContext * context_;
	T * pValue_;
};
} // end namespace wgt
#endif //DI_REF_HPP
