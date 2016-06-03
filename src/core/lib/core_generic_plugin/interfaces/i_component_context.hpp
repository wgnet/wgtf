#ifndef I_COMPONENT_CONTEXT_HPP
#define I_COMPONENT_CONTEXT_HPP

#include <vector>
#include <functional>
#include "core_dependency_system/i_interface.hpp"
#include "core_variant/type_id.hpp"

namespace wgt
{
class IInterface;
class IComponentContextCreator;

class IComponentContextListener
{
public:
	typedef std::function< void * ( const TypeId & ) > InterfaceCaster;

	virtual ~IComponentContextListener() {}
	virtual void onContextCreatorRegistered( IComponentContextCreator * ) {}
	virtual void onContextCreatorDeregistered( IComponentContextCreator * ) {}

	virtual void onInterfaceRegistered( InterfaceCaster & ) {}
	virtual void onInterfaceDeregistered( InterfaceCaster & ) {}
};

class IComponentContext
	: public IComponentContextListener
{
public:
	virtual ~IComponentContext() {}

	enum ContextRegState
	{
		Reg_Local,
		Reg_Parent,
		Reg_Global = Reg_Parent
	};

	template< class T >
	IInterface * registerInterface( T * pImpl,
		bool transferOwnership = true, ContextRegState regState = Reg_Global )
	{
		return registerInterfaceImpl(
			TypeId::getType< T >(), new InterfaceHolder< T >( pImpl, transferOwnership ), regState );
	}

	virtual IInterface * registerInterfaceImpl(
		const TypeId &, IInterface * pImpl,
		ContextRegState regState ) = 0;


	virtual bool deregisterInterface( IInterface * typeId ) = 0;

	virtual void * queryInterface( const TypeId & ) = 0;

	template< class T >
	T * queryInterface()
	{
		return reinterpret_cast< T * >(
			queryInterface( typeid( T ).name() ) );
	}

	virtual void queryInterface(
		const TypeId &, std::vector< void * > & o_Impls ) = 0;

	template< class T >
	void queryInterface( std::vector< T * > & o_Impls )
	{
		queryInterface(
			typeid( T ).name(),
			reinterpret_cast< std::vector< void * > & >( o_Impls ) );
	}

	virtual void registerListener( IComponentContextListener & listener ) = 0;
	virtual void deregisterListener( IComponentContextListener & listener ) = 0;
};
} // end namespace wgt
#endif //I_COMPONENT_CONTEXT_HPP
