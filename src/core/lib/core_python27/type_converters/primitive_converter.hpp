#pragma once
#ifndef _PYTHON_PRIMITIVE_CONVERTER_HPP
#define _PYTHON_PRIMITIVE_CONVERTER_HPP

#include "i_type_converter.hpp"

namespace wgt
{
namespace PythonType
{


/**
 *	Attempts to convert ScriptObject<->T<->Variant.
 */
template< typename T >
class PrimitiveConverter : public IConverter
{
public:
	bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant ) override;
	bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject, void* userData = nullptr) override;
};


} // namespace PythonType
} // end namespace wgt
#endif // _PYTHON_PRIMITIVE_CONVERTER_HPP
