#pragma once
#ifndef _PYTHON27_UNIT_TEST_HPP
#define _PYTHON27_UNIT_TEST_HPP

namespace wgt
{
class IComponentContext;

// Must be the same context with which Python27ScriptingEngine has been
// registered
extern IComponentContext* g_contextManager;
} // end namespace wgt
#endif // _PYTHON27_UNIT_TEST_HPP
