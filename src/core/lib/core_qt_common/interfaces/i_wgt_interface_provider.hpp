#ifndef I_WGT_INTERFACE_PROVIDER_HPP
#define I_WGT_INTERFACE_PROVIDER_HPP

#include "core_variant/type_id.hpp"
#include <QObject>

namespace wgt
{

class IWGTInterfaceProvider
{
public:
	virtual ~IWGTInterfaceProvider() {}

	static const char * getPropName()
	{
		static const char * s_PropName = "wgtf_attached_property";
		return s_PropName;
	}

	template< typename T >
	T * queryInterface() const
	{
		static TypeId typeId = TypeId::getType< T >();
		return static_cast< T * >( queryInterfaceImpl(typeId) );
	}

	static IWGTInterfaceProvider * getInterfaceProvider(const QObject & qObject);


private:
	virtual void* queryInterfaceImpl(const TypeId & id) const = 0;
};

//------------------------------------------------------------------------------
template< bool QueryHelperExists >
struct IWGTInterfaceProviderQueryHelper
{
	template< typename T >
	static bool attachProperty(QObject &, T &)
	{
		//Do nothing
		return false;
	}

	static bool attachProperty(...)
	{
		//Do nothing
		return false;
	}
};


//------------------------------------------------------------------------------
template<>
struct IWGTInterfaceProviderQueryHelper< true >
{
	template< typename T >
	static bool attachProperty(QObject & object, T & provider)
	{ 
		IWGTInterfaceProvider * interfaceProvider = &provider;
		QVariant wrapper = QVariant::fromValue(interfaceProvider);
		object.setProperty(
			IWGTInterfaceProvider::getPropName(), wrapper );
		return true;
	}


	static bool attachProperty(...)
	{
		//Not QObject, cannot auto attach property
		return false;
	}
};

Q_DECLARE_METATYPE(IWGTInterfaceProvider*);

} //end namespace wgt

#endif // I_WGT_ITEM_MODEL_HPP