#include "wg_list_iterator.hpp"

#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
ITEMROLE(value);

WGListIterator::WGListIterator(AbstractListModel& listModel) : listModel_(listModel), size_(listModel.rowCount())
{
	reset();
}

WGListIterator::~WGListIterator()
{
}

void WGListIterator::reset()
{
	currentIndex_ = 0;
	currentItem_ = nullptr;
}

bool WGListIterator::moveNext()
{
	if (currentIndex_ < size_)
	{
		currentItem_ = listModel_.item(currentIndex_++);
		return true;
	}
	currentItem_ = nullptr;
	return false;
}

QVariant WGListIterator::getCurrent() const
{
	if (currentItem_ == nullptr)
	{
		return QVariant();
	}

	auto data = currentItem_->getData(0, 0, ItemRole::valueId);
	return get<IQtHelpers>()->toQVariant(data, const_cast<WGListIterator*>(this));
}

void WGListIterator::setCurrent(QVariant& value)
{
	if (currentItem_ == nullptr)
	{
		return;
	}

	auto data = get<IQtHelpers>()->toVariant(value);
	currentItem_->setData(0, 0, ItemRole::valueId, data);
}
} // end namespace wgt
