#include "collection_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/resizing_memory_stream.hpp"

namespace wgt
{
ITEMROLE( value )
ITEMROLE( key )
ITEMROLE( valueType )
ITEMROLE( keyType )
namespace
{
	class CollectionItem : public AbstractItem
	{
	public:
		CollectionItem(CollectionModel & model, size_t index)
			: model_(model)
			, index_(index)
		{

		}

		Variant getData(int row, int column, size_t roleId) const override
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

		bool setData(int row, int column, size_t roleId, const Variant & data) override
		{
			if (roleId != ValueRole::roleId_)
			{
				return false;
			}

			auto & collection = model_.getSource();
			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return false;
			}

			it.setValue(data);
			return true;
		}

	private:
		CollectionModel & model_;
		size_t index_;
	};
}


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
			size_t role = ValueTypeRole::roleId_; \
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
	if (items_.size() <= (size_t)index) 
	{
		items_.resize(index + 1);
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	item = new CollectionItem(*const_cast< CollectionModel * >(this), index);
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
} // end namespace wgt
