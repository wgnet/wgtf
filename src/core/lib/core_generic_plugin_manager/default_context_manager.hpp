#ifndef DEFAULT_COMPONENT_CONTEXT_HPP
#define DEFAULT_COMPONENT_CONTEXT_HPP

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_variant/type_id.hpp"

#include <set>
#include <map>

namespace wgt
{
class RTTIHelper;

class DefaultComponentContext
	: public IComponentContext
{
public:
	explicit DefaultComponentContext( IComponentContext * parentContext = NULL );
	~DefaultComponentContext();

	IInterface * registerInterfaceImpl(
		const TypeId &, IInterface * pImpl,
		ContextRegState regState ) override;

	bool deregisterInterface( IInterface * typeId ) override;

	void * queryInterface( const TypeId & ) override;

	void queryInterface(
		const TypeId &,
		std::vector< void * > & o_Impls ) override;

	void registerListener( IComponentContextListener & listener ) override;
	void deregisterListener( IComponentContextListener & listener ) override;

private:
	virtual void onInterfaceRegistered( InterfaceCaster & ) override;
	virtual void onInterfaceDeregistered( InterfaceCaster & ) override;

	typedef std::multimap< const TypeId, RTTIHelper * > InterfaceMap;
	InterfaceMap										interfaces_;
	std::set< IInterface * >							registeredInterfaces_;
	IComponentContext *									parentContext_;
	std::vector< IComponentContextListener * >			listeners_;
};
} // end namespace wgt
#endif
