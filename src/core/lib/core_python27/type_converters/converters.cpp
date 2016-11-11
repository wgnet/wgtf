#include "pch.hpp"
#include "converters.hpp"

namespace wgt
{
namespace PythonType
{
Converters::Converters(const BasicTypeConverters& basicTypeConverters, const ParentTypeConverters& parentTypeConverters)
    : basicTypeConverters_(basicTypeConverters), parentTypeConverters_(parentTypeConverters)
{
}

bool Converters::toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject, void* userData) const
{
	const bool success = basicTypeConverters_.toScriptType(inVariant, outObject, userData);
	if (success)
	{
		return true;
	}
	return parentTypeConverters_.toScriptType(inVariant, outObject, userData);
}

bool Converters::toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant,
                           const ObjectHandle& parentHandle, const std::string& childPath) const
{
	const bool success = basicTypeConverters_.toVariant(inObject, outVariant);
	if (success)
	{
		return true;
	}
	return parentTypeConverters_.toVariantWithParent(inObject, outVariant, parentHandle, childPath);
}

} // namespace PythonType
} // end namespace wgt
