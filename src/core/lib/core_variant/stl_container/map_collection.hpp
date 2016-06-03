#ifndef MAP_CONTAINER_COLLECTION_HPP
#define MAP_CONTAINER_COLLECTION_HPP

#include <map>
#include <unordered_map>
#include <cassert>
#include <utility>


namespace wgt
{
namespace collection_details
{

	template< typename CollectionImpl >
	class MapCollectionIteratorImpl:
		public CollectionIteratorImplBase
	{
	public:
		typedef CollectionImpl collection_impl;
		typedef typename collection_impl::container_type container_type;
		typedef typename collection_impl::key_type key_type;
		typedef typename collection_impl::value_type value_type;
		typedef MapCollectionIteratorImpl< collection_impl > this_type;

		static const bool is_const_container = std::is_const< container_type >::value;
		static const bool can_set =
			Variant::traits< value_type >::can_downcast &&
			!is_const_container &&
			!std::is_const< value_type >::value;

		typedef typename std::conditional<
			is_const_container,
			typename container_type::const_iterator,
			typename container_type::iterator
		>::type iterator_type;

		MapCollectionIteratorImpl( const collection_impl& collectionImpl, const iterator_type& iterator ):
			collectionImpl_( collectionImpl ),
			iterator_( iterator )
		{
		}

		container_type& container() const
		{
			return collectionImpl_.container_;
		}

		const iterator_type& base() const
		{
			return iterator_;
		}

		const TypeId& keyType() const override
		{
			return GetTypeImpl< key_type >::keyType( this );
		}

		const TypeId& valueType() const override
		{
			return GetTypeImpl< value_type >::valueType( this );
		}

		Variant key() const override
		{
			if( iterator_ != container().end() )
			{
				return iterator_->first;
			}
			else
			{
				return Variant();
			}
		}

		Variant value() const override
		{
			if( iterator_ != container().end() )
			{
				return iterator_->second;
			}
			else
			{
				return Variant();
			}
		}

		bool setValue( const Variant& v ) const override
		{
			return SetImpl< can_set >::setValue( this, v );
		}

		void inc() override
		{
			++iterator_;
		}

		bool equals( const CollectionIteratorImplBase& that ) const override
		{
			const this_type* t = dynamic_cast< const this_type* >( &that );
			if( !t )
			{
				return false;
			}

			return
				&collectionImpl_ == &t->collectionImpl_ &&
				iterator_ == t->iterator_;
		}

		CollectionIteratorImplPtr clone() const override
		{
			return std::make_shared< this_type >( *this );
		}

	private:
		const collection_impl& collectionImpl_;
		iterator_type iterator_;

		// SetImpl
		template< bool can_set, typename Dummy = void >
		struct SetImpl
		{
			static bool setValue( const this_type* impl, const Variant& v )
			{
				if( impl->iterator_ == impl->container().end() )
				{
					return false;
				}

				return v.with< value_type >( [ impl, &v ]( const value_type& val )
				{
					auto pos = impl->collectionImpl_.makeIterator( impl->iterator_ );
					impl->collectionImpl_.onPreChange_( pos, v );
					auto oldValue = impl->iterator_->second;
					impl->iterator_->second = val;
					impl->collectionImpl_.onPostChanged_( pos, oldValue );
				} );
			}
		};

		template< typename Dummy >
		struct SetImpl< false, Dummy >
		{
			static bool setValue( const this_type* impl, const Variant& v )
			{
				// nop
				return false;
			}
		};

		// GetTypeImpl
		template< typename T, typename Dummy = void >
		struct GetTypeImpl
		{
			static const TypeId& keyType( const this_type* )
			{
				return TypeId::getType< T >();
			}

			static const TypeId& valueType( const this_type* )
			{
				return TypeId::getType< T >();
			}
		};

		template< typename Dummy >
		struct GetTypeImpl< Variant, Dummy >
		{
			static const TypeId& keyType( const this_type* impl )
			{
				if( impl->iterator_ != impl->container().end() )
				{
					return impl->iterator_->first.type()->typeId();
				}
				else
				{
					return TypeId::getType< void >();
				}
			}

			static const TypeId& valueType( const this_type* impl )
			{
				if( impl->iterator_ != impl->container().end() )
				{
					return impl->iterator_->second.type()->typeId();
				}
				else
				{
					return TypeId::getType< void >();
				}
			}
		};

	};


	template< typename IteratorImpl, bool non_unique_keys >
	struct MapCollectionGetNew;


	template< typename IteratorImpl >
	struct MapCollectionGetNew< IteratorImpl, false >
	{
		typedef typename IteratorImpl::collection_impl collection_impl;
		typedef typename IteratorImpl::key_type key_type;
		typedef typename IteratorImpl::value_type value_type;
		typedef typename IteratorImpl::iterator_type iterator_type;

