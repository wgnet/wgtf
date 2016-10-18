#include "collection_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "reflection/reflected_collection.hpp"

namespace wgt
{
ITEMROLE( key )
ITEMROLE( keyType )

namespace CollectionModel_Detail
{
	class CollectionItem : public AbstractItem
	{
	public:
		CollectionItem(CollectionModel & model, size_t index)
			: model_(model)
			, index_(index)
		{

		}

		Variant getData(int row, int column, ItemRole::Id roleId) const override
		{
			auto & collection = model_.getSource();
			if (roleId == ItemRole::valueTypeId)
			{
				return collection.valueType().getName();
			}
			else if (roleId == ItemRole::keyTypeId)
			{
				return collection.keyType().getName();
			}

			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return Variant();
			}

			if (roleId == ItemRole::valueId)
			{
				return it.value();
			}
			else if (roleId == ItemRole::keyId)
			{
				return it.key();
			}

			return Variant();
		}

		bool setData(int row, int column, ItemRole::Id roleId, const Variant & data) override
		{
			if (roleId == ItemRole::valueTypeId)
			{
				// Need to return true so that undo state is restored correctly
				return true;
			}
			else if (roleId == ItemRole::keyTypeId)
			{
				// Need to return true so that undo state is restored correctly
				return true;
			}
			else if (roleId == ItemRole::valueId)
			{
				auto & collection = model_.getSource();
				auto it = collection.begin();
				for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
				if (it == collection.end())
				{
					return false;
				}

				it.setValue( data );
				return true;
			}
			else if (roleId == ItemRole::keyId)
			{
				// Need to return true so that undo state is restored correctly
				return true;
			}
			return false;
		}

		CollectionModel & model_;
		size_t index_;
	};

    void updateCacheAfterInsert(int row,
                                int count,
                                std::vector<std::unique_ptr<AbstractItem>>& items)
    {
	    // Move items in cache
	    const auto start = static_cast<size_t>(row);
	    const auto end = static_cast<size_t>(row + count);
	    if (start < items.size())
	    {
		    // Insert new items in cache
		    for (int i = 0; i < count; ++i)
		    {
			    items.insert(items.begin() + start, nullptr);
		    }

		    // Update indexes of moved items
		    for (size_t index = end; index < items.size(); ++index)
		    {
			    auto& item = items[index];
			    if (item != nullptr)
			    {
				    auto pItem = static_cast<CollectionItem*>(item.get());
				    pItem->index_ += count;
			    }
		    }
	    }
    }

    void updateCacheAfterRemove(int row,
                                int count,
                                std::vector<std::unique_ptr<AbstractItem>>& items)
    {
	    const auto start = static_cast<size_t>(row);
	    if (start < items.size())
	    {
		    const auto end = std::min(static_cast<size_t>(row + count),
		                              items.size());
		    const auto newLastItr = items.erase(items.begin() + start, items.begin() + end);

		    // Update indexes of moved items
		    for (auto itr = newLastItr; itr != items.end(); ++itr)
		    {
			    auto& item = (*itr);
			    if (item != nullptr)
			    {
				    auto pItem = static_cast<CollectionModel_Detail::CollectionItem*>(item.get());
				    pItem->index_ -= count;
			    }
		    }
	    }
    }
    } // end namespace CollectionModel_Detail

CollectionModel::CollectionModel()
{
}


CollectionModel::~CollectionModel()
{
}

#define CONNECT_METHOD(method, connection, callbackType) \
Connection CollectionModel::method(AbstractListModel::callbackType callback) \
{ \
	return connection.connect(callback); \
} \

CONNECT_METHOD(connectPreItemDataChanged, preItemDataChanged_, DataCallback)
CONNECT_METHOD(connectPostItemDataChanged, postItemDataChanged_, DataCallback)

CONNECT_METHOD(connectPreRowsInserted, preRowsInserted_, RangeCallback)
CONNECT_METHOD(connectPostRowsInserted, postRowsInserted_, RangeCallback)

