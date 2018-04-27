#pragma once

#include "core_dependency_system/depends.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "i_script_object_definition_registry.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "core_reflection/interfaces/i_definition_helper.hpp"

#include <mutex>
#include <vector>

namespace wgt
{
class IClassDefinition;
class IComponentContext;
class IDefinitionManager;

namespace ReflectedPython
{
struct ScriptObjectDefinitionDeleter;

/**
 *	Stores an IClassDefinition for each PyScript::ScriptObject instance.
 *	So that duplicate IClassDefinitions are created for the same
 *	PyScript::ScriptObject instance.
 */
class ScriptObjectDefinitionRegistry : public Implements<IScriptObjectDefinitionRegistry>,
                                       public Depends<IDefinitionManager>
{
public:
	virtual ~ScriptObjectDefinitionRegistry();

	void init();
	void fini();

	/**
	 *	Find an existing or add a new definition for the given object.
	 *	Definitions will automatically be removed when the reference count
	 *	on the std::shared_ptr reaches 0.
	 *	@param object the PyScript::ScriptObject for which to find the definition.
	 *		Must not be null.
	 *	@return an existing definition or a newly added definition.
	 */
	virtual std::shared_ptr<IClassDefinition> findOrCreateDefinition(const PyScript::ScriptObject& object) override;

	virtual std::shared_ptr<IClassDefinition> findDefinition(const PyScript::ScriptObject& object) override;

	virtual const RefObjectId& getID(const PyScript::ScriptObject& object) override;

private:
	void removeDefinition(const PyScript::ScriptObject& object, const IClassDefinition* definition);

	friend struct ScriptObjectDefinitionDeleter;

	template <typename PAIR_T>
	friend class PairMatch;
	typedef std::pair<PyScript::ScriptObject, std::weak_ptr<IClassDefinition>> DefinitionPair;
	typedef std::vector<DefinitionPair> DefinitionLookup;
	DefinitionLookup definitions_;
	// TODO replace mutex with a check that it's the main thread
	// because Python scripts should only be run from a single thread
	std::mutex definitionsMutex_;

	std::unique_ptr<IDefinitionHelper> definitionHelper_;

	typedef std::pair<PyScript::ScriptObject, RefObjectId> IdPair;
	typedef std::vector<IdPair> IdLookup;
	IdLookup ids_;
};

} // namespace ReflectedPython
} // end namespace wgt