		static std::pair< iterator_type, bool > get_new(
			collection_impl& collectionImpl,
			const key_type& key )
		{
			typedef std::pair< iterator_type, bool > result_type;

			auto range = collectionImpl.container_.equal_range( key );
			if( range.first == range.second )
			{
				collectionImpl.onPreInsert_( collectionImpl.makeIterator( range.first ), 1 );
				auto r = collectionImpl.container_.emplace_hint( range.first, key, value_type() );
				collectionImpl.onPostInserted_( collectionImpl.makeIterator( r ), 1 );
				return result_type( r, true );
			}
			else
			{
				return result_type( collectionImpl.container_.end(), false );
			}
		}
	};


	template< typename IteratorImpl >
	struct MapCollectionGetNew< IteratorImpl, true >
	{
		typedef typename IteratorImpl::collection_impl collection_impl;
		typedef typename IteratorImpl::key_type key_type;
		typedef typename IteratorImpl::value_type value_type;
		typedef typename IteratorImpl::iterator_type iterator_type;

		static std::pair< iterator_type, bool > get_new(
			collection_impl& collectionImpl,
			const key_type& key )
		{
			typedef std::pair< iterator_type, bool > result_type;

			auto range = collectionImpl.container_.equal_range( key );
			collectionImpl.onPreInsert_( collectionImpl.makeIterator( range.first ), 1 );
			auto r = collectionImpl.container_.emplace_hint( range.first, key, value_type() );
			collectionImpl.onPostInserted_( collectionImpl.makeIterator( r ), 1 );
			return result_type( r, true );
		}
	};


	template< typename Container, bool resizable, bool ordered, bool non_unique_keys >
	class MapCollectionImpl;


	template< typename Container, bool ordered, bool non_unique_keys >
	class MapCollectionImpl< Container, true, ordered, non_unique_keys >:
		public CollectionImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::key_type key_type;
		typedef typename container_type::mapped_type value_type;
		typedef MapCollectionImpl< container_type, true, ordered, non_unique_keys > this_type;
		typedef MapCollectionIteratorImpl< this_type > iterator_impl_type;
		typedef MapCollectionGetNew< iterator_impl_type, non_unique_keys > map_collection_get_new;

		friend class MapCollectionIteratorImpl< this_type >;
		friend struct MapCollectionGetNew< iterator_impl_type, non_unique_keys >;

		static const int flags_value =
			MAPPING |
			( iterator_impl_type::can_set ? WRITABLE : 0 ) |
			RESIZABLE |
			( ordered ? ORDERED : 0) |
			( non_unique_keys ? NON_UNIQUE_KEYS : 0 );

		template< bool can_set, typename Dummy = void >
		struct downcaster_impl
		{
			static bool downcast( container_type* v, CollectionImplBase& storage )
			{
				if( auto storageContainer = storage.container() )
				{
					if( v && storageContainer == v )
					{
						// don't copy to itself
						return true;
					}

					if( storage.containerType() == TypeId::getType< container_type >() )
					{
						// short circuit
						auto container = static_cast< const container_type* >( storageContainer );
						if( v )
						{
							*v = *container;
						}
						return true;
					}
				}

				if( v )
				{
					v->clear();
					for( auto it = storage.begin(), end = storage.end(); !it->equals( *end ); it->inc() )
					{
						key_type key;
						if( !it->key().tryCast( key ) )
						{
							return false;
						}

						value_type val;
						if( !it->value().tryCast( val ) )
						{
							return false;
						}

						v->emplace( std::move( key ), std::move( val ) );
					}
				}

				return true;
			}
		};

		template< typename Dummy >
		struct downcaster_impl< false, Dummy >
		{
		};

		typedef typename std::conditional<
			iterator_impl_type::can_set,
			downcaster_impl< iterator_impl_type::can_set >,
			void >::type downcaster;

		explicit MapCollectionImpl( container_type& container ):
			container_( container )
		{
		}

		const TypeId& keyType() const override
		{
			return TypeId::getType< key_type >();
		}

		const TypeId& valueType() const override
		{
			return TypeId::getType< value_type >();
		}

		const TypeId& containerType() const override
		{
			return TypeId::getType< container_type >();
		}

		const void* container() const override
		{
			return &container_;
		}

		size_t size() const override
		{
			return container_.size();
		}

		CollectionIteratorImplPtr begin() override
		{
			return makeIterator( container_.begin() );
		}

		CollectionIteratorImplPtr end() override
		{
			return makeIterator( container_.end() );
		}

