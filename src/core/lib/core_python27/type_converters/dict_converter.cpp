#include "pch.hpp"

#include "dict_converter.hpp"

#include "converters.hpp"
#include "mapping_collection.hpp"

#include "core_python27/defined_instance.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"

namespace wgt
{
namespace PythonType
{
DictConverter::DictConverter(const Converters& typeConverters) : IParentConverter(), typeConverters_(typeConverters)
{
}

bool DictConverter::toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant,
                              const ObjectHandle& parentHandle, const std::string& childPath) /* override */
{
	if (!PyScript::ScriptDict::check(inObject))
	{
		return false;
	}
	PyScript::ScriptDict scriptDict(inObject.get(), PyScript::ScriptObject::FROM_BORROWED_REFERENCE);
	auto dictHandle = get<IPythonObjManager>()->findOrCreate(scriptDict, parentHandle, childPath);
	assert(dictHandle.isValid());

	auto collectionHolder = std::make_shared<Mapping>(scriptDict, dictHandle, typeConverters_);
	Collection collection(collectionHolder);
	outVariant = Variant(collection);
	return true;
}

bool DictConverter::toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
                                 void* userData) /* override */
{
	if (!inVariant.typeIs<Variant::traits<Collection>::storage_type>())
	{
		return false;
	}
	Collection value;
	const auto isCollection = inVariant.tryCast<Collection>(value);
	if (!isCollection)
	{
		return false;
	}
	// Must be a map
	if (!value.isMapping())
	{
		return false;
	}
	// TODO NGT-1332 check index type is hashable

	const auto size = static_cast<PyScript::ScriptDict::size_type>(value.size());
	auto scriptDict = PyScript::ScriptDict::create(size);

	for (auto itr = value.cbegin(); itr != value.cend(); ++itr)
	{
		const auto variantKey = itr.key();
		PyScript::ScriptObject scriptKey;
		const bool convertedKey = typeConverters_.toScriptType(variantKey, scriptKey);
		if (!convertedKey)
		{
			return false;
		}

		const auto variantItem = itr.value();
		PyScript::ScriptObject scriptValue;
		const bool convertedValue = typeConverters_.toScriptType(variantItem, scriptValue);
		if (!convertedValue)
		{
			return false;
		}

		const bool setResult = scriptDict.setItem(scriptKey, scriptValue, PyScript::ScriptErrorPrint());
		if (!setResult)
		{
			return false;
		}
	}

	outObject = scriptDict;
	return true;
}

} // namespace PythonType
} // end namespace wgt
