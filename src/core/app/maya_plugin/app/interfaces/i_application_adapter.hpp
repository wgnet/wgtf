#ifndef I_APPLICATION_ADAPTER_HPP
#define I_APPLICATION_ADAPTER_HPP

#include "core_dependency_system/i_interface.hpp"
#include "i_application_listener.hpp"

namespace wgt
{
class IApplicationAdapter
{
public:
	virtual ~IApplicationAdapter() {}

	virtual void addListener( IApplicationListener * listener ) = 0;
	virtual void removeListener( IApplicationListener * listener ) = 0;
};
} // end namespace wgt
#endif//I_APPLICATION_ADAPTER_HPP
