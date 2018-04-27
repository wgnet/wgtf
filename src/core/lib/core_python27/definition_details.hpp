#pragma once

#include "listener_hooks.hpp"

#include "core_reflection/interfaces/i_class_definition_details.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "interfaces/i_python_obj_manager.hpp"
#include "core_dependency_system/depends.hpp"

#include <memory>
#include <string>

namespace wgt
{

namespace ReflectedPython
{
/**
 *	Implements the IClassDefinitionDetails interface for Python objects.
 */
class DefinitionDetails : public IClassDefinitionDetails,
                          public IClassDefinitionModifier,
                          public Depends<IPythonObjManager>
{
public:
	DefinitionDetails(const PyScript::ScriptObject& pythonObject);
	~DefinitionDetails();

	bool isAbstract() const override;
	bool isGeneric() const override;
	const char* getName() const override;
	const MetaData & getMetaData() const override;
	ObjectHandleStoragePtr createObjectStorage(const IClassDefinition& definition) const override;
	ManagedObjectPtr createManaged(const IClassDefinition& definition, RefObjectId id = RefObjectId::zero()) const override;

	bool canDirectLookupProperty() const override;
	IBasePropertyPtr directLookupProperty(const char* name) const override;
	PropertyIteratorImplPtr getPropertyIterator() const override;
	IClassDefinitionModifier* getDefinitionModifier() const override;

	IBasePropertyPtr addProperty(const char* name, const TypeId& typeId, MetaData metaData,
	                             bool isCollection) override;
	void removeProperty(const char* name) override;

	/**
	 *	Calculate a reflected property name for the given script object.
	 *	@param object for which to generate the name.
	 *	@return copy of generated name.
	 */
	static std::string generateName(const PyScript::ScriptObject& object);

	/**
	 *	@return the script object that is defined by this.
	 */
	const PyScript::ScriptObject& object() const;

private:
	std::string name_;
	PyScript::ScriptObject pythonObject_;

	MetaData metaData_;
	PyScript::ScriptDict metaDataDict_;
};

} // namespace ReflectedPython
} // end namespace wgt
