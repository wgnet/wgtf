#include "collection.hpp"

#include "core_common/assert.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_reflection/interfaces/i_managed_object.hpp"

namespace wgt
{
CollectionIteratorImplPtr CollectionImplBase::insert(const Variant& key)
{
	return this->get(key, GetPolicy::GET_NEW).first;
}

std::string CollectionIteratorImplBase::indexer() const
{
	// FIXME NGT-1603: Change to actually get the proper key type

	// Attempt to use an index into the collection
	// Defaults to i
	size_t indexKey = 0;
	const bool isIndex = key().tryCast(indexKey);

	// Default to using an index
	std::string propertyName = Collection::getIndexOpen() + std::to_string(static_cast<int>(indexKey)) + Collection::getIndexClose();

	// If the item isn't an index
	if (!isIndex)
	{
		// Try to cast the key to a string
		std::string displayName;
		const bool isString = key().tryCast(displayName);
		if (isString)
		{
			propertyName = Collection::getIndexOpenStr() + displayName + Collection::getIndexCloseStr();
		}
	}
	return propertyName;
}


//------------------------------------------------------------------------------
Connection CollectionImplBase::connectPreInsert(ElementRangeCallback callback)
{
	return Connection();
}


//------------------------------------------------------------------------------
Connection CollectionImplBase::connectPostInserted(ElementRangeCallback callback)
{
	return Connection();
}


//------------------------------------------------------------------------------
Connection CollectionImplBase::connectPreErase(ElementRangeCallback callback)
{
	return Connection();
}


//------------------------------------------------------------------------------
Connection CollectionImplBase::connectPostErased(ElementRangeCallback callback)
{
	return Connection();
}


//------------------------------------------------------------------------------
Connection CollectionImplBase::connectPreChange(ElementPreChangeCallback callback)
{
	return Connection();
}


//------------------------------------------------------------------------------
Connection CollectionImplBase::connectPostChanged(ElementPostChangedCallback callback)
{
	return Connection();
}



//------------------------------------------------------------------------------
Collection::ConstIterator& Collection::ConstIterator::operator++()
{
	detach();
	impl_->inc(1);
	return *this;
}


//------------------------------------------------------------------------------
Collection::ConstIterator Collection::ConstIterator::operator++(int)
{
	ConstIterator tmp = *this;
	++(*this);
	return tmp;
}


//------------------------------------------------------------------------------
Collection::ConstIterator& Collection::ConstIterator::operator+=( size_t advAmount )
{
	if(advAmount == 0)
	{
		return *this;
	}
	TF_ASSERT(advAmount > 0);
	detach();
	impl_->inc(advAmount);
	return *this;
}


//------------------------------------------------------------------------------
bool Collection::ConstIterator::operator==(const Collection::ConstIterator& that) const
{
	if (impl_ && that.impl_)
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
	if (impl_ && !impl_.unique())
	{
		impl_ = impl_->clone();
	}
}

//==============================================================================
const char Collection::getIndexOpen() 
{
	static const char INDEX_OPEN = '[';
	return INDEX_OPEN;
}

//------------------------------------------------------------------------------
const char Collection::getIndexClose()
{
	static const char INDEX_CLOSE = ']';
	return INDEX_CLOSE;
}


//------------------------------------------------------------------------------
// Strings must be quoted to work with TextStream
//------------------------------------------------------------------------------
const char * Collection::getIndexOpenStr()
{
	static constexpr const char* INDEX_STR_OPEN = "[\"";
	return INDEX_STR_OPEN;
}


//------------------------------------------------------------------------------
const char * Collection::getIndexCloseStr()
{
	static constexpr const char* INDEX_STR_CLOSE = "\"]";
	return INDEX_STR_CLOSE;
}


//------------------------------------------------------------------------------
Variant Collection::parseKey(const MetaType* keyType, const char*& propOperator)
{
	Variant key(keyType);
	propOperator += 1; // skip INDEX_OPEN

	FixedMemoryStream dataStream(propOperator);
	TextStream stream(dataStream);

	stream >> key >> match(getIndexClose());

	if (stream.fail())
	{
		return Variant();
	}

	// skip key and closing bracket
	propOperator += stream.seek(0, std::ios_base::cur);
	return key;
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

void Collection::clear()
{
	erase(begin(), end());
}


bool Collection::isSame(const void* container) const
{
	return impl_ && container && // never match nullptr
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	if (impl_)
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
	TF_ASSERT(impl_);

	return impl_->get(key, CollectionImplBase::GET_AUTO).first;
}

const Variant Collection::operator[](const Variant& key) const
{
	TF_ASSERT(impl_);

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

	if (impl_->container() == that.impl_->container())
	{
		return true;
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
		if (thisIt->key() != thatIt->key() || thisIt->value() != thatIt->value())
		{
			return false;
		}

		thisIt->inc();
		thatIt->inc();
	}

	return true;
}

bool Collection::operator<(const Collection& that) const
{
	if (impl_ == nullptr)
	{
		return true;
	}
	if (that.impl_ == nullptr)
	{
		return false;
	}
	return impl_->container() < that.impl_->container();
}

int Collection::flags() const
{
	if (impl_)
	{
		return impl_->flags();
	}
	else
	{
		return 0;
	}
}

Connection Collection::connectPreInsert(ElementRangeCallback callback)
{
	if (impl_)
	{
		return impl_->connectPreInsert(callback);
	}
	else
	{
		return Connection();
	}
}

Connection Collection::connectPostInserted(ElementRangeCallback callback)
{
	if (impl_)
	{
		return impl_->connectPostInserted(callback);
	}
	else
	{
		return Connection();
	}
}

Connection Collection::connectPreErase(ElementRangeCallback callback)
{
	if (impl_)
	{
		return impl_->connectPreErase(callback);
	}
	else
	{
		return Connection();
	}
}

Connection Collection::connectPostErased(ElementRangeCallback callback)
{
	if (impl_)
	{
		return impl_->connectPostErased(callback);
	}
	else
	{
		return Connection();
	}
}

Connection Collection::connectPreChange(ElementPreChangeCallback callback)
{
	if (impl_)
	{
		return impl_->connectPreChange(callback);
	}
	else
	{
		return Connection();
	}
}

Connection Collection::connectPostChanged(ElementPostChangedCallback callback)
{
	if (impl_)
	{
		return impl_->connectPostChanged(callback);
	}
	else
	{
		return Connection();
	}
}

//------------------------------------------------------------------------------
uint64_t Collection::getHashcode() const
{
	if (impl_ == nullptr)
	{
		return 0;
	}
	return reinterpret_cast<uint64_t>(impl_->container());
}


//==============================================================================
Collection::IteratorImpl::IteratorImpl( const CollectionIteratorImplPtr & impl )
	: ConstIterator( impl )
{
}

//------------------------------------------------------------------------------
bool Collection::IteratorImpl::setValue(ManagedObjectPtr & v) const
{
	return impl()->setValue(std::move(v));
}

} // end namespace wgt
