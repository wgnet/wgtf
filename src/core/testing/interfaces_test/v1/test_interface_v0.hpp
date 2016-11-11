#ifndef V1_TEST_INTERFACE_V0_HPP
#define V1_TEST_INTERFACE_V0_HPP

#include "../v0/test_interface_v1.hpp"

namespace wgt
{
DECLARE_INTERFACE_BEGIN(TestInterface, 1, 0)
virtual void test(int) = 0;
DECLARE_INTERFACE_END()
} // end namespace wgt
#endif // V1_TEST_INTERFACE_V0_HPP
