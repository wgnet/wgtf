#pragma once
#ifndef _PYTHON_MAPPING_ITERATOR_HPP
#define _PYTHON_MAPPING_ITERATOR_HPP

#include "converters.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <type_traits>

namespace wgt
{
namespace PythonType
{
/**
 *	This class wraps a PyScript::ScriptDict
 *	with the Collection system's interface.
 */
class MappingIterator final : public CollectionIteratorImplBase
{
public:
	typedef PyScript::ScriptDict container_type;
	typedef PyScript::ScriptObject key_type;
	typedef PyScript::ScriptObject value_type;
	typedef MappingIterator this_type;

	MappingIterator(const ObjectHandle& containerHandle, const container_type& container,
	                const PyScript::ScriptList::size_type index, const Converters& typeConverters);
	MappingIterator(const ObjectHandle& containerHandle, const container_type& container, const key_type& key,
	                const Converters& typeConverters);

	const container_type& container() const;
	key_type rawKey() const;
	PyScript::ScriptList::size_type rawIndex() const;

	virtual const TypeId& keyType() const override;
	virtual const TypeId& valueType() const override;
	virtual Variant key() const override;
	virtual Variant value() const override;
	virtual bool setValue(const Variant& value) const override;
	virtual void inc( size_t advAmount = 1) override;
	virtual bool equals(const CollectionIteratorImplBase& that) const override;
	virtual CollectionIteratorImplPtr clone() const override;

private:
	ObjectHandle containerHandle_;
	container_type container_;

	// List and index used for ordering iterators
	// TODO NGT-1332 it is inefficient to generate a list for every iterator
	// especially if the iterator is never incremented
	PyScript::ScriptList keys_;
	PyScript::ScriptList::size_type index_;

	key_type key_;
	const Converters& typeConverters_;
};

} // namespace PythonType
} // end namespace wgt
#endif // _PYTHON_MAPPING_ITERATOR_HPP
