#pragma once

#include "i_type_converter.hpp"

namespace wgt
{
namespace PythonType
{
/**
 *	Attempts to convert None<->null void *<->Variant.
 */
class NoneConverter : public IConverter
{
public:
	virtual bool toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant) override;

	virtual bool toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
	                          void* userData = nullptr) override;
};

} // namespace PythonType
} // end namespace wgt
