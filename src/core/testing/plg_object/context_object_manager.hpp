#ifndef CONTEXT_OBJECT_MANAGER_HPP
#define CONTEXT_OBJECT_MANAGER_HPP

#include "core_dependency_system/i_interface.hpp"
#include "interfaces/i_object_manager.hpp"

namespace wgt
{
class ContextObjectManagerTest : public Implements<IObjectManagerTest>
{
public:
	ContextObjectManagerTest(IObjectManagerTest& parent, const wchar_t* contextName);

private:
	IObjectManagerTest& parent_;
};
} // end namespace wgt
#endif // CONTEXT_OBJECT_MANAGER_HPP
