#ifndef V0_AUTOMATION_INTERFACE_V0_HPP
#define V0_AUTOMATION_INTERFACE_V0_HPP

#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
DECLARE_INTERFACE_BEGIN(IAutomation, 0, 0)
/**
     *	@return if the application is ready to quit.
     */
virtual bool timedOut() = 0;
DECLARE_INTERFACE_END()
} // end namespace wgt
#endif // V0_AUTOMATION_INTERFACE_V0_HPP