		std::pair< CollectionIteratorImplPtr, bool > get( const Variant& key, GetPolicy policy ) override
		{
			typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

			key_type k;
			if( !key.tryCast( k ) )
			{
				return result_type( end(), false );
			}

			switch( policy )
			{
			case GET_EXISTING:
				{
					auto range = container_.equal_range( k );
					if( range.first != range.second )
					{
						return result_type( makeIterator( range.first ), false );
					}
					else
					{
						return result_type( makeIterator( container_.end() ), false );
					}
				}

			case GET_NEW:
				{
					// insert a new one
					auto r = map_collection_get_new::get_new( *this, k );
					return result_type( makeIterator( r.first ), r.second );
				}

			case GET_AUTO:
				{
					auto range = container_.equal_range( k );
					if( range.first != range.second )
					{
						// key exists
						return result_type( makeIterator( range.first ), false );
					}

					// insert a new one
					onPreInsert_( makeIterator( range.first ), 1 );
					auto r = container_.emplace_hint( range.first, k, value_type() );
					auto pos = makeIterator( r );
					onPostInserted_( pos, 1 );
					return result_type( pos, true );
				}

			default:
				return result_type( end(), false );

			}
		}

		CollectionIteratorImplPtr erase( const CollectionIteratorImplPtr& pos ) override
		{
			iterator_impl_type* ii = dynamic_cast< iterator_impl_type* >( pos.get() );
			assert( ii );
			assert( &ii->container() == &container_ );

			onPreErase_( pos, 1 );
			auto r = container_.erase( ii->base() );
			onPostErased_( makeIterator( r ), 1);
			return makeIterator( r );
		}

		size_t erase( const Variant& key ) override
		{
			key_type k;
			if( !key.tryCast( k ) )
			{
				return 0;
			}

			auto range = container_.equal_range( k );
			size_t count = std::distance( range.first, range.second );
			if( count > 0 )
			{
				onPreErase_( makeIterator( range.first ), count );
				auto r = container_.erase(range.first, range.second);
				onPostErased_( makeIterator( r ), count);
			}

			return count;
		}

		CollectionIteratorImplPtr erase(
			const CollectionIteratorImplPtr& first,
			const CollectionIteratorImplPtr& last ) override
		{
			iterator_impl_type* ii_first = dynamic_cast< iterator_impl_type* >( first.get() );
			iterator_impl_type* ii_last = dynamic_cast< iterator_impl_type* >( last.get() );
			assert( ii_first && ii_last );
			assert( &ii_first->container() == &container_ && &ii_last->container() == &container_ );

			size_t count = std::distance( ii_first->base(), ii_last->base() );
			if( count > 0 )
			{
				onPreErase_( first, count );
				auto r = container_.erase( ii_first->base(), ii_last->base() );
				onPostErased_( makeIterator( r ), count);
				return makeIterator( r );
			}
			else
			{
				return makeIterator( container_.end() );
			}
		}

		int flags() const override
		{
			return flags_value;
		}

		Connection connectPreInsert( ElementRangeCallback callback ) override
		{
			return onPreInsert_.connect( callback );
		}

		Connection connectPostInserted( ElementRangeCallback callback ) override
		{
			return onPostInserted_.connect( callback );
		}

		Connection connectPreErase( ElementRangeCallback callback ) override
		{
			return onPreErase_.connect( callback );
		}

		Connection connectPostErased(ElementRangeCallback callback) override
		{
			return onPostErased_.connect( callback );
		}

		Connection connectPreChange( ElementPreChangeCallback callback ) override
		{
			return onPreChange_.connect( callback );
		}

		Connection connectPostChanged( ElementPostChangedCallback callback ) override
		{
			return onPostChanged_.connect( callback );
		}

	private:
		container_type& container_;
		Signal< ElementRangeCallbackSignature > onPreInsert_;
		Signal< ElementRangeCallbackSignature > onPostInserted_;
		Signal< ElementRangeCallbackSignature > onPreErase_;
		Signal< ElementRangeCallbackSignature > onPostErased_;
		Signal< ElementPreChangeCallbackSignature > onPreChange_;
		Signal< ElementPostChangedCallbackSignature > onPostChanged_;

		CollectionIteratorImplPtr makeIterator( typename iterator_impl_type::iterator_type iterator ) const
		{
			return std::make_shared< iterator_impl_type >( *this, iterator );
		}

	};


	template< typename Container, bool ordered, bool non_unique_keys >
	class MapCollectionImpl< Container, false, ordered, non_unique_keys >:
		public CollectionImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::key_type key_type;
		typedef typename container_type::mapped_type value_type;
		typedef MapCollectionImpl< container_type, false, ordered, non_unique_keys > this_type;
		typedef MapCollectionIteratorImpl< this_type > iterator_impl_type;

		friend class MapCollectionIteratorImpl< this_type >;

