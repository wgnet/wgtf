#pragma once
#ifndef _PYTHON_SEQUENCE_ITERATOR_HPP
#define _PYTHON_SEQUENCE_ITERATOR_HPP


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
 *	This class wraps a PyScript::ScriptSequence or ScriptList or ScriptTuple
 *	with the Collection system's interface.
 */
template< typename T >
class SequenceIterator final : public CollectionIteratorImplBase
{
public:
	static const bool is_supported =
		std::is_convertible< T, PyScript::ScriptSequence >::value;

	static_assert( is_supported,
		"T must inherit from a PyScript::ScriptSequence type" );

	typedef T container_type;
	typedef typename container_type::size_type key_type;
	typedef Variant value_type;
	typedef SequenceIterator< T > this_type;

	SequenceIterator( const ObjectHandle & containerHandle,
		const container_type & container,
		key_type index,
		const Converters & typeConverters );

	const container_type & container() const;
	/**
	 *	Get the index into the list.
	 *	As an int, not as a Variant like the key() method.
	 *	
	 *	In Python if you pass in a negative index,
	 *	Python adds the length of the sequence to the index.
	 *	E.g. list[-1] gets the last item in the list
	 *	For this implementation, negative indexes should be checked and
	 *	converted to a positive one in the range start-end.
	 */
	key_type rawIndex() const;

	virtual const TypeId& keyType() const override;
	virtual const TypeId& valueType() const override;
	virtual Variant key() const override;
	virtual Variant value() const override;
	virtual bool setValue( const Variant & value ) const override;
	virtual void inc() override;
	virtual bool equals( const CollectionIteratorImplBase & that ) const override;
	virtual CollectionIteratorImplPtr clone() const override;

private:
	ObjectHandle containerHandle_;
	container_type container_;
	key_type index_;
	const Converters & typeConverters_;
};


} // namespace PythonType
} // end namespace wgt
#endif // _PYTHON_SEQUENCE_ITERATOR_HPP
