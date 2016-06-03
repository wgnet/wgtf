#include "wg_list_iterator.hpp"

#include "core_data_model/i_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"

#include "core_qt_common/helpers/qt_helpers.hpp"

namespace wgt
{
WGListIterator::WGListIterator( IListModel & listModel )
	: listModel_( listModel )
	, size_( listModel.size() )
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
		currentItem_ = listModel_.item( currentIndex_++ );
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

	auto data = currentItem_->getData( 0, ValueRole::roleId_ );
	return QtHelpers::toQVariant( data, const_cast<WGListIterator*>(this) );
}

void WGListIterator::setCurrent( QVariant & value )
{
	if (currentItem_ == nullptr)
	{
		return;
	}

	auto data = QtHelpers::toVariant( value );
	currentItem_->setData( 0, ValueRole::roleId_, data );
}
} // end namespace wgt
