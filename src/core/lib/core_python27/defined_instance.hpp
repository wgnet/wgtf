#pragma once
#ifndef PYTHON_DEFINED_INSTANCE_HPP
#define PYTHON_DEFINED_INSTANCE_HPP


#include "core_reflection/generic/base_generic_object.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <memory>


namespace wgt
{
class IComponentContext;


namespace ReflectedPython
{


/**
 *	A Python Defined Instance is the bridge between an ObjectHandle and a
 *	PyObject*.
 *	
 *	When a PyObject* is converted into a Defined Instance, it will have all of
 *	its members and methods accessible via the NGT Reflection System.
 *	
 *	Python definitions are "generic types".
 *	
 *	Generic types are not based on a static class definition, like a C++ class.
 *	Because generic types can add and remove members at runtime, they must
 *	provide a different class definition per instance.
 *	
 *	So Defined Instance inherits from BaseGenericObject in order to provide a
 *	different class definition for each instance of PyObject*.
 *	
 *	@see GenericObject, QtScriptObject.
 */
class DefinedInstance : public BaseGenericObject
{
public:

	/**
	 *	Do not use this function. Use DefinedInstance::findOrCreate().
	 *	It is required to be implemented by the .mpp implementation.
	 *	But the lifetime of Python objects cannot managed by ObjectManager, so
	 *	always create this object with the other constructor provided.
	 */
	DefinedInstance();
	~DefinedInstance();

	static ObjectHandle findOrCreate( IComponentContext & context,
		const PyScript::ScriptObject & pythonObject,
		const ObjectHandle & parentHandle,
		const std::string & childPath );

	static ObjectHandle find( IComponentContext & context,
		const PyScript::ScriptObject & pythonObject );

	const PyScript::ScriptObject & pythonObject() const;
	const DefinedInstance & root() const;
	const std::string & fullPath() const;

private:
	/**
	 *	Construct a class definition from the given Python object.
	 */
	DefinedInstance(
		IComponentContext & context,
		const PyScript::ScriptObject & pythonObject,
		std::shared_ptr< IClassDefinition > & definition,
		const ObjectHandle & parentHandle,
		const std::string & childPath );

	// Prevent copy and move
	// There should only be one DefinedInstance per PyScript::ScriptObject
	// Existing DefinedInstances can be found using DefinedInstance::find()
	DefinedInstance( const DefinedInstance & other );
	DefinedInstance( DefinedInstance && other );
	DefinedInstance & operator=( const DefinedInstance & other );
	DefinedInstance & operator=( DefinedInstance && other );

	ObjectHandle getDerivedType() const override;
	ObjectHandle getDerivedType() override;

	/**
	 *	This is here purely to keep a reference to the Python object.
	 */
	const PyScript::ScriptObject pythonObject_;

	/**
	 *	Methods and members in pythonObject_ are added to this definition to
	 *	be used by NGT reflection.
	 */
	std::shared_ptr<IClassDefinition> pDefinition_;

	IComponentContext * context_;

	// Track parent object so that the reflection system can get the full
	// property path to this object
	// Need to keep a strong reference to the parent DefinedInstance
	// in the case of accessing properties like "parent.child1.child2",
	// then the only thing holding a reference alive to "child1" will be "child2"
	ObjectHandle parentHandle_;
	const DefinedInstance * pRoot_;
	std::string fullPath_;
};



} // namespace ReflectedPython
} // end namespace wgt
#endif // PYTHON_DEFINED_INSTANCE_HPP
