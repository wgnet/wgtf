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

	void * noneType = nullptr;
	outVariant = Variant( noneType );
	return true;
}


bool NoneConverter::toScriptType( const Variant & inVariant,
	PyScript::ScriptObject & outObject, void* userData ) /* override */
{
	// null void * -> None
	if (!inVariant.typeIs< Variant::traits< void * >::storage_type >())
	{
		return false;
	}

	void * ptr = nullptr;
	const bool success = inVariant.tryCast< void * >( ptr );
	if (success && (ptr == nullptr))
	{
		outObject = PyScript::ScriptObject::none();
		return true;
	}
	return false;
}


} // namespace PythonType
} // end namespace wgt
