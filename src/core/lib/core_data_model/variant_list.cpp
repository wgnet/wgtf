#include "variant_list.hpp"
#include "i_item.hpp"
#include "i_item_role.hpp"
#include "core_variant/variant.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_serialization/resizing_memory_stream.hpp"

namespace wgt
{
class VariantListItem : public IItem
{
public:
	VariantListItem(const Variant& value);
	VariantListItem(Variant&& value);

	// IItem
	const char* getDisplayText(int column) const override;
	ThumbnailData getThumbnail(int column) const override;
	Variant getData(int column, ItemRole::Id roleId) const override;
	bool setData(int column, ItemRole::Id roleId, const Variant& data) override;
	//

private:
	void initDisplayName();

	Variant value_;
	std::string displayName_;

	friend class VariantList;
};

VariantListItem::VariantListItem(const Variant& value) : value_(value)
{
	initDisplayName();
}

VariantListItem::VariantListItem(Variant&& value) : value_(std::move(value))
{
	initDisplayName();
}

const char* VariantListItem::getDisplayText(int column) const
{
	return displayName_.c_str();
}

ThumbnailData VariantListItem::getThumbnail(int column) const
{
	return nullptr;
}

Variant VariantListItem::getData(int column, ItemRole::Id roleId) const
{
	if (roleId == ValueTypeRole::roleId_)
	{
		ObjectHandle handle;
		if (value_.tryCast(handle))
		{
			return handle.type().getName();
		}

		auto type = value_.type();
		if (type != nullptr)
		{
			auto typeId = type->typeId();
			if (typeId == TypeId::getType<ObjectHandle>())
			{
				ObjectHandle objHandle;
				bool isOk = value_.tryCast(objHandle);
				assert(isOk);
				typeId = objHandle.type();
			}
			return typeId.getName();
		}
	}
	else if (roleId == ValueRole::roleId_)
	{
		return value_;
	}
	else if (roleId == IndexPathRole::roleId_)
	{
		ResizingMemoryStream dataStream;
		TextStream s(dataStream);
		s << value_;
		return dataStream.takeBuffer();
	}

	return Variant();
}

bool VariantListItem::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	if (roleId != ValueRole::roleId_)
	{
		return false;
	}

	value_ = data;
	initDisplayName();
	return true;
}

void VariantListItem::initDisplayName()
{
	displayName_ = getData(0, ValueTypeRole::roleId_).value<std::string>();
}

VariantList::ConstIterator::ConstIterator(const ConstIterator& rhs)
    : iterator_(new Items::const_iterator(*rhs.iterator_))
{
}

VariantList::ConstIterator& VariantList::ConstIterator::operator=(const ConstIterator& rhs)
{
	if (this != &rhs)
	{
		iterator_.reset(new Items::const_iterator(*rhs.iterator_));
	}

	return *this;
}

VariantList::ConstIterator::reference VariantList::ConstIterator::operator*() const
{
	return *operator->();
}

VariantList::ConstIterator::pointer VariantList::ConstIterator::operator->() const
{
	auto item = static_cast<VariantListItem*>((*iterator_)->get());
	const Variant& value = item->value_;

	return &value;
}

VariantList::ConstIterator& VariantList::ConstIterator::operator++()
{
	++(*iterator_);
	return *this;
}

VariantList::ConstIterator VariantList::ConstIterator::operator++(int)
{
	ConstIterator tmp = *this;
	++(*this);
	return tmp;
}

bool VariantList::ConstIterator::operator==(const ConstIterator& other) const
{
	return *iterator_ == *other.iterator_;
}

bool VariantList::ConstIterator::operator!=(const ConstIterator& other) const
{
	return !(*this == other);
}

bool VariantList::ConstIterator::operator<(const ConstIterator& other) const
{
	return *iterator_ < *other.iterator_;
}

VariantList::ConstIterator::difference_type VariantList::ConstIterator::operator-(
const VariantList::ConstIterator& other) const
{
	return *iterator_ - *other.iterator_;
}

VariantList::ConstIterator VariantList::ConstIterator::operator+(VariantList::ConstIterator::difference_type n) const
{
	return *iterator_ + n;
}

VariantList::ConstIterator::ConstIterator(const Items::const_iterator& iterator)
    : iterator_(new Items::const_iterator(iterator))
{
}

const VariantList::Items::const_iterator& VariantList::ConstIterator::iterator() const
{
	return *iterator_;
}

VariantList::Iterator::Iterator(const Iterator& rhs) : ConstIterator()
{
	iterator_.reset(new Items::const_iterator(rhs.iterator()));
}

VariantList::Iterator& VariantList::Iterator::operator=(const Iterator& rhs)
{
	if (this != &rhs)
	{
		iterator_.reset(new Items::const_iterator(*rhs.iterator_));
	}

	return *this;
}

VariantList::Iterator::reference VariantList::Iterator::operator*() const
{
	return *operator->();
}

VariantList::Iterator::pointer VariantList::Iterator::operator->() const
{
	auto item = static_cast<VariantListItem*>((*iterator_)->get());
	Variant& value = item->value_;

	return &value;
}

VariantList::Iterator& VariantList::Iterator::operator++()
{
	++(*iterator_);
	return *this;
}

VariantList::Iterator VariantList::Iterator::operator++(int)
{
	Iterator tmp = *this;
	++(*this);
	return tmp;
}

