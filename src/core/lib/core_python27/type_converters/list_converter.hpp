#pragma once
#ifndef PYTHON_LIST_CONVERTER_HPP
#define PYTHON_LIST_CONVERTER_HPP

#include "i_parent_type_converter.hpp"

namespace wgt
{
class IComponentContext;

namespace PythonType
{
class Converters;

/**
 *	Attempts to convert ScriptList<->Collection<->Variant.
 */
class ListConverter final : public IParentConverter
{
public:
	ListConverter(IComponentContext& context, const Converters& typeConverters);

	virtual bool toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant,
	                       const ObjectHandle& parentHandle, const std::string& childPath) override;
	virtual bool toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
	                          void* userData = nullptr) override;

private:
	IComponentContext& context_;
	const Converters& typeConverters_;
};

} // namespace PythonType
} // end namespace wgt
#endif // PYTHON_LIST_CONVERTER_HPP
