#ifndef V0_AUTOMATION_INTERFACE_V1_HPP
#define V0_AUTOMATION_INTERFACE_V1_HPP

#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
DECLARE_INTERFACE_BEGIN(AutomationInterface, 0, 1)
/**
     *	@return if the application is ready to quit.
     */
virtual bool timedOut() = 0;
/**
     *	Tell automation that loading has finished.
     */
virtual void notifyLoadingDone() = 0;
DECLARE_INTERFACE_END()
} // end namespace wgt
#endif // V0_AUTOMATION_INTERFACE_V1_HPP
