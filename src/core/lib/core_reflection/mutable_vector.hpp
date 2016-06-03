#ifndef MUTABLE_VECTOR_HPP
#define MUTABLE_VECTOR_HPP

#include <cassert>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

namespace wgt
{
template< typename T >
class ConstMutableIterator;

template< typename T >
class MutableIterator;

namespace internal
{
	template< typename T >
	struct DefaultComparator : std::unary_function< const T &, bool >
	{
		DefaultComparator( const T & compareTo )
			: compareTo_( compareTo )
		{

		}

		bool operator()( const T & value )
		{
			return compareTo_ == value;
		}

		const T & compareTo_;
	};

	template< typename T >
	struct DefaultComparator< std::weak_ptr< T > > : std::unary_function< std::weak_ptr< T >, bool >
	{
		DefaultComparator( std::weak_ptr< T > compareTo )
			: compareTo_( compareTo.lock() )
		{

		}

		bool operator()( std::weak_ptr< T > value )
		{
			return compareTo_ == value.lock();
		}

		std::shared_ptr< T > compareTo_;
	};
}
/**
 *	A vector in which you can safely iterate over it, while items are
 *	inserted/removed.
 *	
 *	The iterators will update their positions as items are inserted/removed.
 */
template< typename T, class Comparator = internal::DefaultComparator< T > >
class MutableVector
{
public:
	typedef ConstMutableIterator<T> const_iterator;
	typedef MutableIterator<T> iterator;
	typedef T value_type;

	MutableVector()
	{
	}

	iterator begin() const
	{
		std::lock_guard< std::mutex > guard( mutex_ );
		return iterator( *this, 0 );
	}

	const_iterator cbegin() const
	{
		std::lock_guard< std::mutex > guard( mutex_ );

		return const_iterator( *this, 0 );
	}

	iterator end() const
	{
		std::lock_guard< std::mutex > guard( mutex_ );
		return iterator( *this );
	}

	const_iterator cend() const
	{
		std::lock_guard< std::mutex > guard( mutex_ );

		return const_iterator( *this );
	}

	void push_back( const value_type& val )
	{
		std::lock_guard< std::mutex > guard( mutex_ );
		vector_.push_back( val );
	}

	void erase( const value_type& val )
	{
		std::lock_guard< std::mutex > guard( mutex_ );

		auto it = std::find_if( vector_.begin(), vector_.end(), Comparator( val ) );
		if (it == vector_.end())
		{
			return;
		}

		auto index = static_cast<size_t>( std::distance( vector_.begin(), it ) );
		vector_.erase( it );

		for ( auto iter : iterators_ )
		{
			if (index <= iter->index_)
			{
				--iter->index_;
			}
			--iter->size_;
		}
	}

	void clear()
	{
		std::lock_guard< std::mutex > guard( mutex_ );
		vector_.clear();
	}

private:
	std::vector<T> vector_;
	mutable std::set<const_iterator*> iterators_;
	mutable std::mutex mutex_;

	friend const_iterator;

	MutableVector( const MutableVector<T>& );
	MutableVector( MutableVector<T>&& );
	MutableVector& operator=( const MutableVector<T>& );
	MutableVector& operator=( MutableVector<T>&& );
};

template< typename T >
class ConstMutableIterator
{
public:
	typedef MutableVector<T> collection;

protected:
	// end iterator constructor
	ConstMutableIterator( const collection& collection )
		: collection_( collection )
		, index_( 0 )
		, size_( SIZE_MAX )
	{
	}

	ConstMutableIterator( const collection& collection, size_t index )
		: collection_( collection )
	{
		// Calling code is responsible for locking here
		auto inserted = collection_.iterators_.insert( this ).second;
		assert( inserted );
		index_ = index;
		size_ = collection_.vector_.size();
	}

public:
	ConstMutableIterator( const ConstMutableIterator<T>& other )
		: collection_( other.collection_ )
	{
		std::lock_guard< std::mutex > guard( collection_.mutex_ );

		auto inserted = collection_.iterators_.insert( this ).second;
		assert( inserted );
		index_ = other.index_;
		size_ = other.size_;
	}

	ConstMutableIterator( ConstMutableIterator<T>&& other )
		: collection_( other.collection_ )
	{
		std::lock_guard< std::mutex > guard( collection_.mutex_ );

		auto inserted = collection_.iterators_.insert( this ).second;
		assert( inserted );
		index_ = other.index_;
		size_ = other.size_;

		auto it = collection_.iterators_.find( &other );
		assert( it != collection_.iterators_.end() );
		collection_.iterators_.erase( it );
		other.index_ = 0;
		other.size_ = 0;
	}

