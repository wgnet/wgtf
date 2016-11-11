#pragma once

#include "core_script/type_converter_queue.hpp"
#include "i_type_converter.hpp"
#include "i_parent_type_converter.hpp"

#include <string>

namespace wgt
{
namespace PyScript
{
class ScriptObject;
} // namespace PyScript
class ObjectHandle;
class Variant;

namespace PythonType
{
typedef TypeConverterQueue<IConverter, PyScript::ScriptObject> BasicTypeConverters;
typedef TypeConverterQueue<IParentConverter, PyScript::ScriptObject> ParentTypeConverters;

/**
 *	Wrapper class for both TypeConverterQueue and DefaultConverter.
 *	DefaultConverter::toVariant() does not accept the same number of arguments
 *	as TypeConverterQueue.
 */
class Converters
{
public:
	Converters(const BasicTypeConverters& basicTypeConverters, const ParentTypeConverters& parentTypeConverters);

	/**
	 *	Convert the given Variant into a ScriptType by searching through the
	 *	type converters.
	 *
	 *	Search is performed from most-recently-added to first-added.
	 *
	 *	@param inVariant the variant to be converted.
	 *	@param outObject storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@return true on success.
	 */
	bool toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject, void* userData = nullptr) const;

	/**
	 *	Convert the given ScriptType into a Variant by searching through the
	 *	type converters.
	 *
	 *	Search is performed from most-recently-added to first-added.
	 *
	 *	@param inObject the ScriptType to be converted.
	 *	@param outVariant storage for the resulting object.
	 *		Should not be modified if conversion fails.
	 *	@param childPath path of inObject.
	 *	@return true on success.
	 */
	bool toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant, const ObjectHandle& parentHandle,
	               const std::string& childPath) const;

private:
	const BasicTypeConverters& basicTypeConverters_;
	const ParentTypeConverters& parentTypeConverters_;
};

} // namespace PythonType
} // end namespace wgt
