#ifndef WGT_INTERFACE_PROVIDER_HPP
#define WGT_INTERFACE_PROVIDER_HPP

#include <memory>
#include <set>
#include "core_dependency_system/i_interface.hpp"
#include "core_qt_common/interfaces/i_wgt_interface_provider.hpp"

namespace wgt
{

/**
* Add interface querying capabilities to Qt objects.
*
* Inherit from this class and call registerInterface() in your constructor to expose a
* query mechanism which can be used by other parts of the code to discover interfaces
* that your class provides. This is needed because objects are usually passed around
* as Qt types (for example, QAbstractItemModel), so we can't just add a queryInterface
* method in a base class and have that available everywhere.
*
* This class assumes the object you're registering inherits from QObject, and works by
* attaching a property to the object which points to the IWGTInterfaceProvider instance.
* The property can then be queried on any QObject, and if it's present, the query interface
* mechanism can be invoked:
*
* @code
*	QAbstractItemModel* model = sourceModel();
*	// Use IWGTInterfaceProvider::getInterfaceProvider to query our custom property.
*	IWGTInterfaceProvider* interfaceProvider = IWGTInterfaceProvider::getInterfaceProvider(*model);
*	// Use queryInterface if the property was present and we got back an interface provider.
*	IWgtItemModel* wgtItemModel = interfaceProvider ? interfaceProvider->queryInterface<IWgtItemModel>() : nullptr;
*	if(wgtItemModel)
*	{
*		// The object has the interface we need, use it.
*	}
* @endcode
*/
class WGTInterfaceProvider
	: public IWGTInterfaceProvider
{
public:
	template< typename T >
	WGTInterfaceProvider( T * pThis )
	{
		IWGTInterfaceProviderQueryHelper<
			std::is_base_of< IWGTInterfaceProvider, T >::value >::attachProperty(*pThis, *pThis);
	}

	template< typename T >
	void registerInterface(T & interfaceRef)
	{
		std::unique_ptr< IInterface > interfaceHolder( 
			new InterfaceHolder<T>(&interfaceRef, false ) );
		this->registerInterfaceImpl(interfaceHolder);
	}


private:
	void registerInterfaceImpl(std::unique_ptr< IInterface > & pImpl);
	void * queryInterfaceImpl(const TypeId & id) const override;

	std::set< std::unique_ptr< IInterface > > interfaces_;
};

} //end namespace wgt

#endif // WGT_INTERFACE_PROVIDER_HPP