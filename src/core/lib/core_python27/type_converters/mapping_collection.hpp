#pragma once
#ifndef _PYTHON_MAPPING_COLLECTION_HPP
#define _PYTHON_MAPPING_COLLECTION_HPP


#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"

#include "mapping_iterator.hpp"

#include "converters.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <type_traits>


namespace wgt
{
namespace PythonType
{


/**
 *	Wraps a PyScript::ScriptDict with the Collection system's interface.
 *	Unordered map.
 */
class Mapping final : public CollectionImplBase
{
public:
	typedef Mapping base;
	typedef PyScript::ScriptDict container_type;
	typedef PyScript::ScriptObject key_type;
	typedef PyScript::ScriptObject value_type;
	typedef Mapping this_type;

	typedef MappingIterator iterator_impl_type;

	Mapping( const container_type & container,
		const ObjectHandle & containerHandle,
		const Converters & typeConverters );

	virtual size_t size() const override;

	virtual CollectionIteratorImplPtr begin() override;
	virtual CollectionIteratorImplPtr end() override;

	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;
	virtual result_type get(
		const Variant & key,
		CollectionImplBase::GetPolicy policy ) override;
	virtual CollectionIteratorImplPtr insert(const Variant& key,
	                                         const Variant& value) override;

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
	// Need to hold a reference to handle
	container_type container_;
	const ObjectHandle containerHandle_;
	const Converters & typeConverters_;
};


} // namespace PythonType
} // end namespace wgt
#endif // _PYTHON_MAPPING_COLLECTION_HPP
