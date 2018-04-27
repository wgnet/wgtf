#ifndef I_PYTHON_OBJ_MANAGER_HPP
#define I_PYTHON_OBJ_MANAGER_HPP

#include "wg_pyscript/py_script_object.hpp"
#include "core_object/object_handle_storage.hpp"

namespace wgt
{
class ObjectHandle;

class IPythonObjManager
{
public:
    virtual ObjectHandleStoragePtr create(const PyScript::ScriptObject& pythonObject, const ObjectHandle& parentHandle, const std::string& childPath) = 0;
	virtual ObjectHandle findOrCreate(const PyScript::ScriptObject& pythonObject, const ObjectHandle& parentHandle, const std::string& childPath) = 0;
	virtual ObjectHandle find(const PyScript::ScriptObject& pythonObject) = 0;
};
}

#endif // I_PYTHON_OBJ_MANAGER_HPP