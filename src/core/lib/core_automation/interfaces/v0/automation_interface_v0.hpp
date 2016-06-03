#ifndef V0_AUTOMATION_INTERFACE_V0_HPP
#define V0_AUTOMATION_INTERFACE_V0_HPP

#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
DECLARE_INTERFACE_BEGIN( AutomationInterface, 0, 0 )
	virtual bool timedOut() = 0;
DECLARE_INTERFACE_END()
} // end namespace wgt
#endif // V0_AUTOMATION_INTERFACE_V0_HPP
