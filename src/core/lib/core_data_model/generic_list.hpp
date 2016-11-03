#ifndef GENERIC_LIST_HPP
#define GENERIC_LIST_HPP

#include "i_list_model.hpp"
#include "i_item.hpp"
#include "i_item_role.hpp"
#include "core_variant/variant.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include <memory>

namespace wgt
{
template< typename T >
class GenericListT;

template< typename T >
class GenericListItemT : public IItem
{
public:
	GenericListItemT( const T& value )
		:value_( value )
	{
	}

	GenericListItemT( T&& value )
		: value_( value )
	{
	}

	virtual ~GenericListItemT()
	{
	}

	// IItem
	const char * getDisplayText( int column ) const override
	{
		return nullptr;
	}
	ThumbnailData getThumbnail( int column ) const override
	{
		return nullptr;
	}
	Variant getData( int column, ItemRole::Id roleId ) const override
	{
		if (roleId == ValueTypeRole::roleId_)
		{
			return typeid( T ).name();
		}
		else if (roleId == ValueRole::roleId_)
		{
			return value_;
		}
		else if (roleId == IndexPathRole::roleId_)
		{
			ResizingMemoryStream dataStream;
			TextStream s(dataStream);
			s << Variant( value_ );
			return dataStream.takeBuffer();
		}

		return Variant();
	}
	bool setData( int column, ItemRole::Id roleId, const Variant & data ) override
	{
		if (roleId != ValueRole::roleId_)
		{
			return false;
		}
		T tmp;
		bool isOk = data.tryCast( tmp );
		if (!isOk)
		{
			return false;
		}
		value_ = tmp;
		return true;
	}
	//

private:
	T value_;

	friend class GenericListT<T>;
};

template < typename T >
class GenericListT
	: public IListModel
{
public:
	GenericListT() {}
	virtual ~GenericListT() {}

	// IListModel
	IItem * item( size_t index ) const override
	{
		if (index >= items_.size())
		{
			return nullptr;
		}

		return items_[index].get();
	}

	size_t index( const IItem * item ) const override
	{
		auto index = 0;
		auto it = items_.begin();
		for (; it != items_.end() && it->get() != item; ++index, ++it) {}
		assert( it != items_.end() );
		return index;
	}

	bool empty() const override
	{
		return items_.empty();
	}
	
	size_t size() const override
	{
		return items_.size();
	}

	int columnCount() const override
	{
		return 1;
	}

	bool canClear() const override
	{
		return true;
	}

	void clear() override
	{
		this->resize( 0 );
	}
	//

	typedef std::vector< std::unique_ptr< IItem > > Items;

	class ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::const_iterator::difference_type difference_type;
		typedef const T value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		ConstIterator( const ConstIterator& rhs )
			: iterator_( new Items::const_iterator( *rhs.iterator_ ) )
		{
		}
		ConstIterator& operator=( const ConstIterator& rhs )
		{
			if (this != &rhs)
			{
				iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
			}
			return *this;
		}

		reference operator*( ) const
		{
			return *operator->();
		}
		pointer operator->() const
		{
			auto item = static_cast< GenericListItemT< T > * >( (*this->iterator_)->get() );
			const T & value = item->value_;

			return &value;
		}

		ConstIterator & operator++( )
		{
			++(*iterator_);
			return *this;
		}
		ConstIterator operator++( int )
		{
			ConstIterator tmp = *this;
			++(*this);
			return tmp;
		}
		bool operator==( const ConstIterator & other ) const
		{
			return *iterator_ == *other.iterator_;
		}
		bool operator!=( const ConstIterator & other ) const
		{
			return !(*this == other);
		}
		bool operator<( const ConstIterator & other ) const
		{
			return *iterator_ < *other.iterator_;
		}
		difference_type operator-( const ConstIterator & other ) const
		{
			return *iterator_ - *other.iterator_;
		}
		ConstIterator operator+(difference_type n) const
		{
			return *iterator_ + n;
		}

	protected:
		ConstIterator() {}
		ConstIterator( const Items::const_iterator & iterator )
			: iterator_( new Items::const_iterator( iterator ) )
		{
		}

		const Items::const_iterator& iterator() const
		{
			return *iterator_;
		}

		std::unique_ptr<Items::const_iterator> iterator_;

		friend class GenericListT<T>;
	};

	class Iterator : public ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::iterator::difference_type difference_type;
		typedef T value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		Iterator( const Iterator& rhs )
			: ConstIterator()
		{
			this->iterator_.reset( new Items::const_iterator( rhs.iterator() ) );
		}
		Iterator& operator=( const Iterator& rhs )
		{
			if (this != &rhs)
			{
				this->iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
			}

			return *this;
		}

