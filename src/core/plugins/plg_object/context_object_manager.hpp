#ifndef CONTEXT_OBJECT_MANAGER_HPP
#define CONTEXT_OBJECT_MANAGER_HPP

#include "core_dependency_system/i_interface.hpp"
#include "interfaces/i_object_manager.hpp"

namespace wgt
{
class ContextObjectManager : public Implements<IObjectManagerNew>
{
public:
	ContextObjectManager(IObjectManagerNew& parent, const wchar_t* contextName);

private:
	IObjectManagerNew& parent_;
};
} // end namespace wgt
#endif // CONTEXT_OBJECT_MANAGER_HPP
