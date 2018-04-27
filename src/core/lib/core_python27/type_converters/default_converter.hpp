#pragma once

#include "core_dependency_system/depends.hpp"
#include "../interfaces/i_python_obj_manager.hpp"

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
class DefaultConverter : public IParentConverter, public Depends<IPythonObjManager>
{
public:
	virtual bool toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant,
	                       const ObjectHandle& parentHandle, const std::string& childPath) override;

	virtual bool toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
	                          void* userData = nullptr) override;
};

} // namespace PythonType
} // end namespace wgt
