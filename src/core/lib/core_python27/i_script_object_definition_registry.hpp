#pragma once
#include <memory>

namespace wgt
{
class IClassDefinition;
class RefObjectId;

namespace PyScript
{
class ScriptObject;
}

/**
 *	This class is for keeping a unique class definition for every Python object.
 */
struct IScriptObjectDefinitionRegistry
{
	/**
	 *	Registers a ScriptObject with the engine.
	 *	Returns existing definition if object was previously registered.
	 *	@pre object must not be null.
	 *  @return a IClassDefinition for the associated ScriptObject.
	 *		Should not be null.
	 */
	virtual std::shared_ptr<IClassDefinition> findOrCreateDefinition(const PyScript::ScriptObject& object) = 0;

	/**
	 *	Find a ScriptObject that has been registered with the engine.
	 *	@pre object must not be null.
	 *  @return a IClassDefinition for the associated ScriptObject.
	 *		Or null if not found.
	 */
	virtual std::shared_ptr<IClassDefinition> findDefinition(const PyScript::ScriptObject& object) = 0;

	/**
	 *	Get the ID for each ScriptObject.
	 *	@pre object must not be null.
	 *	@pre object must have been registered with getDefinition() first.
	 *  @return an ID for the associated ScriptObject.
	 */
	virtual const RefObjectId& getID(const PyScript::ScriptObject& object) = 0;
};
} // end namespace wgt
