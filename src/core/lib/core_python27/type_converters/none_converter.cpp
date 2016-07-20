#include "pch.hpp"
#include "none_converter.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace wgt
{
namespace PythonType
{


bool NoneConverter::toVariant( const PyScript::ScriptObject & inObject,
	Variant & outVariant ) /* override */
{
	// Check for types.NoneType
	if (!inObject.isNone())
	{
		return false;
	}

	outVariant = Variant( nullptr );
	return true;
}


bool NoneConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject, void* userData ) /* override */
{
	// null void * -> None
	if (!inVariant.isNullPointer() || !inVariant.typeIs< void >())
	{
		return false;
	}

	outObject = PyScript::ScriptObject::none();
	return true;
}


} // namespace PythonType
} // end namespace wgt
