#ifndef V0_HELLO_INTERFACE_V1_HPP
#define V0_HELLO_INTERFACE_V1_HPP

#include "hello_interface_v0.hpp"

namespace wgt
{
DECLARE_INTERFACE_BEGIN(HelloInterface, 0, 1)
virtual void display() = 0;
virtual void remove() = 0;
DECLARE_INTERFACE_END()
} // end namespace wgt

#endif // V0_HELLO_INTERFACE_V1_HPP