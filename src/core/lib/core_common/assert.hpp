#ifndef TF_ASSERT_HPP_INCLUDED
#define TF_ASSERT_HPP_INCLUDED


#ifdef WG_CORE

#include "wg_debug/assert.hpp"
#define TF_ASSERT(...) WG_ASSERT(__VA_ARGS__)

#else // ifdef WG_CORE

// Use default assertion routines
#include <cassert>
#define TF_ASSERT(...) assert(__VA_ARGS__)

#endif // ifdef WG_CORE

#endif // TF_ASSERT_HPP_INCLUDED
