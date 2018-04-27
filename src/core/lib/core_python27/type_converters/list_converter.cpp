#include "pch.hpp"

#include "list_converter.hpp"

#include "converters.hpp"
#include "sequence_collection.hpp"

#include "core_python27/defined_instance.hpp"
#include "core_variant/variant.hpp"
#include "wg_pyscript/py_script_object.hpp"

namespace wgt
{
namespace PythonType
{
ListConverter::ListConverter(const Converters& typeConverters) : IParentConverter(), typeConverters_(typeConverters)
{
}

bool ListConverter::toVariant(const PyScript::ScriptObject& inObject, Variant& outVariant,
                              const ObjectHandle& parentHandle, const std::string& childPath) /* override */
{
	if (!PyScript::ScriptList::check(inObject))
	{
		return false;
	}
	PyScript::ScriptList scriptList(inObject.get(), PyScript::ScriptObject::FROM_BORROWED_REFERENCE);
	auto listHandle = get<IPythonObjManager>()->findOrCreate(scriptList, parentHandle, childPath);
	assert(listHandle.isValid());

	auto collectionHolder = std::make_shared<Sequence<PyScript::ScriptList>>(scriptList, listHandle, typeConverters_);
	Collection collection(collectionHolder);
	outVariant = Variant(collection);
	return true;
}

bool ListConverter::toScriptType(const Variant& inVariant, PyScript::ScriptObject& outObject,
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
	// List is not a map
	if (value.isMapping())
	{
		return false;
	}
	// Lists must be able to add/remove elements
	if (!value.canResize())
	{
		return false;
	}
	// Check index type is numeric
	if ((value.keyType() != TypeId::getType<size_t>()) &&
	    (value.keyType() != TypeId::getType<Sequence<PyScript::ScriptList>::key_type>()))
	{
		return false;
	}

	const auto size = static_cast<PyScript::ScriptList::size_type>(value.size());
	auto scriptList = PyScript::ScriptList::create(size);

	auto itr = value.cbegin();
	for (PyScript::ScriptList::size_type i = 0; i < size; ++i)
	{
		const auto variantItem = (*itr);
		PyScript::ScriptObject scriptItem;
		const bool convertResult = typeConverters_.toScriptType(variantItem, scriptItem);
		if (!convertResult)
		{
			return false;
		}

		const bool setResult = scriptList.setItem(i, scriptItem);
		if (!setResult)
		{
			return false;
		}

		++itr;
	}

	outObject = scriptList;
	return true;
}

} // namespace PythonType
} // end namespace wgt
