#include "pch.hpp"

#include "sequence_iterator.hpp"

#include "converters.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"

#include <cassert>

namespace wgt
{
namespace PythonType
{
namespace SequenceIterator_Detail
{
template <typename T>
PyScript::ScriptObject getItem(const typename SequenceIterator<T>::container_type& container,
                               const typename SequenceIterator<T>::key_type& index)
{
	return container.getItem(index);
}

template <>
PyScript::ScriptObject getItem<PyScript::ScriptSequence>(
const typename SequenceIterator<PyScript::ScriptSequence>::container_type& container,
const typename SequenceIterator<PyScript::ScriptSequence>::key_type& index)
{
	return container.getItem(index, PyScript::ScriptErrorPrint());
}

template <typename T>
bool setItem(const typename SequenceIterator<T>::container_type& container,
             const typename SequenceIterator<T>::key_type& index, PyScript::ScriptObject& scriptValue)
{
	return container.setItem(index, scriptValue);
}

template <>
bool setItem<PyScript::ScriptSequence>(
const typename SequenceIterator<PyScript::ScriptSequence>::container_type& container,
const typename SequenceIterator<PyScript::ScriptSequence>::key_type& index, PyScript::ScriptObject& scriptValue)
{
	return container.setItem(index, scriptValue, PyScript::ScriptErrorPrint());
}

} // namespace SequenceIterator_Detail

template <typename T>
SequenceIterator<T>::SequenceIterator(const ObjectHandle& containerHandle, const container_type& container,
                                      typename SequenceIterator<T>::key_type index, const Converters& typeConverters)
    : containerHandle_(containerHandle), container_(container), index_(index), typeConverters_(typeConverters)
{
	// Does not support negative indexes
	assert(index_ >= 0);
}

template <typename T>
const typename SequenceIterator<T>::container_type& SequenceIterator<T>::container() const
{
	return container_;
}

template <typename T>
typename SequenceIterator<T>::key_type SequenceIterator<T>::rawIndex() const
{
	return index_;
}

template <typename T>
const TypeId& SequenceIterator<T>::keyType() const
{
	return TypeId::getType<key_type>();
}

template <typename T>
const TypeId& SequenceIterator<T>::valueType() const
{
	// TODO: can we do it more effectively?
	return value().type()->typeId();
}

template <typename T>
Variant SequenceIterator<T>::key() const /* override */
{
	return Variant(index_);
}

template <typename T>
Variant SequenceIterator<T>::value() const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG("IndexError: sequence index out of range\n");
		return Variant();
	}

	PyScript::ScriptObject item = SequenceIterator_Detail::getItem<T>(container_, index_);

	Variant result;
	std::string childPath;
	childPath += IClassDefinition::INDEX_OPEN;
	childPath += std::to_string(index_);
	childPath += IClassDefinition::INDEX_CLOSE;
	const bool success = typeConverters_.toVariant(item, result, containerHandle_, childPath);
	return result;
}

template <typename T>
bool SequenceIterator<T>::setValue(const Variant& value) const /* override */
{
	if ((index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG("IndexError: sequence assignment index out of range\n");
		return false;
	}

	PyScript::ScriptObject scriptValue;
	const bool success = typeConverters_.toScriptType(value, scriptValue);
	if (!success)
	{
		return false;
	}

	return SequenceIterator_Detail::setItem<T>(container_, index_, scriptValue);
}

template <typename T>
void SequenceIterator<T>::inc() /* override */
{
	++index_;
}

template <typename T>
bool SequenceIterator<T>::equals(const CollectionIteratorImplBase& that) const /* override */
{
	const this_type* t = dynamic_cast<const this_type*>(&that);
	assert(t);
	if (!t)
	{
		return false;
	}

	assert(container_ == t->container_);
	return (index_ == t->index_);
}

template <typename T>
CollectionIteratorImplPtr SequenceIterator<T>::clone() const /* override */
{
	return std::make_shared<this_type>(*this);
}

// Explicit instantiations
template class SequenceIterator<PyScript::ScriptList>;
template class SequenceIterator<PyScript::ScriptSequence>;
template class SequenceIterator<PyScript::ScriptTuple>;

} // namespace PythonType
} // end namespace wgt
