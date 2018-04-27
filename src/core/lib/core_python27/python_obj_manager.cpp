#include "pch.hpp"
#include "python_obj_manager.hpp"
#include "defined_instance.hpp"

namespace wgt
{

void PythonObjManager::init()
{

}

void PythonObjManager::fini()
{
	objects_.clear();
}

ObjectHandleStoragePtr PythonObjManager::create(const PyScript::ScriptObject& pythonObject,
                                      const ObjectHandle& parentHandle, 
                                      const std::string& childPath,
                                      std::shared_ptr<IClassDefinition> definition,
                                      const RefObjectId& id)
{
	std::unique_ptr<ReflectedPython::DefinedInstance> instance(new ReflectedPython::DefinedInstance(pythonObject, definition, parentHandle, childPath));
	return std::make_shared<ObjectHandleStorage<std::unique_ptr<ReflectedPython::DefinedInstance>>>(std::move(instance));
}

//------------------------------------------------------------------------------
ObjectHandleStoragePtr PythonObjManager::create(const PyScript::ScriptObject& pythonObject,
                                      const ObjectHandle& parentHandle,
                                      const std::string& childPath)
{
    assert(pythonObject.exists());

    // Get a definition that's the same for each ScriptObject instance
    auto pRegistry = get<IScriptObjectDefinitionRegistry>();
    assert(pRegistry != nullptr);
    auto pDefinition = pRegistry->findOrCreateDefinition(pythonObject);
    assert(pDefinition != nullptr);
    const auto& id = pRegistry->getID(pythonObject);

	// Search for an existing object handle that's using that definition
	auto pObjectManager = get<IObjectManager>();
	assert(pObjectManager != nullptr);
	auto handle = pObjectManager->getObject(id);
	assert(!handle.isValid());

    return create(pythonObject, parentHandle, childPath, pDefinition, id);
}

//------------------------------------------------------------------------------
ObjectHandle PythonObjManager::findOrCreate(const PyScript::ScriptObject& pythonObject,
                                            const ObjectHandle& parentHandle, 
                                            const std::string& childPath)
{
	assert(pythonObject.exists());

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = get<IScriptObjectDefinitionRegistry>();
	assert(pRegistry != nullptr);
	auto pDefinition = pRegistry->findOrCreateDefinition(pythonObject);
	assert(pDefinition != nullptr);
	const auto& id = pRegistry->getID(pythonObject);

	// Search for an existing object handle that's using that definition
	auto pObjectManager = get<IObjectManager>();
	assert(pObjectManager != nullptr);
	auto handle = pObjectManager->getObject(id);
	if (handle.isValid())
	{
		return handle;
	}

	auto storage = create(pythonObject, parentHandle, childPath, pDefinition, id);
	objects_.emplace_back(storage, id);
	return objects_.back().getHandle();
}

//------------------------------------------------------------------------------
ObjectHandle PythonObjManager::find(const PyScript::ScriptObject& pythonObject)
{
	assert(pythonObject.exists());

	// Get a definition that's the same for each ScriptObject instance
	auto pRegistry = get<IScriptObjectDefinitionRegistry>();
	assert(pRegistry != nullptr);
	auto& registry = (*pRegistry);

	auto pDefinition = registry.findDefinition(pythonObject);
	if (pDefinition == nullptr)
	{
		return nullptr;
	}
	auto& definition = (*pDefinition);

	const auto& id = registry.getID(pythonObject);

	// Search for an existing object handle that's using that definition
	auto pObjectManager = get<IObjectManager>();
	assert(pObjectManager != nullptr);
	auto& objectManager = (*pObjectManager);
	return objectManager.getObject(id);
}

} // end namespace wgt