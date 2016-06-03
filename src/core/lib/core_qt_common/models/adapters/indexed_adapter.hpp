#ifndef INDEXED_ADAPTER_HPP
#define INDEXED_ADAPTER_HPP

#include <memory>


namespace wgt
{
template< class Adapter >
struct IndexedAdapter
{
	IndexedAdapter()
	{}

	// I've excluded default values here so that the compiler will choose
	// the operator== with QModelIndex where it can, and not construct
	// a new IndexedAdapter to compare to.
	IndexedAdapter(
		QPersistentModelIndex index,
		Adapter* data )
		: index_( index )
		, data_( data )
	{}

	IndexedAdapter( IndexedAdapter< Adapter >&& rhs )
		: index_( rhs.index_ )
		, data_( rhs.data_.release() )
	{
	}

	IndexedAdapter< Adapter >& operator=( IndexedAdapter< Adapter >&& rhs )
	{
		if (this != &rhs)
		{
			index_ = rhs.index_;
			data_.reset( rhs.data_.release() );
		}

		return *this;
	}
	
	bool operator==( const IndexedAdapter< Adapter >& rhs ) const
	{
		return index_ == rhs.index_;
	}

	bool operator==( const QModelIndex& index ) const
	{
		return index_ == index;
	}

	QPersistentModelIndex index_;
	std::unique_ptr< Adapter > data_;
};

template< typename T >
void isolateRedundantIndices( 
	const QModelIndex& parent,
	std::vector< IndexedAdapter< T > >& indices, 
	std::vector< std::unique_ptr< T > >& redundantAdapters )
{
	for (;;)
	{
		auto predicate = [&parent]( const IndexedAdapter< T >& item )
		{
			auto itemParent = item.index_.parent();
			return itemParent.internalId() == parent.internalId(); 
		};
		auto it = std::find_if( indices.begin(), indices.end(), predicate );
		if (it == indices.end())
		{
			break;
		}
		isolateRedundantIndex( it, indices, redundantAdapters );
	}
}

template< typename T >
void isolateRedundantIndices( 
	const QModelIndex& parent, int firstRow, int lastRow,
	std::vector< IndexedAdapter< T > >& indices, 
	std::vector< std::unique_ptr< T > >& redundantAdapters )
{
	for (auto row = firstRow; row <= lastRow; ++row)
	{
		auto predicate = [&parent, row]( const IndexedAdapter< T >& item )
		{
			auto itemParent = item.index_.parent();
			auto itemRow = item.index_.row();
			return itemParent.internalId() == parent.internalId() && 
				itemRow == row; 
		};
		auto it = std::find_if( indices.begin(), indices.end(), predicate );
		isolateRedundantIndex( it, indices, redundantAdapters );
	}
}

template< typename T >
void isolateRedundantIndex( 
	const QModelIndex& index,
	std::vector< IndexedAdapter< T > >& indices, 
	std::vector< std::unique_ptr< T > >& redundantAdapters )
{
	auto predicate = [&index]( const IndexedAdapter< T >& item )
	{ 
		return item.index_.internalId() == index.internalId(); 
	};
	auto it = std::find_if( indices.begin(), indices.end(), predicate );
	isolateRedundantIndex( it, indices, redundantAdapters );
}

template< typename T >
void isolateRedundantIndex( 
	const typename std::vector< IndexedAdapter< T > >::iterator& it,
	std::vector< IndexedAdapter< T > >& indices, 
	std::vector< std::unique_ptr< T > >& redundantAdapters )
{
	if (it == indices.end())
	{
		return;
	}
	
	auto redundantIndex = it->index_;
	auto redundantData = it->data_.release();

	redundantAdapters.emplace_back( redundantData );

	auto index = it - indices.begin();
	const size_t last = indices.size() - 1;
	if (index != last)
	{
		std::swap( indices[ index ], indices[ last ] );
	}
	indices.pop_back();

	// recursively check children for found indices
	isolateRedundantIndices( redundantIndex, indices, redundantAdapters );
}
} // end namespace wgt
#endif // INDEXED_ADAPTER_HPP
