#include "collection.hpp"
#include <cassert>


namespace wgt
{
CollectionIteratorImplPtr CollectionImplBase::insert(const Variant& key)
{
	return this->get(key, GetPolicy::GET_NEW).first;
}

Collection::ConstIterator& Collection::ConstIterator::operator++()
{
	detach();
	impl_->inc();
	return *this;
}


bool Collection::ConstIterator::operator==(const Collection::ConstIterator& that) const
{
	if(impl_ && that.impl_)
	{
		return impl_->equals(*that.impl_);
	}
	else
	{
		return impl_.get() == that.impl_.get();
	}
}


void Collection::ConstIterator::detach()
{
	if(impl_ && !impl_.unique())
	{
		impl_ = impl_->clone();
	}
}


bool Collection::isValid() const
{
	return impl_.get() != nullptr;
}


const TypeId& Collection::keyType() const
{
	return impl_->keyType();
}


const TypeId& Collection::valueType() const
{
	return impl_->valueType();
}


bool Collection::isSame( const void* container ) const
{
	return
		impl_ &&
		container && // never match nullptr
		impl_->container() == container;
}


bool Collection::empty() const
{
	if (impl_)
	{
		return impl_->size() == 0;
	}
	else
	{
		return true;
	}
}


size_t Collection::size() const
{
	if(impl_)
	{
		return impl_->size();
	}
	else
	{
		return 0;
	}
}


Collection::Iterator Collection::begin()
{
	if(impl_)
	{
		return impl_->begin();
	}
	else
	{
		return Iterator();
	}
}


Collection::ConstIterator Collection::begin() const
{
	if(impl_)
	{
		return impl_->begin();
	}
	else
	{
		return Iterator();
	}
}


Collection::Iterator Collection::end()
{
	if(impl_)
	{
		return impl_->end();
	}
	else
	{
		return Iterator();
	}
}


Collection::ConstIterator Collection::end() const
{
	if(impl_)
	{
		return impl_->end();
	}
	else
	{
		return Iterator();
	}
}


Collection::Iterator Collection::find(const Variant& key)
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_EXISTING).first;
	}
	else
	{
		return Iterator();
	}
}


Collection::ConstIterator Collection::find(const Variant& key) const
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_EXISTING).first;
	}
	else
	{
		return ConstIterator();
	}
}


Collection::Iterator Collection::insert(const Variant& key)
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_NEW).first;
	}
	else
	{
		return Iterator();
	}
}

Collection::Iterator Collection::insertValue(const Variant& key, const Variant& value)
{
	if (impl_)
	{
		return impl_->insert(key, value);
	}
	else
	{
		return Iterator();
	}
}

std::pair<Collection::Iterator, bool> Collection::get(const Variant& key)
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_AUTO);
	}
	else
	{
		return std::pair<Iterator, bool>(Iterator(), false);
	}
}


Collection::Iterator Collection::erase(const Iterator& pos)
{
	if(impl_)
	{
		return impl_->erase(pos.impl());
	}
	else
	{
		return Iterator();
	}
}


size_t Collection::eraseKey(const Variant& key)
{
	if(impl_)
	{
		return impl_->eraseKey(key);
	}
	else
	{
		return 0;
	}
}


Collection::Iterator Collection::erase(const Iterator& first, const Iterator& last)
{
	if(impl_)
	{
		return impl_->erase(first.impl(), last.impl());
	}
	else
	{
		return Iterator();
	}
}


Collection::ValueRef Collection::operator[](const Variant& key)
{
	assert(impl_);

	return impl_->get(key, CollectionImplBase::GET_AUTO).first;
}


const Variant Collection::operator[](const Variant& key) const
{
	assert(impl_);

	return impl_->get(key, CollectionImplBase::GET_EXISTING).first->value();
}


bool Collection::operator==(const Collection& that) const
{
	if (impl_ == that.impl_)
	{
		return true;
	}

	if (impl_ == nullptr || that.impl_ == nullptr)
	{
		return false;
	}

	auto size = that.impl_->size();
	if (impl_->size() != size)
	{
		return false;
	}

	auto thisIt = impl_->begin();
	auto thatIt = that.impl_->begin();
	for (size_t i = 0; i < size; ++i)
	{
		if (thisIt->key() != thatIt->key() ||
			thisIt->value() != thatIt->value())
		{
			return false;
		}

		thisIt->inc();
		thatIt->inc();
	}

	return true;
}


int Collection::flags() const
{
	if( impl_ )
	{
		return impl_->flags();
	}
	else
	{
		return 0;
	}
}


Connection Collection::connectPreInsert( ElementRangeCallback callback )
{
	if( impl_ )
	{
		return impl_->connectPreInsert( callback );
	}
	else
	{
		return Connection();
	}
}


Connection Collection::connectPostInserted( ElementRangeCallback callback )
{
	if( impl_ )
	{
		return impl_->connectPostInserted( callback );
	}
	else
	{
		return Connection();
	}
}


Connection Collection::connectPreErase( ElementRangeCallback callback )
{
	if( impl_ )
	{
		return impl_->connectPreErase( callback );
	}
	else
	{
		return Connection();
	}
}


Connection Collection::connectPostErased( ElementRangeCallback callback)
{
	if( impl_ )
	{
		return impl_->connectPostErased( callback );
	}
	else
	{
		return Connection();
	}
}


Connection Collection::connectPreChange( ElementPreChangeCallback callback )
{
	if( impl_ )
	{
		return impl_->connectPreChange( callback );
	}
	else
	{
		return Connection();
	}
}


Connection Collection::connectPostChanged( ElementPostChangedCallback callback )
{
	if( impl_ )
	{
		return impl_->connectPostChanged( callback );
	}
	else
	{
		return Connection();
	}
}
} // end namespace wgt
