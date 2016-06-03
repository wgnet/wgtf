#ifndef V0_TEST_INTERFACE_V0_HPP
#define V0_TEST_INTERFACE_V0_HPP

#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
DECLARE_INTERFACE_BEGIN( TestInterface, 0, 0 )
	virtual void test() = 0;
DECLARE_INTERFACE_END()
} // end namespace wgt
#endif //V0_TEST_INTERFACE_V0_HPP
