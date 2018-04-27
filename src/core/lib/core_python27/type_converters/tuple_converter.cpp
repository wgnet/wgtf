#include "pch.hpp"

#include "tuple_converter.hpp"

#include "converters.hpp"
#include "sequence_collection.hpp"

#include "core_python27/defined_instance.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"

namespace wgt
{
namespace PythonType
{
TupleConverter::TupleConverter(const Converters& typeConverters) : IParentConverter(), typeConverters_(typeConverters)
{
}

bool TupleConverter::toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant,
                               const ObjectHandle& parentHandle, const std::string& childPath) /* override */
{
	if (!PyScript::ScriptTuple::check(inObject))
	{
		return false;
	}
	PyScript::ScriptTuple scriptTuple(inObject.get(), PyScript::ScriptObject::FROM_BORROWED_REFERENCE);
	auto tupleHandle = get<IPythonObjManager>()->findOrCreate(scriptTuple, parentHandle, childPath);
	assert(tupleHandle.isValid());

	auto collectionHolder =
	std::make_shared<Sequence<PyScript::ScriptTuple>>(scriptTuple, tupleHandle, typeConverters_);
	Collection collection(collectionHolder);
	outVariant = Variant(collection);
	return true;
}

bool TupleConverter::toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
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
	// Tuple is not a map
	if (value.isMapping())
	{
		return false;
	}
	// Tuples cannot add/remove elements
	if (value.canResize())
	{
		return false;
	}
	// Check index type is numeric
	if ((value.keyType() != TypeId::getType<size_t>()) &&
	    (value.keyType() != TypeId::getType<Sequence<PyScript::ScriptTuple>::key_type>()))
	{
		return false;
	}

	const auto size = static_cast<PyScript::ScriptTuple::size_type>(value.size());
	auto scriptTuple = PyScript::ScriptTuple::create(size);

	auto itr = value.cbegin();
	for (PyScript::ScriptTuple::size_type i = 0; i < size; ++i)
	{
		const auto variantItem = (*itr);
		PyScript::ScriptObject scriptItem;
		const bool convertResult = typeConverters_.toScriptType(variantItem, scriptItem);
		if (!convertResult)
		{
			return false;
		}

		const bool setResult = scriptTuple.setItem(i, scriptItem);
		if (!setResult)
		{
			return false;
		}

		++itr;
	}

	outObject = scriptTuple;
	return true;
}

} // namespace PythonType
} // end namespace wgt
