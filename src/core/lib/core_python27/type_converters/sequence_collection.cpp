#include "pch.hpp"

#include "sequence_collection.hpp"

#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
namespace PythonType
{
namespace Sequence_Detail
{
template <typename T>
typename std::enable_if<Sequence<T>::can_resize, CollectionIteratorImplPtr>::type insert(
const ObjectHandle& containerHandle, typename Sequence<T>::container_type& container,
const typename Sequence<T>::key_type i, const CollectionIteratorImplPtr& end, const Converters& typeConverters)
{
	auto noneType = PyScript::ScriptObject(Py_None, PyScript::ScriptObject::FROM_BORROWED_REFERENCE);

	typename Sequence<T>::key_type resultIndex = i;

	// Insert at start
	if (i <= 0)
	{
		// Only add 1 at start
		resultIndex = 0;
		const bool success = container.insert(0, noneType);
		if (!success)
		{
			return end;
		}
	}
	// Append to end
	else if (i >= container.size())
	{
		// Only add 1 at end
		resultIndex = container.size();
		const bool success = container.append(noneType);
		if (!success)
		{
			return end;
		}
	}
	// Insert in middle
	else
	{
		// Add 1 in middle
		const bool success = container.insert(i, noneType);
		if (!success)
		{
			return end;
		}
	}

	return std::make_shared<Sequence<T>::iterator_impl_type>(containerHandle, container, resultIndex, typeConverters);
}

template <typename T>
typename std::enable_if<!Sequence<T>::can_resize, CollectionIteratorImplPtr>::type insert(
const ObjectHandle& containerHandle, typename Sequence<T>::container_type& container,
const typename Sequence<T>::key_type i, const CollectionIteratorImplPtr& end, const Converters& typeConverters)
{
	NGT_ERROR_MSG("Cannot insert into container that does not resize\n");
	return end;
}

template <typename T>
typename std::enable_if<Sequence<T>::can_resize, typename Sequence<T>::result_type>::type erase(
const ObjectHandle& containerHandle, typename Sequence<T>::container_type& container,
const typename Sequence<T>::key_type first, const typename Sequence<T>::key_type last,
const CollectionIteratorImplPtr& end, const Converters& typeConverters)
{
	// [begin,end)
	if ((first < 0) || (first >= container.size()))
	{
		NGT_ERROR_MSG("First index is not within sequence\n");
		return Sequence<T>::result_type(end, false);
	}
	// (begin,end]
	if ((last <= 0) || (last > container.size()))
	{
		NGT_ERROR_MSG("Last index is not within sequence\n");
		return Sequence<T>::result_type(end, false);
	}
	// Bad range
	if (first >= last)
	{
		NGT_ERROR_MSG("First index must be before last index\n");
		return Sequence<T>::result_type(end, false);
	}

	const PyScript::ScriptList erase(nullptr);
	const bool success = container.setSlice(first, last, erase, PyScript::ScriptErrorPrint());
	// Container does not match iterators
	assert(success);
	if (!success)
	{
		NGT_ERROR_MSG("Could not erase item\n");
		return Sequence<T>::result_type(end, false);
	}

	// An iterator pointing to the new location of the element that followed
	// the last element erased by the function call.
	// This is the container end if the operation erased the last element in
	// the sequence.
	const size_t numErased = (last - first);
	const Sequence<T>::key_type newLastIndex = last - numErased;
	return Sequence<T>::result_type(
	std::make_shared<Sequence<T>::iterator_impl_type>(containerHandle, container, newLastIndex, typeConverters), true);
}

template <typename T>
typename std::enable_if<!Sequence<T>::can_resize, typename Sequence<T>::result_type>::type erase(
const ObjectHandle& containerHandle, typename Sequence<T>::container_type& container,
const typename Sequence<T>::key_type first, const typename Sequence<T>::key_type last,
const CollectionIteratorImplPtr& end, const Converters& typeConverters)
{
	NGT_ERROR_MSG("Cannot erase from container that does not resize\n");
	return Sequence<T>::result_type(end, false);
}

} // namespace Sequence_Detail

template <typename T>
Sequence<T>::Sequence(const typename Sequence<T>::container_type& container, const ObjectHandle& containerHandle,
                      const Converters& typeConverters)
    : CollectionImplBase(), container_(container), containerHandle_(containerHandle), typeConverters_(typeConverters)
{
}

template <typename T>
size_t Sequence<T>::size() const /* override */
{
	return container_.size();
}

template <typename T>
CollectionIteratorImplPtr Sequence<T>::begin() /* override */
{
	const key_type startIndex = 0;
	return std::make_shared<iterator_impl_type>(containerHandle_, container_, startIndex, typeConverters_);
}

template <typename T>
CollectionIteratorImplPtr Sequence<T>::end() /* override */
{
	const key_type endIndex = container_.size();
	return std::make_shared<iterator_impl_type>(containerHandle_, container_, endIndex, typeConverters_);
}

template <typename T>
typename Sequence<T>::result_type Sequence<T>::get(const Variant& key,
                                                   CollectionImplBase::GetPolicy policy) /* override */
{
	key_type i;
	if (!key.tryCast(i))
	{
		return result_type(this->end(), false);
	}

	// If you pass in a negative index,
	// Python adds the length of the sequence to the index.
	// E.g. list[-1] gets the last item in the list
	// SequenceIterator should always have an index in the range start-end
	if (i < 0)
	{
		i += container_.size();
	}

	if (policy == GET_EXISTING)
	{
		if (i < 0)
		{
			return result_type(this->end(), false);
		}
		if (i < container_.size())
		{
			return result_type(std::make_shared<iterator_impl_type>(containerHandle_, container_, i, typeConverters_),
			                   false);
		}
		else
		{
			return result_type(this->end(), false);
		}
	}
	else if (policy == GET_NEW)
	{
		auto itr = Sequence_Detail::insert<T>(containerHandle_, container_, i, this->end(), typeConverters_);
		return result_type(itr, itr != this->end());
	}
	else if (policy == GET_AUTO)
	{
		const bool found = ((i >= 0) && (i < container_.size()));
		if (found)
		{
			// Get existing
			return result_type(std::make_shared<iterator_impl_type>(containerHandle_, container_, i, typeConverters_),
			                   false);
		}

		// Insert new at start or end
		return result_type(Sequence_Detail::insert<T>(containerHandle_, container_, i, this->end(), typeConverters_),
		                   true);
	}
	else
	{
		assert(false && "Not implemented");
		return result_type(this->end(), false);
	}
}

template <typename T>
typename CollectionIteratorImplPtr Sequence<T>::insert(const Variant& key, const Variant& value) /* override */
{
	key_type i;
	if (!key.tryCast(i))
	{
		return this->end();
	}

	// If you pass in a negative index,
	// Python adds the length of the sequence to the index.
	// E.g. list[-1] gets the last item in the list
	// SequenceIterator should always have an index in the range start-end
	if (i < 0)
	{
		i += container_.size();
	}

	const auto insertItr = Sequence_Detail::insert<T>(containerHandle_, container_, i, this->end(), typeConverters_);
	if (insertItr != this->end())
	{
		insertItr->setValue(value);
	}
	return insertItr;
}

template <typename T>
CollectionIteratorImplPtr Sequence<T>::erase(const CollectionIteratorImplPtr& pos) /* override */
{
	const auto pItr = dynamic_cast<iterator_impl_type*>(pos.get());
	assert(pItr != nullptr);
	if (pItr == nullptr)
	{
		return this->end();
	}
	assert(container_ == pItr->container());
	if (container_ != pItr->container())
	{
		return this->end();
	}

	auto result = Sequence_Detail::erase<T>(containerHandle_, container_, pItr->rawIndex(), pItr->rawIndex() + 1,
	                                        this->end(), typeConverters_);

	return result.first;
}

template <typename T>
size_t Sequence<T>::eraseKey(const Variant& key) /* override */
{
	key_type index;
	if (!key.tryCast(index))
	{
		return 0;
	}

	const auto result =
	Sequence_Detail::erase<T>(containerHandle_, container_, index, index + 1, this->end(), typeConverters_);

	return result.second ? 1 : 0;
}

template <typename T>
CollectionIteratorImplPtr Sequence<T>::erase(const CollectionIteratorImplPtr& first,
                                             const CollectionIteratorImplPtr& last) /* override */
{
	const auto pFirst = dynamic_cast<iterator_impl_type*>(first.get());
	const auto pLast = dynamic_cast<iterator_impl_type*>(last.get());
	assert((pFirst != nullptr) && (pLast != nullptr));
	if ((pFirst == nullptr) || (pLast == nullptr))
	{
		return this->end();
	}
	assert((container_ == pFirst->container()) && (container_ == pLast->container()));
	if ((container_ != pFirst->container()) || (container_ != pLast->container()))
	{
		return this->end();
	}

	auto result = Sequence_Detail::erase<T>(containerHandle_, container_, pFirst->rawIndex(), pLast->rawIndex(),
	                                        this->end(), typeConverters_);

	return result.first;
}

template <typename T>
const TypeId& Sequence<T>::keyType() const /* override */
{
	return TypeId::getType<key_type>();
}

template <typename T>
const TypeId& Sequence<T>::valueType() const /* override */
{
	return TypeId::getType<value_type>();
}

template <typename T>
const TypeId& Sequence<T>::containerType() const /* override */
{
	return TypeId::getType<container_type>();
}

template <typename T>
const void* Sequence<T>::container() const /* override */
{
	return &container_;
}

template <typename T>
int Sequence<T>::flags() const /* override */
{
	return WRITABLE | RESIZABLE | ORDERED;
}

template <>
int Sequence<PyScript::ScriptTuple>::flags() const /* override */
{
	return WRITABLE | ORDERED;
}

// Explicit instantiations
template class Sequence<PyScript::ScriptList>;
template class Sequence<PyScript::ScriptSequence>;
template class Sequence<PyScript::ScriptTuple>;

} // namespace PythonType
} // end namespace wgt