CONNECT_METHOD(connectPreRowsRemoved, preRowsRemoved_, RangeCallback)
CONNECT_METHOD(connectPostRowsRemoved, postRowsRemoved_, RangeCallback)

#undef CONNECT_METHOD

void CollectionModel::setSource(Collection & collection)
{
	// TODO emit signal
	items_.clear();

	connectPreChange_.disconnect();
	connectPostChanged_.disconnect();
	connectPreInsert_.disconnect();
	connectPostInserted_.disconnect();
	connectPreErase_.disconnect();
	connectPostErase_.disconnect();

	collection_ = collection;

	//callback(int row, int column, size_t role, const Variant & value)
#define VALUE_CALLBACK(callback) \
		{ \
			ItemRole::Id role = ValueTypeRole::roleId_; \
			int row = -1; \
			if (pos.key().tryCast<int>(row)) \
			{ \
				callback(row, 0, role, value); \
			} \
			else \
			{ \
				auto it = collection_.begin(); \
				int index = 0; \
				for ( ; it != collection_.end() && it != pos; ++it, ++index); \
				callback(index, 0, role, value); \
			} \
		}

	//callback(int row, int count)
#define RANGE_CALLBACK(callback) \
	{\
		int row = -1;\
		if (pos.key().tryCast<int>(row))\
		{\
			callback(row, (int)count);\
		}\
		else\
		{\
			auto it = collection_.begin();\
			int index = 0;\
			for ( ; it != collection_.end() && it != pos; ++it, ++index);\
			callback(index, (int)count);\
		}\
	}
	
	connectPreChange_ =
		collection_.connectPreChange(
		(Collection::ElementPreChangeCallback)[=](const Collection::Iterator& pos, const Variant& value)
			VALUE_CALLBACK(preItemDataChanged_)
		);
	
	connectPostChanged_ =
		collection_.connectPostChanged(
		(Collection::ElementPostChangedCallback)[=](const Collection::Iterator& pos, const Variant& value)
			VALUE_CALLBACK(postItemDataChanged_)
		);

	connectPreInsert_ =
		collection_.connectPreInsert(
		(Collection::ElementRangeCallback)[=](const Collection::Iterator& pos, size_t count)
			RANGE_CALLBACK(preRowsInserted_)
		);
	
	connectPostInserted_ =
		collection_.connectPostInserted(
		(Collection::ElementRangeCallback)[=](const Collection::Iterator& pos, size_t count)
			RANGE_CALLBACK(postRowsInserted_)
		);

	connectPreErase_ =
		collection_.connectPreErase(
		(Collection::ElementRangeCallback)[=](const Collection::Iterator& pos, size_t count)
			RANGE_CALLBACK(preRowsRemoved_)
		);

	connectPostErase_ =
		collection_.connectPostErased(
		(Collection::ElementRangeCallback)[=](const Collection::Iterator& pos, size_t count)
			RANGE_CALLBACK(postRowsRemoved_)
		);

#undef VALUE_CALLBACK
#undef RANGE_CALLBACK
}


const Collection & CollectionModel::getSource() const
{
	return collection_;
}


Collection & CollectionModel::getSource()
{
	return collection_;
}


AbstractItem * CollectionModel::item(int index) const
{
	// Do not create items past the end of the collection
	if (static_cast< size_t >( index ) >= collection_.size())
	{
		return nullptr;
	}

	if (items_.size() <= (size_t)index) 
	{
		items_.resize(index + 1);
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	item = new CollectionModel_Detail::CollectionItem(
	*const_cast<CollectionModel*>(this), index);
	items_[index] = std::unique_ptr< AbstractItem >(item);
	return item;
}


int CollectionModel::index(const AbstractItem* item) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it) {}
	assert(it != items_.end());
	return index;
}


int CollectionModel::rowCount() const
{
	return (int)collection_.size();
}


int CollectionModel::columnCount() const
{
	return 1;
}