		static const int flags_value =
			MAPPING |
			( iterator_impl_type::can_set ? WRITABLE : 0 ) |
			( ordered ? ORDERED : 0) |
			( non_unique_keys ? NON_UNIQUE_KEYS : 0 );

		typedef void downcaster;

		explicit MapCollectionImpl( container_type& container ):
			container_( container )
		{
		}

		const TypeId& keyType() const override
		{
			return TypeId::getType< key_type >();
		}

		const TypeId& valueType() const override
		{
			return TypeId::getType< value_type >();
		}

		const TypeId& containerType() const override
		{
			return TypeId::getType< container_type >();
		}

		const void* container() const override
		{
			return &container_;
		}

		size_t size() const override
		{
			return container_.size();
		}

		CollectionIteratorImplPtr begin() override
		{
			return makeIterator( container_.begin() );
		}

		CollectionIteratorImplPtr end() override
		{
			return makeIterator( container_.end() );
		}

		std::pair< CollectionIteratorImplPtr, bool > get( const Variant& key, GetPolicy policy ) override
		{
			typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

			key_type k;
			if( !key.tryCast( k ) )
			{
				return result_type( end(), false );
			}

			switch( policy )
			{
			case GET_EXISTING:
			case GET_AUTO:
				return result_type( makeIterator( container_.find( k ) ), false );

			case GET_NEW:
			default:
				return result_type( end(), false );

			}
		}

		CollectionIteratorImplPtr erase( const CollectionIteratorImplPtr& pos ) override
		{
			return end();
		}

		size_t erase( const Variant& key ) override
		{
			return 0;
		}

		CollectionIteratorImplPtr erase(
			const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last ) override
		{
			return end();
		}

		int flags() const override
		{
			return flags_value;
		}

		Connection connectPreChange( ElementPreChangeCallback callback ) override
		{
			return onPreChange_.connect( callback );
		}

		Connection connectPostChanged( ElementPostChangedCallback callback ) override
		{
			return onPostChanged_.connect( callback );
		}

	private:
		container_type& container_;
		Signal< ElementPreChangeCallbackSignature > onPreChange_;
		Signal< ElementPostChangedCallbackSignature > onPostChanged_;

		CollectionIteratorImplPtr makeIterator( typename iterator_impl_type::iterator_type iterator ) const
		{
			return std::make_shared< iterator_impl_type >( *this, iterator );
		}

	};


	// std::map

	template< typename Key, typename T, typename Compare, typename Alloc >
	MapCollectionImpl< std::map< Key, T, Compare, Alloc >, true, true, false > deduceCollectionImplType( std::map< Key, T, Compare, Alloc >& );

	template< typename Key, typename T, typename Compare, typename Alloc >
	MapCollectionImpl< const std::map< Key, T, Compare, Alloc >, false, true, false > deduceCollectionImplType( const std::map< Key, T, Compare, Alloc >& );

	// std::unordered_map

	template< typename Key, typename T, typename Hash, typename Pred, typename Alloc >
	MapCollectionImpl< std::unordered_map< Key, T, Hash, Pred, Alloc >, true, false, false > deduceCollectionImplType( std::unordered_map< Key, T, Hash, Pred, Alloc >& );

	template< typename Key, typename T, typename Hash, typename Pred, typename Alloc >
	MapCollectionImpl< const std::unordered_map< Key, T, Hash, Pred, Alloc >, false, false, false > deduceCollectionImplType( const std::unordered_map< Key, T, Hash, Pred, Alloc >& );

	// std::multimap

	template< typename Key, typename T, typename Compare, typename Alloc >
	MapCollectionImpl< std::multimap< Key, T, Compare, Alloc >, true, true, true > deduceCollectionImplType( std::multimap< Key, T, Compare, Alloc >& );

	template< typename Key, typename T, typename Compare, typename Alloc >
	MapCollectionImpl< const std::multimap< Key, T, Compare, Alloc >, false, true, true > deduceCollectionImplType( const std::multimap< Key, T, Compare, Alloc >& );

	// std::unordered_multimap

	template< typename Key, typename T, typename Hash, typename Pred, typename Alloc >
	MapCollectionImpl< std::unordered_multimap< Key, T, Hash, Pred, Alloc >, true, false, true > deduceCollectionImplType( std::unordered_multimap< Key, T, Hash, Pred, Alloc >& );

	template< typename Key, typename T, typename Hash, typename Pred, typename Alloc >
	MapCollectionImpl< const std::unordered_multimap< Key, T, Hash, Pred, Alloc >, false, false, true > deduceCollectionImplType( const std::unordered_multimap< Key, T, Hash, Pred, Alloc >& );

}
} // end namespace wgt

#endif
