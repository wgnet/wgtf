#include "reflected_collection.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"

namespace wgt
{
namespace ReflectedCollectionDetails
{
	class ReflectedCollectionListener : public PropertyAccessorListener
	{
	public:
		ReflectedCollectionListener( ReflectedCollection & reflectedCollection )
			: reflectedCollection_( reflectedCollection )
		{
			object_ = reflectedCollection_.pa_.getObject();
			path_ = reflectedCollection_.pa_.getFullPath();
		}

		void preSetValue( const PropertyAccessor & accessor, const Variant& value ) override 
		{
			if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
			{
				return;
			}
		}
		
		void postSetValue( const PropertyAccessor & accessor, const Variant& value ) override 
		{
			if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
			{
				return;
			}
		}

		void preInsert( const PropertyAccessor & accessor, size_t index, size_t count ) override
		{
			if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
			{
				return;
			}

			auto it = reflectedCollection_.begin();
			for (size_t i = 0; i < index; ++i)
			{
				it->inc();
			}
			reflectedCollection_.onPreInsert_( it, count );
		}

		void postInserted( const PropertyAccessor & accessor, size_t index, size_t count ) override 
		{
			if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
			{
				return;
			}

			reflectedCollection_.reset();
			auto it = reflectedCollection_.begin();
			for (size_t i = 0; i < index; ++i)
			{
				it->inc();
			}
			reflectedCollection_.onPostInserted_( it, count );
		}

		void preErase( const PropertyAccessor & accessor, size_t index, size_t count ) override 
		{
			if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
			{
				return;
			}

			auto it = reflectedCollection_.begin();
			for (size_t i = 0; i < index; ++i)
			{
				it->inc();
			}
			reflectedCollection_.onPreErase_( it, count );
		}

		void postErased( const PropertyAccessor & accessor, size_t index, size_t count ) override 
		{
			if (object_ != accessor.getObject() || path_ != accessor.getFullPath())
			{
				return;
			}

			reflectedCollection_.reset();
			auto it = reflectedCollection_.begin();
			for (size_t i = 0; i < index; ++i)
			{
				it->inc();
			}
			reflectedCollection_.onPostErased_( it, count );
		}

	private:
		ReflectedCollection & reflectedCollection_;
		ObjectHandle object_;
		std::string path_;
	};
}

//TODO custom iterator

ReflectedCollection::ReflectedCollection( const PropertyAccessor & pa, IReflectionController * controller )
	: pa_( pa )
	, listener_( new ReflectedCollectionDetails::ReflectedCollectionListener( *this ) )
    , controller_( controller )
{
	auto definitionManager = const_cast< IDefinitionManager * >( pa_.getDefinitionManager() );
	definitionManager->registerPropertyAccessorListener( listener_ );

	reset();
}

ReflectedCollection::~ReflectedCollection()
{
	auto definitionManager = const_cast< IDefinitionManager * >( pa_.getDefinitionManager() );
	definitionManager->deregisterPropertyAccessorListener( listener_ );
}

void ReflectedCollection::reset()
{
	auto value = pa_.getValue();
	collection_ = value.cast< Collection >();
}

size_t ReflectedCollection::size() const
{
	return collection_.size();
}

CollectionIteratorImplPtr ReflectedCollection::begin()
{
	return collection_.begin().impl();
}

CollectionIteratorImplPtr ReflectedCollection::end()
{
	return collection_.end().impl();
}

std::pair< CollectionIteratorImplPtr, bool > ReflectedCollection::get(const Variant& key, GetPolicy policy)
{
	typedef std::pair< CollectionIteratorImplPtr, bool > result_type;

	auto it = collection_.find( key );
	if (policy == GetPolicy::GET_EXISTING)
	{
		return result_type( it.impl(), false );
	}

	if (it != collection_.end())
	{
		if (policy == GetPolicy::GET_AUTO)
		{
			return result_type( it.impl(), false );
		}
	}

	assert(controller_ != nullptr);
	controller_->insert( pa_, key, Variant() );
	it = collection_.find( key );
	return it != end() ? result_type( it.impl(), true ) : result_type( end(), false );
}

CollectionIteratorImplPtr ReflectedCollection::insert(const Variant& key,
                                                      const Variant& value) /* override */
{
	assert(controller_ != nullptr);
	controller_->insert(pa_, key, value);
	auto it = collection_.find(key);
	return it != end() ? it.impl() : end();
}

CollectionIteratorImplPtr ReflectedCollection::erase(const CollectionIteratorImplPtr& pos)
{
	assert( false );
	return end();
}

size_t ReflectedCollection::eraseKey(const Variant& key)
{
	assert (controller_ != nullptr);
	auto count = collection_.size();
	controller_->erase( pa_, key );
	count -= collection_.size();
	return count;
}

CollectionIteratorImplPtr ReflectedCollection::erase(const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last)
{
	assert( false );
	return end();
}

const TypeId& ReflectedCollection::keyType() const
{
	return collection_.keyType();
}

const TypeId& ReflectedCollection::valueType() const
{
	return collection_.valueType();
}

const TypeId& ReflectedCollection::containerType() const
{
	return collection_.impl()->containerType();
}

const void* ReflectedCollection::container() const
{
	return collection_.impl()->container();
}

int ReflectedCollection::flags() const
{
	return collection_.flags();
}
} // end namespace wgt