		reference operator*( ) const
		{
			return *operator->();
		}
		pointer operator->() const
		{
			auto item = static_cast< GenericListItemT< T > * >( (*this->iterator_)->get() );
			T & value = item->value_;

			return &value;
		}

		Iterator & operator++( )
		{
			++(*this->iterator_);
			return *this;
		}
		Iterator operator++( int )
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}
		Iterator operator+(difference_type n) const
		{
			*this->iterator_ += n;
			return *this;
		}

	private:
		Iterator( const Items::iterator & iterator )
			: ConstIterator()
		{
			this->iterator_.reset( new Items::const_iterator( iterator ) );
		}

		const Items::const_iterator& iterator() const
		{
			return *( this->iterator_.get() );
		}

		friend class GenericListT<T>;
	};

	void resize( size_t newSize )
	{
		auto oldSize = size();
		if (newSize < oldSize)
		{
			signalPreItemsRemoved( newSize, oldSize - newSize );
			items_.resize( newSize );
			signalPostItemsRemoved( newSize, oldSize - newSize );
		}
		else if (newSize > oldSize)
		{
			signalPreItemsInserted( oldSize, newSize - oldSize );
			items_.resize( newSize );
			signalPostItemsInserted( oldSize, newSize - oldSize );
		}
	}

	ConstIterator cbegin() const
	{
		return ConstIterator( items_.cbegin() );
	}
	ConstIterator cend() const
	{
		return ConstIterator( items_.cend() );
	}
	ConstIterator begin() const
	{
		return ConstIterator(items_.cbegin());
	}
	ConstIterator end() const
	{
		return ConstIterator(items_.cend());
	}
	Iterator begin()
	{
		return Iterator( items_.begin() );
	}
	Iterator end()
	{
		return Iterator( items_.end() );
	}

	Iterator insert( const Iterator & position, const T & value )
	{
		auto index = std::distance( items_.cbegin(), position.iterator() );

		signalPreItemsInserted( index, 1 );
		auto it = items_.emplace(
			position.iterator(), new GenericListItemT<T>( value ) );
		signalPostItemsInserted( index, 1 );

		return it;
	}
	Iterator erase( const Iterator & position )
	{
		auto index = std::distance( items_.cbegin(), position.iterator() );

		signalPreItemsRemoved( index, 1 );
		auto it = items_.erase( position.iterator() );
		signalPostItemsRemoved( index, 1 );

		return it;
	}
	Iterator erase( const Iterator & first, const Iterator & last )
	{
		auto index = std::distance( items_.cbegin(), first.iterator() );
		auto count = std::distance( first.iterator(), last.iterator() );

		signalPreItemsRemoved( index, count );
		auto it = items_.erase( first.iterator(), last.iterator() );
		signalPostItemsRemoved( index, count );

		return it;
	}

	void emplace_back( T && value )
	{
		const auto index = items_.size();

		signalPreItemsInserted( index, 1 );
		items_.emplace( items_.end(), new GenericListItemT<T>( std::move( value ) ) );
		signalPostItemsInserted(  index, 1 );
	}
	
	void push_back( const T & value )
	{
		const auto index = items_.size();
		
		signalPreItemsInserted( index, 1 );
		items_.emplace( items_.end(), new GenericListItemT<T>( value ) );
		signalPostItemsInserted( index, 1 );
	}
	
	void push_front( const T & value )
	{
		auto index = 0;

		signalPreItemsInserted( index, 1 );
		items_.emplace( items_.begin(), new GenericListItemT<T>( value ) );
		signalPostItemsInserted( index, 1 );
	}

	T pop_back()
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.back().get() );
		T value = item->value_;

		auto index = items_.size() - 1;

		signalPreItemsRemoved( index, 1 );
		items_.pop_back();
		signalPostItemsRemoved( index, 1 );

		return value;
	}
	T pop_front()
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.front().get() );
		T value = item->value_;

		auto index = 0;

		signalPreItemsRemoved( index, 1 );
		items_.erase( items_.begin() );
		signalPostItemsRemoved( index, 1 );

		return value;
	}
	const T & back() const
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.back().get() );
		const T & value = item->value_;

		return value;
	}
	const T & front() const
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.front().get() );
		const T & value = item->value_;

		return value;
	}

	T & operator[](size_t index)
	{
		auto item = static_cast< GenericListItemT<T> * >( items_[index].get() );
		T & value = item->value_;

		return value;
	}
	const T & operator[](size_t index) const
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_[index].get() );
		const T & value = item->value_;

		return value;
	}

private:
	GenericListT( const GenericListT& rhs );
	GenericListT& operator=( const GenericListT& rhs );

	Items items_;

	friend class Iterator;
};
} // end namespace wgt
#endif // GENERIC_LIST_HPP