bool CollectionModel::insertRows( int row, int count ) /* override */
{
	// Insert/remove by row disabled for mapping types
	if (collection_.isMapping())
	{
		assert( false && "Use insertItem instead" );
		return false;
	}

	if ((row < 0) || (count < 0))
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert index directly to key
	Variant key( row );
	for (int i = 0; i < count; ++i)
	{
		// Repeatedly inserting items at the same key
		// should add count items before the first
		const auto insertItr = collection_.insert( key );
		if (insertItr == collection_.end())
		{
			return false;
		}
	}

	CollectionModel_Detail::updateCacheAfterInsert(row, count, items_);

	return true;
}


bool CollectionModel::removeRows( int row, int count ) /* override */
{
	// Insert/remove by row disabled for mapping types
	if (collection_.isMapping())
	{
		assert( false && "Use removeItem instead" );
		return false;
	}

	if ((row < 0) || (count < 0))
	{
		return false;
	}

	// Trying to remove too many rows
	if ((row + count) > this->rowCount())
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert index directly to key
	Variant key( row );
	for (int i = 0; i < count; ++i)
	{
		// Repeatedly removing items at the same key
		// should remove count items after the first
		const auto erasedCount = collection_.eraseKey( key );
		if (erasedCount == 0)
		{
			return false;
		}
	}

	// Remove from item cache
	const auto start = static_cast< size_t >( row );
	if (start < items_.size())
	{
		const auto end = std::min( static_cast< size_t >( row + count ),
			items_.size() );
		auto newLastItr = items_.erase( items_.begin() + start, items_.begin() + end );
		
		// Update indexes of moved items
		for (auto itr = newLastItr; itr != items_.end(); ++itr)
		{
			auto & item = (*itr);
			if (item != nullptr)
			{
				auto pItem = static_cast<CollectionModel_Detail::CollectionItem*>(item.get());
				pItem->index_ -= count;
			}
		}
	}

	return true;
}


std::vector< std::string > CollectionModel::roles() const
{
	std::vector< std::string > roles;
	roles.push_back( ItemRole::valueName );
	roles.push_back( ItemRole::valueTypeName );
	roles.push_back( ItemRole::keyName );
	roles.push_back( ItemRole::keyTypeName );
	return roles;
}

AbstractItem* CollectionModel::find(const Variant& key)
{
	const auto itr = collection_.find(key);
	if (itr == collection_.cend())
	{
		return nullptr;
	}

	int index = 0;
	for (auto countItr = collection_.cbegin(); countItr != itr; ++index, ++countItr)
	{
		// nop
	}
	assert(index < this->rowCount());
	return this->item(index);
}

const AbstractItem* CollectionModel::find(const Variant& key) const
{
	return const_cast<CollectionModel*>(this)->find(key);
}

bool CollectionModel::insertItem(const Variant& key)
{
	const auto insertItr = collection_.insert(key);
	return (insertItr != collection_.end());
}

bool CollectionModel::insertItem(const Variant& key, const Variant& value)
{
	const auto insertItr = collection_.insertValue(key, value);
	const auto success = (insertItr != collection_.end());
	if (!success)
	{
		return false;
	}

	// Move items in cache
	int row = 0;
	const int count = 1;
	for (auto itr = collection_.begin(); itr != insertItr; ++itr)
	{
		++row;
	}

	CollectionModel_Detail::updateCacheAfterInsert(row, count, items_);

	return true;
}

bool CollectionModel::removeItem( const Variant & key )
{
	const auto erasedCount = collection_.eraseKey( key );
	const auto success = (erasedCount > 0);
	if (!success)
	{
		return false;
	}

	// Remove from item cache
	int row = 0;
	const int count = 1;
	for (auto itr = collection_.begin(); itr.key() == key; ++itr)
	{
		++row;
	}

	CollectionModel_Detail::updateCacheAfterRemove(row, count, items_);

	return true;
}


bool CollectionModel::isMapping() const
{
	return collection_.isMapping();
}

bool CollectionModel::hasController() const
{
	return dynamic_cast<ReflectedCollection*>(collection_.impl().get()) != nullptr;
}

} // end namespace wgt
