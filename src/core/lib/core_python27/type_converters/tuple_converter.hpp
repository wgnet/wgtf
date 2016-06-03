#pragma once
#ifndef _PYTHON_TUPLE_CONVERTER_HPP
#define _PYTHON_TUPLE_CONVERTER_HPP

#include "i_parent_type_converter.hpp"

namespace wgt
{
class IComponentContext;

namespace PythonType
{

class Converters;


/**
 *	Attempts to convert ScriptTuple<->Collection<->Variant.
 */
class TupleConverter final : public IParentConverter
{
public:
	TupleConverter( IComponentContext & context,
		const Converters & typeConverters );

	virtual bool toVariant( const PyScript::ScriptObject & inObject,
		Variant & outVariant,
		const ObjectHandle & parentHandle,
		const std::string & childPath ) override;
	virtual bool toScriptType( const Variant & inVariant,
		PyScript::ScriptObject & outObject, void* userData = nullptr ) override;
private:
	IComponentContext & context_;
	const Converters & typeConverters_;
};


} // namespace PythonType
} // end namespace wgt
#endif // _PYTHON_TUPLE_CONVERTER_HPP
