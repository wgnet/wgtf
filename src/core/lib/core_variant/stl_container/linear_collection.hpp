#ifndef LINEAR_CONTAINER_COLLECTION_HPP
#define LINEAR_CONTAINER_COLLECTION_HPP

#include <vector>
#include <deque>
#include <array>
#include <cassert>
#include <utility>


namespace wgt
{
namespace collection_details
{

	template< typename CollectionImpl >
	class LinearCollectionIteratorImpl:
		public CollectionIteratorImplBase
	{
	public:
		typedef CollectionImpl collection_impl;
		typedef typename collection_impl::container_type container_type;
		typedef typename collection_impl::key_type key_type;
		typedef typename collection_impl::value_type value_type;
		typedef LinearCollectionIteratorImpl< collection_impl > this_type;

		static const bool can_set =
			Variant::traits< value_type >::can_cast &&
			!std::is_const< container_type >::value &&
			!std::is_const< value_type >::value;

		LinearCollectionIteratorImpl( const collection_impl& collectionImpl, key_type index ):
			collectionImpl_( collectionImpl ),
			index_( index )
		{
		}

		container_type& container() const
		{
			return collectionImpl_.container_;
		}

		key_type index() const
		{
			return index_;
		}

		const TypeId& keyType() const override
		{
			return TypeId::getType< key_type >();
		}

		const TypeId& valueType() const override
		{
			return GetTypeImpl< value_type >::valueType( this );
		}

		Variant key() const override
		{
			return index_;
		}

		Variant value() const override
		{
			if( index_ < container().size() )
			{
				return GetImpl< value_type >::value( this );
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
			index_ += 1;
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
				index_ == t->index_;
		}

		CollectionIteratorImplPtr clone() const override
		{
			return std::make_shared< this_type >( *this );
		}

	private:
		const collection_impl& collectionImpl_;
		key_type index_;

		// SetImpl
		template< bool can_set, typename Dummy = void >
		struct SetImpl
		{
			static bool setValue( const this_type* impl, const Variant& v )
			{
				if( impl->index_ >= impl->container().size() )
				{
					return false;
				}

				return v.visit< value_type >( [ impl, &v ]( const value_type& val )
				{
					auto pos = impl->collectionImpl_.makeIterator( impl->index_ );
					impl->collectionImpl_.onPreChange_( pos, v );
					auto&& ref = impl->container()[ impl->index_ ];
					auto oldValue = ref;
					ref = val;
					impl->collectionImpl_.onPostChanged_( pos, oldValue );
				} );
			}
		};

		template< typename Dummy >
		struct SetImpl< false, Dummy >
		{
			static bool setValue( const this_type*, const Variant& )
			{
				// nop
				return false;
			}
		};

		// GetImpl
		template< typename T, typename Dummy = void >
		struct GetImpl
		{
			static Variant value( const this_type* impl )
			{
				return impl->container()[ impl->index_ ];
			}
		};

		template< typename Dummy >
		struct GetImpl< bool, Dummy >
		{
			static Variant value( const this_type* impl )
			{
				return ( bool )impl->container()[ impl->index_ ];
			}
		};

		// GetTypeImpl
		template< typename T, typename Dummy = void >
		struct GetTypeImpl
		{
			static const TypeId& valueType( const this_type* )
			{
				return TypeId::getType< T >();
			}
		};

		template< typename Dummy >
		struct GetTypeImpl< Variant, Dummy >
		{
			static const TypeId& valueType( const this_type* impl )
			{
				if( impl->index_ < impl->container().size() )
				{
					return impl->container()[ impl->index_ ].type()->typeId();
				}
				else
				{
					return TypeId::getType< void >();
				}
			}
		};

	};


	template< typename Container >
	struct linear_collection_container_traits
	{
		typedef Container container_type;

		static void insertDefaultAt( container_type& container, const typename container_type::iterator& pos )
		{
			container.emplace( pos );
		}
	};

	template< typename Allocator >
	struct linear_collection_container_traits< std::vector< bool, Allocator > >
	{
		typedef std::vector< bool, Allocator > container_type;

		static void insertDefaultAt( container_type& container, const typename container_type::iterator& pos )
		{
			// std::vector< bool > doesn't have emplace
			container.insert( pos, false );
		}
	};


	template< typename Container, bool can_resize >
	class LinearCollectionImpl;


	template< typename Container >
	class LinearCollectionImpl< Container, true >:
		public CollectionImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::size_type key_type;
		typedef typename container_type::value_type value_type;
		typedef LinearCollectionImpl< container_type, true > this_type;
		typedef LinearCollectionIteratorImpl< this_type > iterator_impl_type;

		friend class LinearCollectionIteratorImpl< this_type >;

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
					v->reserve( storage.size() );
					for( auto it = storage.begin(), end = storage.end(); !it->equals( *end ); it->inc() )
					{
						key_type i;
						if( !it->key().tryCast( i ) )
						{
							return false;
						}

						value_type val;
						if( !it->value().tryCast( val ) )
						{
							return false;
						}

						if( i >= v->size() )
						{
							// WARNING! Can potentially cause OOM issues.
							v->resize( i + 1 );
						}

						( *v )[ i ] = std::move( val );
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

		explicit LinearCollectionImpl( container_type& container ):
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
			return makeIterator( 0 );
		}

		CollectionIteratorImplPtr end() override
		{
			return makeIterator( container_.size() );
		}

		std::pair< CollectionIteratorImplPtr, bool > get( const Variant& key, GetPolicy policy ) override
		{
			typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

			key_type i;
			if( !key.tryCast( i ) )
			{
				return result_type( end(), false );
			}

			switch( policy )
			{
			case GET_EXISTING:
				if( i < container_.size() )
				{
					return result_type( makeIterator( i ), false );
				}
				else
				{
					return result_type( end(), false );
				}

			case GET_NEW:
				if( i >= container_.size() )
				{
					auto pos = makeIterator( container_.size() );
					auto count = i + 1 - container_.size();
					onPreInsert_( pos, count );

					container_.resize( i + 1 );

					onPostInserted_( pos, count );
				}
				else
				{
					auto pos = makeIterator( i );
					onPreInsert_( pos, 1 );
					linear_collection_container_traits< container_type >::insertDefaultAt( container_, container_.begin() + i );
					onPostInserted_( pos, 1 );
				}

				return result_type( makeIterator( i ), true );

			case GET_AUTO:
				{
					bool found = i < container_.size();
					if( !found )
					{
						auto pos = makeIterator( container_.size() );
						auto count = i + 1 - container_.size();
						onPreInsert_( pos, count );

						container_.resize( i + 1 );

						onPostInserted_( pos, count );
					}

					return result_type( makeIterator( i ), !found );
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
			assert( ii->index() < container_.size() );
						
			onPreErase_( pos, 1 );

			auto r = container_.erase( container_.begin() + ii->index() );

			onPostErased_(makeIterator(r - container_.begin()), 1);

			return makeIterator( r - container_.begin() );
		}

		size_t eraseKey( const Variant& key ) override
		{
			key_type i;
			if( !key.tryCast( i ) )
			{
				return 0;
			}

			if( i >= container_.size() )
			{
				return 0;
			}

			onPreErase_( makeIterator( i ), 1 );

			auto r = container_.erase(container_.begin() + i);

			onPostErased_(makeIterator(r - container_.begin()), 1);

			return 1;
		}

		CollectionIteratorImplPtr erase(
			const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last ) override
		{
			iterator_impl_type* ii_first = dynamic_cast< iterator_impl_type* >( first.get() );
			iterator_impl_type* ii_last = dynamic_cast< iterator_impl_type* >( last.get() );
			assert( ii_first );
			assert( ii_last );
			assert( &ii_first->container() == &container_ );
			assert( &ii_last->container() == &container_ );
			assert( ii_last->index() <= container_.size() );
			assert( ii_first->index() <= ii_last->index() );
			
			auto count = ii_last->index() - ii_first->index();
			onPreErase_( first, count );

			auto r = container_.erase(
				container_.begin() + ii_first->index(), container_.begin() + ii_last->index() );

			onPostErased_(makeIterator(r - container_.begin()), count);

			return makeIterator( r - container_.begin() );
		}

		int flags() const override
		{
			return
				( iterator_impl_type::can_set ? WRITABLE : 0 ) |
				RESIZABLE |
				ORDERED;
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

		CollectionIteratorImplPtr makeIterator( key_type index ) const
		{
			return std::make_shared< iterator_impl_type >( *this, index );
		}
	};


	template< typename Container >
	class LinearCollectionImpl< Container, false >:
		public CollectionImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::size_type key_type;
		typedef typename container_type::value_type value_type;
		typedef LinearCollectionImpl< container_type, false > this_type;
		typedef LinearCollectionIteratorImpl< this_type > iterator_impl_type;

		friend class LinearCollectionIteratorImpl< this_type >;

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
					v->fill( value_type() );
					for( auto it = storage.begin(), end = storage.end(); !it->equals( *end ); it->inc() )
					{
						key_type i;
						if( !it->key().tryCast( i ) )
						{
							return false;
						}

						if( i >= v->size() )
						{
							return false;
						}

						value_type val;
						if( !it->value().tryCast( val ) )
						{
							return false;
						}

						( *v )[ i ] = std::move( val );
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

		explicit LinearCollectionImpl( container_type& container ):
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
			return makeIterator( 0 );
		}

		CollectionIteratorImplPtr end() override
		{
			return makeIterator( container_.size() );
		}

		std::pair< CollectionIteratorImplPtr, bool > get( const Variant& key, GetPolicy policy ) override
		{
			typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

			key_type i;
			if( !key.tryCast( i ) )
			{
				return result_type( end(), false );
			}

			switch( policy )
			{
			case GET_EXISTING:
			case GET_AUTO:
				if( i < container_.size() )
				{
					return result_type( makeIterator( i ), false );
				}
				else
				{
					return result_type( end(), false );
				}

			case GET_NEW:
			default:
				return result_type( end(), false );

			}
		}

		CollectionIteratorImplPtr erase( const CollectionIteratorImplPtr& pos ) override
		{
			return end();
		}

		size_t eraseKey( const Variant& key ) override
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
			return
				( iterator_impl_type::can_set ? WRITABLE : 0 ) |
				ORDERED;
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

		CollectionIteratorImplPtr makeIterator( key_type index ) const
		{
			return std::make_shared< iterator_impl_type >( *this, index );
		}

	};


	// std::vector

	template< typename T, typename Alloc >
	LinearCollectionImpl< std::vector< T, Alloc >, true > deduceCollectionImplType( std::vector< T, Alloc >& );

	template< typename T, typename Alloc >
	LinearCollectionImpl< const std::vector< T, Alloc >, false > deduceCollectionImplType( const std::vector< T, Alloc >& );

	// std::deque

	template< typename T, typename Alloc >
	LinearCollectionImpl< std::deque< T, Alloc >, true > deduceCollectionImplType( std::deque< T, Alloc >& );

	template< typename T, typename Alloc >
	LinearCollectionImpl< const std::deque< T, Alloc >, false > deduceCollectionImplType( const std::deque< T, Alloc >& );

	// std::array

	template< typename T, size_t N >
	LinearCollectionImpl< std::array< T, N >, false > deduceCollectionImplType( std::array< T, N >& );

	template< typename T, size_t N >
	LinearCollectionImpl< const std::array< T, N >, false > deduceCollectionImplType( const std::array< T, N >& );

}
} // end namespace wgt
#endif
