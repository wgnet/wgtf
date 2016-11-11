#pragma once

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "i_parent_type_converter.hpp"

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
 *	Attempts to convert ScriptObject<->Variant.
 *	This is for any Python type that inherits from "object".
 */
class DefaultConverter : public IParentConverter
{
public:
	DefaultConverter(IComponentContext& context);

	virtual bool toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant,
	                       const ObjectHandle& parentHandle, const std::string& childPath) override;

	virtual bool toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
	                          void* userData = nullptr) override;

private:
	IComponentContext& context_;
};

} // namespace PythonType
} // end namespace wgt
