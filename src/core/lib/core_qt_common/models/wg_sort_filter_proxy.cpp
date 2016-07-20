#include "wg_sort_filter_proxy.hpp"
#include "qt_item_data.hpp"

namespace wgt
{

struct WGSortFilterProxy::Impl
{
	Impl() : sortFilter_( nullptr ) {}

	QObject * sortFilter_;
	std::shared_ptr< QtItemData::MetaObject > metaObject_;
};

WGSortFilterProxy::WGSortFilterProxy()
	: impl_( new Impl )
{
	QObject::connect( this, &QAbstractProxyModel::sourceModelChanged, this, &WGSortFilterProxy::onSourceModelChanged );
	QObject::connect( this, &WGSortFilterProxy::sortFilterChanged, this, &WGSortFilterProxy::onSortFilterChanged );
}

WGSortFilterProxy::~WGSortFilterProxy()
{

}

void WGSortFilterProxy::sort( int column, Qt::SortOrder order )
{
	QSortFilterProxyModel::sort( column, order );
}

QObject * WGSortFilterProxy::getSortFilter() const
{
	return impl_->sortFilter_;
}

void WGSortFilterProxy::setSortFilter( QObject * sortFilter )
{
	if (impl_->sortFilter_ == sortFilter)
	{
		return;
	}

	impl_->sortFilter_ = sortFilter;
	emit sortFilterChanged();
}

void WGSortFilterProxy::onSourceModelChanged()
{
	auto model = sourceModel();
	impl_->metaObject_ = model != nullptr ? QtItemData::getMetaObject( *model ) : nullptr;
}

void WGSortFilterProxy::onSortFilterChanged()
{
	invalidateFilter();
}

bool WGSortFilterProxy::lessThan( const QModelIndex & source_left, const QModelIndex & source_right ) const
{
	if (impl_->sortFilter_ != nullptr)
	{
		QVariant ret;
		auto left = std::unique_ptr< QObject >( new QtItemData( source_left, impl_->metaObject_ ) );
		auto right = std::unique_ptr< QObject >( new QtItemData( source_right, impl_->metaObject_ ) );
		if (QMetaObject::invokeMethod( impl_->sortFilter_, "lessThan", 
				Q_RETURN_ARG( QVariant, ret ), 
				Q_ARG( QVariant, QVariant::fromValue( left.get() ) ), 
				Q_ARG( QVariant, QVariant::fromValue( right.get() ) ) ))
		{
			return ret.toBool();
		}
	}

	return QSortFilterProxyModel::lessThan( source_left, source_right );
}

}