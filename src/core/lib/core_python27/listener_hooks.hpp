#pragma once

#include "core_reflection/property_accessor_listener.hpp"
#include "wg_pyscript/py_script_object.hpp"

// Use this macro to enable/disable firing UI notifications from scripts
#define ENABLE_PYTHON_LISTENER_HOOKS 1

// Use this macro to change if listeners are notified with a
// full path + root object
// or
// child path + child object
#define ENABLE_FULL_PATH_PYTHON_LISTENER_HOOKS 0

namespace wgt
{
namespace ReflectedPython
{
/**
 *	Attach hooks for listening to setattr and delattr.
 *	So that the GUI can be notified when Python objects change.
 *	@param pythonObject attach hooks to this type of object.
 *		Note: that it will attach hooks to the *type* and not the *instance*.
 */
void attachListenerHooks(PyScript::ScriptObject& pythonObject);

/**
 *	@see attachListenerHooks()
 */
void detachListenerHooks(PyScript::ScriptObject& pythonObject);

/**
 *	Class for checking if a notification has already been fired before setattr
 *	is called.
 *	E.g. if a property is set by the command system then a notification has
 *	already been fired and the setattr hook should not send another one.
 */
class HookListener : public PropertyAccessorListener
{
public:
	HookListener();
	void preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	void postSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	bool entered() const;

private:
	size_t entered_;
};

} // namespace ReflectedPython

// Needed to pass state to Python functions below
class IComponentContext;
extern IComponentContext* g_pHookContext;
extern std::weak_ptr<ReflectedPython::HookListener> g_listener;
} // end namespace wgt
