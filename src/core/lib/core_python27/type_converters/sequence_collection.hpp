#pragma once
#ifndef _PYTHON_SEQUENCE_COLLECTION_HPP
#define _PYTHON_SEQUENCE_COLLECTION_HPP


#include "core_variant/collection.hpp"

#include "sequence_iterator.hpp"

#include "converters.hpp"
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
class Sequence final : public CollectionImplBase
{
public:
	static const bool is_supported =
		std::is_convertible< T, PyScript::ScriptSequence >::value;

	static_assert( is_supported,
		"T must inherit from a PyScript::ScriptSequence type" );

	static const bool can_resize =
		std::is_convertible< T, PyScript::ScriptList >::value;

	typedef Sequence base;
	typedef T container_type;
	typedef typename container_type::size_type key_type;
	typedef Variant value_type;
	typedef Sequence this_type;

	typedef SequenceIterator< T > iterator_impl_type;

	Sequence( const container_type & container,
		const ObjectHandle & containerHandle,
		const Converters & typeConverters );

	virtual size_t size() const override;

	virtual CollectionIteratorImplPtr begin() override;
	virtual CollectionIteratorImplPtr end() override;

	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;
	virtual result_type get( const Variant & key,
		CollectionImplBase::GetPolicy policy ) override;

	virtual CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr & pos ) override;
	virtual size_t eraseKey( const Variant & key ) override;
	virtual CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr & first,
		const CollectionIteratorImplPtr& last ) override;

	virtual const TypeId & keyType() const override;
	virtual const TypeId & valueType() const override;
	virtual const TypeId & containerType() const override;
	virtual const void * container() const override;
	virtual int flags() const override;

private:
	container_type container_;
	const ObjectHandle containerHandle_;
	const Converters & typeConverters_;
};


} // namespace PythonType
} // end namespace wgt
#endif // _PYTHON_SEQUENCE_COLLECTION_HPP
