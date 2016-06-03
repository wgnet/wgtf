#include "pch.hpp"

#include "mapping_iterator.hpp"

#include "converters.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_variant/variant.hpp"

#include <cassert>


namespace wgt
{
namespace PythonType
{


MappingIterator::MappingIterator( const ObjectHandle & containerHandle,
	const container_type & container,
	const PyScript::ScriptList::size_type index,
	const Converters & typeConverters )
	: containerHandle_( containerHandle )
	, container_( container )
	, keys_( container_.keys( PyScript::ScriptErrorPrint() ) )
	, index_( index )
	, key_( nullptr )
	, typeConverters_( typeConverters )
{
	// Only supports positive indices
	assert( index_ >= 0 );
	if (index_ < container_.size())
	{
		key_ = keys_.getItem( index_ );
	}
}


MappingIterator::MappingIterator( const ObjectHandle & containerHandle,
	const container_type & container,
	const key_type & key,
	const Converters & typeConverters )
	: containerHandle_( containerHandle )
	, container_( container )
	, keys_( container_.keys( PyScript::ScriptErrorPrint() ) )
	, index_( 0 )
	, key_( key )
	, typeConverters_( typeConverters )
{
	if (!key_.exists())
	{
		index_ = container_.size();
	}
	else
	{
		bool found = false;
		// If the key is not found, then index_ == end
		for (; index_ < keys_.size(); ++index_)
		{
			auto scriptKey = keys_.getItem( index_ );
			if (key.compareTo( scriptKey, PyScript::ScriptErrorPrint() ) == 0)
			{
				found = true;
				break;
			}
		}

		// HACK NGT-1603 Try to cast key to an index
		// Work-around for how ReflectedPropertyItem::getChild will try to
		// access items with the a string "[index]"
		if (!found)
		{
			Variant result;
			ObjectHandle parentHandle;
			const char * childPath = "";
			const bool success = typeConverters_.toVariant( key_, result, parentHandle, childPath );
			PyScript::ScriptList::size_type fakeIndex = container_.size();
			const bool isIndex = result.tryCast( fakeIndex );

			if (isIndex && (fakeIndex >= 0) && (fakeIndex < container_.size()))
			{
				index_ = fakeIndex;
				key_ = keys_.getItem( index_ );
			}
		}
	}
}


const MappingIterator::container_type & MappingIterator::container() const
{
	return container_;
}


MappingIterator::key_type MappingIterator::rawKey() const
{
	return key_;
}


PyScript::ScriptList::size_type MappingIterator::rawIndex() const
{
	return index_;
}


const TypeId& MappingIterator::keyType() const
{
	// TODO: can we do it more effectively?
	return key().type()->typeId();
}


const TypeId& MappingIterator::valueType() const
{
	// TODO: can we do it more effectively?
	return value().type()->typeId();
}


Variant MappingIterator::key() const /* override */
{
	Variant result;
	ObjectHandle parentHandle;
	const std::string childPath = key_.str( PyScript::ScriptErrorPrint() ).c_str();
	const bool success = typeConverters_.toVariant( key_, result, parentHandle, childPath );
	assert( success );
	return result;
}


Variant MappingIterator::value() const /* override */
{
	if (!key_.exists() || (index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "KeyError: %s\n",
			key_.str( PyScript::ScriptErrorPrint() ).c_str() );
		return Variant();
	}

	PyScript::ScriptObject item = container_.getItem( key_,
		PyScript::ScriptErrorPrint() );
	
	Variant result;
	std::string childPath;
	childPath += INDEX_OPEN;
	childPath += key_.str( PyScript::ScriptErrorPrint() ).c_str();
	childPath += INDEX_CLOSE;
	const bool success = typeConverters_.toVariant( item, result, containerHandle_, childPath );
	assert( success );
	return result;
}


bool MappingIterator::setValue( const Variant & value ) const /* override */
{
	if (!key_.exists() || (index_ < 0) || (index_ >= container_.size()))
	{
		NGT_ERROR_MSG( "KeyError: %s\n",
			key_.str( PyScript::ScriptErrorPrint() ).c_str() );
		return false;
	}

	PyScript::ScriptObject scriptValue;
	const bool success = typeConverters_.toScriptType( value, scriptValue );
	if (!success)
	{
		return false;
	}

	return container_.setItem( key_,
		scriptValue,
		PyScript::ScriptErrorPrint() );
}


void MappingIterator::inc() /* override */
{
	++index_;
	if (index_ >= container_.size())
	{
		key_ = nullptr;
		return;
	}

	auto scriptKey = keys_.getItem( index_ );
	key_ = scriptKey;
}


bool MappingIterator::equals(
	const CollectionIteratorImplBase & that ) const /* override */
{
	const this_type * t = dynamic_cast< const this_type * >( &that );
	assert( t );
	if (!t)
	{
		return false;
	}

	assert( container_ == t->container_ );
	return (index_ == t->index_);
}


CollectionIteratorImplPtr MappingIterator::clone() const /* override */
{
	return std::make_shared< this_type >( *this );
}


} // namespace PythonType
} // end namespace wgt
