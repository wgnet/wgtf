#ifndef PYTHON_OBJ_MANAGER_HPP
#define PYTHON_OBJ_MANAGER_HPP

#include "interfaces/i_python_obj_manager.hpp"
#include "core_dependency_system/depends.hpp"

#include "i_script_object_definition_registry.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
namespace ReflectedPython
{
    class DefinedInstance;
}

class PythonObjManager : public Implements<IPythonObjManager>,
                         public Depends<IScriptObjectDefinitionRegistry, IObjectManager>
{
public:
	void init();
	void fini();

    virtual ObjectHandleStoragePtr create(const PyScript::ScriptObject& pythonObject, const ObjectHandle& parentHandle, const std::string& childPath) override;
    virtual ObjectHandle findOrCreate(const PyScript::ScriptObject& pythonObject, const ObjectHandle& parentHandle, const std::string& childPath) override;
    virtual ObjectHandle find(const PyScript::ScriptObject& pythonObject) override;

private:
	ObjectHandleStoragePtr create(const PyScript::ScriptObject& pythonObject,
                        const ObjectHandle& parentHandle, 
                        const std::string& childPath,
                        std::shared_ptr<IClassDefinition> definition,
                        const RefObjectId& id);

	// TODO remove the need for this
	// currently required by the type converters - need to get to a point where findOrCreate ALWAYS finds and NEVER creates.
    std::vector<ManagedObject<ReflectedPython::DefinedInstance>> objects_;
};
}

#endif