VariantList::Iterator VariantList::Iterator::operator+(VariantList::Iterator::difference_type n) const
{
	ConstIterator itr = *iterator_ + n;
	return *(Iterator*)&itr;
}

VariantList::Iterator::Iterator(const Items::iterator& iterator) : ConstIterator()
{
	iterator_.reset(new Items::const_iterator(iterator));
}

const VariantList::Items::const_iterator& VariantList::Iterator::iterator() const
{
	return *(iterator_.get());
}

VariantList::VariantList()
{
}

VariantList::~VariantList()
{
}

IItem* VariantList::item(size_t index) const
{
	if (index >= items_.size())
	{
		return nullptr;
	}

	return items_[index].get();
}

size_t VariantList::index(const IItem* item) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it)
	{
	}
	assert(it != items_.end());
	return index;
}

bool VariantList::empty() const
{
	return items_.empty();
}

size_t VariantList::size() const
{
	return items_.size();
}

int VariantList::columnCount() const
{
	return 1;
}

bool VariantList::canClear() const
{
	return true;
}

void VariantList::clear()
{
	this->resize(0);
}

void VariantList::resize(size_t newSize)
{
	auto oldSize = size();
	if (newSize < oldSize)
	{
		signalPreItemsRemoved(newSize, oldSize - newSize);
		items_.resize(newSize);
		signalPostItemsRemoved(newSize, oldSize - newSize);
	}
	else if (newSize > oldSize)
	{
		signalPreItemsInserted(oldSize, newSize - oldSize);
		items_.resize(newSize);
		signalPostItemsInserted(oldSize, newSize - oldSize);
	}
}

VariantList::ConstIterator VariantList::cbegin() const
{
	return ConstIterator(items_.cbegin());
}

VariantList::ConstIterator VariantList::cend() const
{
	return ConstIterator(items_.cend());
}

VariantList::ConstIterator VariantList::begin() const
{
	return ConstIterator(items_.cbegin());
}

VariantList::ConstIterator VariantList::end() const
{
	return ConstIterator(items_.cend());
}

VariantList::Iterator VariantList::begin()
{
	return Iterator(items_.begin());
}

VariantList::Iterator VariantList::end()
{
	return Iterator(items_.end());
}

VariantList::Iterator VariantList::insert(const VariantList::Iterator& position, const Variant& value)
{
	auto index = std::distance(items_.cbegin(), position.iterator());

	signalPreItemsInserted(index, 1);
	auto it = items_.emplace(position.iterator(), createItem(value));
	signalPostItemsInserted(index, 1);

	return it;
}

VariantList::Iterator VariantList::erase(const VariantList::Iterator& position)
{
	auto index = std::distance(items_.cbegin(), position.iterator());

	signalPreItemsRemoved(index, 1);
	auto it = items_.erase(position.iterator());
	signalPostItemsRemoved(index, 1);

	return it;
}

VariantList::Iterator VariantList::erase(const VariantList::Iterator& first, const VariantList::Iterator& last)
{
	auto index = std::distance(items_.cbegin(), first.iterator());
	auto count = std::distance(first.iterator(), last.iterator());

	signalPreItemsRemoved(index, count);
	auto it = items_.erase(first.iterator(), last.iterator());
	signalPostItemsRemoved(index, count);

	return it;
}

void VariantList::emplace_back(Variant&& value)
{
	const auto index = items_.size();

	signalPreItemsInserted(index, 1);
	items_.emplace(items_.end(), new VariantListItem(std::move(value)));
	signalPostItemsInserted(index, 1);
}

void VariantList::push_back(const Variant& value)
{
	auto index = items_.size();

	signalPreItemsInserted(index, 1);
	items_.emplace(items_.end(), createItem(value));
	signalPostItemsInserted(index, 1);
}

void VariantList::push_front(const Variant& value)
{
	auto index = 0;

	signalPreItemsInserted(index, 1);
	items_.emplace(items_.begin(), createItem(value));
	signalPostItemsInserted(index, 1);
}

Variant VariantList::pop_back()
{
	auto item = static_cast<const VariantListItem*>(items_.back().get());
	Variant value = item->value_;

	auto index = items_.size() - 1;

	signalPreItemsRemoved(index, 1);
	items_.pop_back();
	signalPostItemsRemoved(index, 1);

	return value;
}

Variant VariantList::pop_front()
{
	auto item = static_cast<const VariantListItem*>(items_.front().get());
	Variant value = item->value_;

	auto index = 0;

	signalPreItemsRemoved(index, 1);
	items_.erase(items_.begin());
	signalPostItemsRemoved(index, 1);

	return value;
}

const Variant& VariantList::back() const
{
	auto item = static_cast<const VariantListItem*>(items_.back().get());
	const Variant& value = item->value_;

	return value;
}

const Variant& VariantList::front() const
{
	auto item = static_cast<const VariantListItem*>(items_.front().get());
	const Variant& value = item->value_;

	return value;
}

Variant& VariantList::operator[](size_t index)
{
	auto item = static_cast<VariantListItem*>(items_[index].get());
	Variant& value = item->value_;

	return value;
}

const Variant& VariantList::operator[](size_t index) const
{
	auto item = static_cast<const VariantListItem*>(items_[index].get());
	const Variant& value = item->value_;

	return value;
}

IItem* VariantList::createItem(const Variant& value)
{
	return new VariantListItem(value);
}
} // end namespace wgt
