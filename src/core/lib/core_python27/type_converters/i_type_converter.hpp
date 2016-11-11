#pragma once
#ifndef I_PYTHON_TYPE_CONVERTER_HPP
#define I_PYTHON_TYPE_CONVERTER_HPP

namespace wgt
{
namespace PyScript
{
class ScriptObject;
} // namespace PyScript

class Variant;

namespace PythonType
{
/**
 *	Interface for adding conversion of types between Python scripts with C++.
 *	Implement this interface to allow conversion of custom types.
 */
class IConverter
{
public:
	/**
	 *	Convert the given ScriptObject to a Variant.
	 *	Interface required by TypeConverterQueue.
	 *
	 *	@param inObject the object to be converted. It has to point to a valid PyObject.
	 *	@param outVariant storage for the converted item.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	virtual bool toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant) = 0;

	/**
	 *	Convert the given Variant to a ScriptObject.
	 *	Interface required by TypeConverterQueue.
	 *
	 *	@param outVariant the object to be converted.
	 *	@param outObject storage for the converted item.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	virtual bool toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
	                          void* userData = nullptr) = 0;
};

} // namespace PythonType
} // end namespace wgt
#endif // I_PYTHON_TYPE_CONVERTER_HPP