	~ConstMutableIterator()
	{
		std::lock_guard< std::mutex > guard( collection_.mutex_ );

		auto it = collection_.iterators_.find( this );
		if ( it != collection_.iterators_.end() )
		{
			collection_.iterators_.erase( it );
		}
	}

	ConstMutableIterator& operator=( const ConstMutableIterator<T>& other )
	{
		if (this == &other)
		{
			return *this;
		}

		typename collection::Lock lock( collection_ );

		assert( &collection_ == &other.collection_ );
		index_ = other.index_;
		size_ = other.size_;

		return *this;
	}

	ConstMutableIterator& operator=( ConstMutableIterator<T>&& other )
	{
		if (this == &other)
		{
			return *this;
		}

		typename collection::Lock lock( collection_ );
		
		assert( &collection_ == &other.collection_ );
		index_ = other.index_;
		size_ = other.size_;

		auto it = collection_.iterators_.find( &other );
		assert( it != collection_.iterators_.end() );
		collection_.iterators_.erase( it );
		other.index_ = 0;
		other.size_ = 0;

		return *this;
	}

	const T& operator*() const
	{
		std::lock_guard< std::mutex > guard( collection_.mutex_ );

		assert( index_ < collection_.vector_.size() );
		return collection_.vector_[index_];
	}

	const T* operator->() const
	{
		return &operator*();
	}

	ConstMutableIterator& operator++()
	{
		++index_;
		return *this;
	}

	ConstMutableIterator operator++( int )
	{
		auto tmp = *this;
		++*this;
		return tmp;
	}

	ConstMutableIterator& operator--()
	{
		--index_;
		return *this;
	}

	ConstMutableIterator operator--( int )
	{
		auto tmp = *this;
		--*this;
		return tmp;
	}

	ConstMutableIterator& operator+=( size_t offset )
	{
		index_ += offset;
		return *this;
	}

	ConstMutableIterator operator+( size_t offset ) const
	{
		auto tmp = *this;
		return tmp += offset;
	}

	ConstMutableIterator& operator-=( size_t offset )
	{
		index_ -= offset;
		return *this;
	}

	ConstMutableIterator operator-( size_t offset ) const
	{
		auto tmp = *this;
		return tmp -= offset;
	}

	bool operator==( const ConstMutableIterator& other ) const
	{
		std::lock_guard< std::mutex > guard( collection_.mutex_ );

		if (&collection_ != &other.collection_)
		{
			return false;
		}

		if (index_ == other.index_ && size_ == other.size_)
		{
			return true;
		}

		auto atEnd = index_ == size_ || size_ == SIZE_MAX;
		auto otherAtEnd = other.index_ == other.size_ || other.size_ == SIZE_MAX;
		return atEnd == otherAtEnd;
	}

	bool operator!=( const ConstMutableIterator& other ) const
	{
		return !(*this == other);
	}

protected:
	const collection& collection_;
	size_t index_;
	size_t size_;

	friend collection;
};

template< typename T >
class MutableIterator : public ConstMutableIterator<T>
{
protected:
	// end iterator constructor
	MutableIterator( const typename ConstMutableIterator<T>::collection& collection )
		: ConstMutableIterator<T>::ConstMutableIterator( collection )
	{
	}

	MutableIterator( const typename ConstMutableIterator<T>::collection& collection, size_t index )
		: ConstMutableIterator<T>::ConstMutableIterator( collection, index )
	{
	}

public:
	MutableIterator( const ConstMutableIterator<T>& other )
		: ConstMutableIterator<T>::ConstMutableIterator( other )
	{
	}

	MutableIterator( ConstMutableIterator<T>&& other )
		: ConstMutableIterator<T>::ConstMutableIterator( other )
	{
	}

	T& operator*() const
	{
		assert( ConstMutableIterator<T>::index_ <
					 ConstMutableIterator<T>::collection_.vector_.size() );
		return ConstMutableIterator<T>::collection_.vector_[ConstMutableIterator<T>::index_];
	}

	T* operator->() const
	{
		return &operator*();
	}

protected:
	friend typename ConstMutableIterator<T>::collection;
};
} // end namespace wgt
#endif//MUTABLE_VECTOR_HPP
