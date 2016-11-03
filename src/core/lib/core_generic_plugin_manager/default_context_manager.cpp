#include "default_context_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_dependency_system/i_interface.hpp"
#include <assert.h>
#include <unordered_map>

namespace wgt
{
namespace
{
	static const TypeId s_ComponentContextCreatorTypeId =
		TypeId::getType< IComponentContextCreator >();
}

class RTTIHelper
{
public:
	//==========================================================================
	RTTIHelper( IInterface * pImpl )
		: pImpl_( pImpl )
	{
	}

	//==========================================================================
	~RTTIHelper()
	{
		delete pImpl_;
	}

	//==========================================================================
	void * queryInterface(
		const TypeId & name )
	{
		auto && findIt = typeCache_.find( name );
		if (findIt != typeCache_.end())
		{
			return findIt->second;
		}
		void * found = pImpl_->queryInterface( name );
		typeCache_.insert( std::make_pair( name, found ) );
		return found;
	}

	IInterface * getImpl() const { return pImpl_; }
	bool owns() const { return owns_; }

private:
	IInterface *								pImpl_;
	bool										owns_;
	std::unordered_map< const TypeId, void * >	typeCache_;
};


//==============================================================================
DefaultComponentContext::DefaultComponentContext( IComponentContext * parentContext )
	: parentContext_( parentContext )
{
}


//==============================================================================
DefaultComponentContext::~DefaultComponentContext()
{
	for( auto & interface : interfaces_ )
	{
		registeredInterfaces_.erase( interface.second->getImpl() );
		delete interface.second;
	}
	interfaces_.clear();

	if(parentContext_ == nullptr)
	{
		return;
	}
	for( auto & parentInterface : registeredInterfaces_ )
	{
		parentContext_->deregisterInterface( parentInterface );
	}
}

//==============================================================================
IInterface * DefaultComponentContext::registerInterfaceImpl(
	const TypeId & id, IInterface * pImpl, ContextRegState regState )
{
	//Use pointer as unique id
	registeredInterfaces_.insert( pImpl );
	if (regState == Reg_Parent &&
		parentContext_ != nullptr)
	{
		return parentContext_->registerInterfaceImpl(
			id, pImpl, Reg_Local );
	}
	auto insertIt = interfaces_.insert(
		std::make_pair(
			id, new RTTIHelper( pImpl ) ) );

	auto contextCreator =
		static_cast< IComponentContextCreator * >(
			pImpl->queryInterface( s_ComponentContextCreatorTypeId ) );
	if(contextCreator)
	{
		for( auto & listener : listeners_ )
		{
			listener->onContextCreatorRegistered( contextCreator );
		}
	}
	IComponentContextListener::InterfaceCaster function =
		std::bind( &RTTIHelper::queryInterface, insertIt->second, std::placeholders::_1 );
	onInterfaceRegistered( function );
	return pImpl;
}


//------------------------------------------------------------------------------
void DefaultComponentContext::onInterfaceRegistered( InterfaceCaster & caster )
{
	for( auto & listener : listeners_ )
	{
		listener->onInterfaceRegistered( caster );
	}
}


//------------------------------------------------------------------------------
void DefaultComponentContext::onInterfaceDeregistered( InterfaceCaster & caster )
{
	for( auto & listener : listeners_ )
	{
		listener->onInterfaceDeregistered( caster );
	}
}


//==============================================================================
bool DefaultComponentContext::deregisterInterface(
	IInterface * pImpl )
{
	for( InterfaceMap::const_iterator it = interfaces_.begin();
		 it != interfaces_.end();
		 it++ )
	{
		if (pImpl != it->second->getImpl())
		{
			continue;
		}
		IComponentContextCreator * contextCreator =
			static_cast< IComponentContextCreator * >(
				pImpl->queryInterface( s_ComponentContextCreatorTypeId ) );
		if (contextCreator)
		{
			for( auto & listener : listeners_ )
			{
				listener->onContextCreatorDeregistered( contextCreator );
			} 
		}
		IComponentContextListener::InterfaceCaster function =
			std::bind( &RTTIHelper::queryInterface, it->second, std::placeholders::_1 ); 
		for( auto & listener : listeners_ )
		{
			listener->onInterfaceDeregistered( function );
		} 
		// For safety capture and delete after removing from the lists to prevent querying the interface during deletion
		auto helper = it->second;
		interfaces_.erase( it );
		registeredInterfaces_.erase( pImpl );
		delete helper;
		return true;
	}
	if (parentContext_ == nullptr)
	{
		return false;
	}
	bool deregistered =
		parentContext_->deregisterInterface( pImpl );
	if (deregistered)
	{
		registeredInterfaces_.erase( pImpl );
	}
	return deregistered;
}


//==============================================================================
void * DefaultComponentContext::queryInterface( const TypeId & name )
{
	for( auto & interfaceIt : interfaces_ )
	{
		void * found = interfaceIt.second->queryInterface(
			name );
		if (found)
		{
			return found;
		}
	}
	if (parentContext_ == nullptr)
	{
		return nullptr;
	}
	return parentContext_->queryInterface( name );
}


//==============================================================================
void DefaultComponentContext::queryInterface(
	const TypeId & name, std::vector< void * > & o_Impls )
{
	for( auto & interfaceIt : interfaces_ )
	{
		void * found =
			interfaceIt.second->queryInterface( name );
		if (found)
		{
			o_Impls.push_back( found );
		}
	}
	if (parentContext_ == nullptr)
	{
		return;
	}
	return parentContext_->queryInterface( name, o_Impls );
}


//==============================================================================
void DefaultComponentContext::registerListener( IComponentContextListener & listener )
{
	listeners_.push_back( &listener );
}


//==============================================================================
void DefaultComponentContext::deregisterListener(
	IComponentContextListener & listener )
{
	auto && listenerIt = std::find(
		listeners_.begin(), listeners_.end(), &listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}
} // end namespace wgt
