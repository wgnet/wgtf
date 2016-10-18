#include "pch.hpp"
#include "mapping_collection.hpp"

#include "i_type_converter.hpp"

#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
namespace PythonType
{
namespace Mapping_Detail
{
CollectionIteratorImplPtr insert(const ObjectHandle& containerHandle,
                                 Mapping::container_type& container,
                                 const Mapping::key_type key,
                                 const CollectionIteratorImplPtr& end,
                                 const Converters& typeConverters)
{
	auto noneType = PyScript::ScriptObject( Py_None,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	const bool success = container.setItem( key,
		noneType,
		PyScript::ScriptErrorPrint() );
	if (success)
	{
		return std::make_shared<Mapping::iterator_impl_type>(
		containerHandle,
		container,
		key,
		typeConverters);
	}
	return end;
}

} // namespace Mapping_Detail

Mapping::Mapping( const Mapping::container_type & container,
	const ObjectHandle & containerHandle,
	const Converters & typeConverters )
	: CollectionImplBase()
	, container_( container )
	, containerHandle_( containerHandle )
	, typeConverters_( typeConverters )
{
}


size_t Mapping::size() const /* override */
{
	return container_.size();
}


CollectionIteratorImplPtr Mapping::begin() /* override */
{
	// Start index into dict.keys()
	const PyScript::ScriptList::size_type startIndex = 0;
	return std::make_shared< iterator_impl_type >( containerHandle_,
		container_,
		startIndex,
		typeConverters_ );
}


CollectionIteratorImplPtr Mapping::end() /* override */
{
	return std::make_shared< iterator_impl_type >( containerHandle_,
		container_,
		PyScript::ScriptObject( nullptr ),
		typeConverters_ );
}


Mapping::result_type Mapping::get( const Variant & key,
	CollectionImplBase::GetPolicy policy ) /* override */
{
	PyScript::ScriptObject scriptKey;
	const bool success = typeConverters_.toScriptType( key, scriptKey );
	if (!success)
	{
		return result_type( this->end(), false );
	}

	if (policy == GET_EXISTING)
	{
		return result_type(
			std::make_shared< iterator_impl_type >( containerHandle_,
				container_,
				scriptKey,
				typeConverters_ ),
			false );
	}
	else if (policy == GET_NEW)
	{
		auto itr = Mapping_Detail::insert(containerHandle_,
		                                  container_,
		                                  scriptKey,
		                                  this->end(),
		                                  typeConverters_);
		return result_type(itr, itr != this->end());
	}
	else if (policy == GET_AUTO)
	{
		auto scriptValue = container_.getItem( scriptKey,
			PyScript::ScriptErrorPrint() );
		if (scriptValue.exists())
		{
			// Get existing
			return result_type(
				std::make_shared< iterator_impl_type >( containerHandle_,
					container_,
					scriptKey,
					typeConverters_ ),
				false );
		}

		// Insert new at start or end
		return result_type(Mapping_Detail::insert(containerHandle_,
		                                          container_,
		                                          scriptKey,
		                                          this->end(),
		                                          typeConverters_),
		                   true);
	}
	else
	{
		assert( false && "Not implemented" );
		return result_type( this->end(), false );
	}
}

CollectionIteratorImplPtr Mapping::insert(const Variant& key,
                                          const Variant& value) /* override */
{
	PyScript::ScriptObject scriptKey;
	const bool success = typeConverters_.toScriptType(key, scriptKey);
	if (!success)
	{
		return this->end();
	}

	const auto insertItr = Mapping_Detail::insert(containerHandle_,
	                                              container_,
	                                              scriptKey,
	                                              this->end(),
	                                              typeConverters_);
	if (insertItr != this->end())
	{
		insertItr->setValue(value);
	}
	return insertItr;
}

CollectionIteratorImplPtr Mapping::erase(
	const CollectionIteratorImplPtr & pos ) /* override */
{
	const auto pItr = dynamic_cast< iterator_impl_type * >( pos.get() );
	if (pItr == nullptr)
	{
		NGT_ERROR_MSG( "Invalid iterator\n" );
		return this->end();
	}
	if (container_ != pItr->container())
	{
		NGT_ERROR_MSG( "Invalid iterator\n" );
		return this->end();
	}
	if ((pItr->rawIndex() < 0) || (pItr->rawIndex() >= container_.size()))
	{
		NGT_ERROR_MSG( "Iterator is not within map\n" );
		return this->end();
	}

	const bool removed = container_.delItem( pItr->rawKey(),
		PyScript::ScriptErrorPrint() );
	// Container does not match iterators
	assert( removed );
	if (!removed)
	{
		NGT_ERROR_MSG( "Could not erase item\n" );
		return this->end();
	}

	return std::make_shared< iterator_impl_type >( containerHandle_,
		container_,
		pItr->rawIndex(),
		typeConverters_ );
}


size_t Mapping::eraseKey( const Variant & key ) /* override */
{
	PyScript::ScriptObject scriptKey;
	const bool success = typeConverters_.toScriptType( key, scriptKey );
	assert( success );
	if (!success)
	{
		NGT_ERROR_MSG( "Invalid key\n" );
		return 0;
	}

	const bool removed = container_.delItem( scriptKey, PyScript::ScriptErrorPrint() );
	if (!removed)
	{
		NGT_ERROR_MSG( "Could not erase item\n" );
		return 0;
	}

	return 1;
}


CollectionIteratorImplPtr Mapping::erase( const CollectionIteratorImplPtr & first,
	const CollectionIteratorImplPtr& last ) /* override */
{
	const auto pFirst = dynamic_cast< iterator_impl_type * >( first.get() );
	const auto pLast = dynamic_cast< iterator_impl_type * >( last.get() );
	if ((pFirst == nullptr) || (pLast == nullptr))
	{
		NGT_ERROR_MSG( "Invalid iterator\n" );
		return this->end();
	}
	if ((container_ != pFirst->container()) || (container_ != pLast->container()))
	{
		NGT_ERROR_MSG( "Invalid iterator\n" );
		return this->end();
	}
	// [begin,end)
	if ((pFirst->rawIndex() < 0) || (pFirst->rawIndex() >= container_.size()))
	{
		NGT_ERROR_MSG( "Iterator is not within map\n" );
		return this->end();
	}
	// (begin,end]
	if ((pLast->rawIndex() <= 0) || (pLast->rawIndex() > container_.size()))
	{
		NGT_ERROR_MSG( "Iterator is not within map\n" );
		return this->end();
	}
	// Bad range
	if (pFirst->rawIndex() >= pLast->rawIndex())
	{
		NGT_ERROR_MSG( "First index must be before last index\n" );
		return this->end();
	}

	// Collect keys before erasing
	std::vector< key_type > foundKeys;
	auto current = (*pFirst);
	for (; !current.equals( *pLast ); current.inc())
	{
		foundKeys.push_back( current.rawKey() );
	}
	const auto sizeErased = current.rawIndex() - pFirst->rawIndex();
	const auto lastIndex = current.rawIndex() - sizeErased;

	for (const auto & key : foundKeys)
	{
		const bool removed = container_.delItem( key,
			PyScript::ScriptErrorPrint() );
		// Container does not match iterators
		// Container will be left in an inconsistent state
		assert( removed );
		if (!removed)
		{
			NGT_ERROR_MSG( "Could not erase item\n" );
			return this->end();
		}
	}

	return std::make_shared< iterator_impl_type >( containerHandle_,
		container_,
		lastIndex,
		typeConverters_ );
}


const TypeId & Mapping::keyType() const /* override */
{
	static auto s_KeyType = TypeId::getType< key_type >();
	return s_KeyType;
}


const TypeId & Mapping::valueType() const /* override */
{
	static auto s_ValueType = TypeId::getType< value_type >();
	return s_ValueType;
}


const TypeId & Mapping::containerType() const /* override */
{
	return TypeId::getType< container_type >();
}


const void * Mapping::container() const /* override */
{
	return &container_;
}


int Mapping::flags() const /* override */
{
	return
		MAPPING |
		WRITABLE |
		RESIZABLE;
}


} // namespace PythonType
} // end namespace wgt
