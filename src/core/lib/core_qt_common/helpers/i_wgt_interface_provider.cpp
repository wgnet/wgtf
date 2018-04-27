#include "../interfaces/i_wgt_interface_provider.hpp"
#include <QObject>
#include <QVariant>

namespace wgt
{

//------------------------------------------------------------------------------
IWGTInterfaceProvider * IWGTInterfaceProvider::getInterfaceProvider(
	const QObject & qObject)
{
	auto prop
		= qObject.property(IWGTInterfaceProvider::getPropName());
	if (prop.isValid() == false)
	{
		return nullptr;
	}
	return prop.value< IWGTInterfaceProvider *>();
}

} //end namespace wgt