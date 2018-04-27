#include "wgt_interface_provider.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
void * WGTInterfaceProvider::queryInterfaceImpl(const TypeId & id) const
{
	for (auto& it : interfaces_)
	{
		void* found = it->queryInterface(id);
		if (found)
		{
			return found;
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------
void WGTInterfaceProvider::registerInterfaceImpl(
	std::unique_ptr< IInterface > & pImpl)
{
	interfaces_.insert(std::move(pImpl));
}

} //end namespace wgt