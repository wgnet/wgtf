#include "wg_filtered_list_model.hpp"

#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/wg_filter.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_reflection/object_handle.hpp"

#include <QRegExp>

namespace wgt
{
struct WGFilteredListModel::Implementation
{
	Implementation( WGFilteredListModel & self );
	~Implementation();

	void setFilter( WGFilter * filter );
	void onFilterChanged();
	void onFilteringBegin();
	void onFilteringEnd();

	WGFilteredListModel & self_;
	WGFilter * filter_;
	FilteredListModel filteredModel_;
	QtConnectionHolder qtConnections_;
	ConnectionHolder connections_;
	Connection filterChangedConnection_;
};

WGFilteredListModel::Implementation::Implementation( WGFilteredListModel & self )
	: self_( self )
	, filter_( nullptr )
{
	connections_ += filteredModel_.onFilteringBegin.connect( std::bind( &WGFilteredListModel::Implementation::onFilteringBegin, this ) );
	connections_ += filteredModel_.onFilteringEnd.connect( std::bind( &WGFilteredListModel::Implementation::onFilteringEnd, this ) );
}

WGFilteredListModel::Implementation::~Implementation()
{
	setFilter( nullptr );
}

void WGFilteredListModel::Implementation::setFilter( WGFilter * filter )
{
	if (filter_ == filter)
	{
		return;
	}

	filterChangedConnection_.disconnect();
	filter_ = filter;
	auto current = filter_ != nullptr ? filter_->getFilter() : nullptr;

	if (current != nullptr)
	{
		filterChangedConnection_ = current->signalFilterChanged.connect( std::bind( &WGFilteredListModel::Implementation::onFilterChanged, this ) );
	}

	filteredModel_.setFilter( current );
	emit self_.filterChanged();
}

void WGFilteredListModel::Implementation::onFilterChanged()
{
	filteredModel_.refresh();
}

void WGFilteredListModel::Implementation::onFilteringBegin()
{
	emit self_.filteringBegin();
}

void WGFilteredListModel::Implementation::onFilteringEnd()
{
	emit self_.filteringEnd();
}

WGFilteredListModel::WGFilteredListModel()
	: impl_( new Implementation( *this ) )
{
	impl_->qtConnections_ += QObject::connect( 
		this, &WGListModel::sourceChanged, 
		this, &WGFilteredListModel::onSourceChanged ); 
}

WGFilteredListModel::~WGFilteredListModel()
{
	setSource( QVariant() );

	impl_->qtConnections_.reset();

	// Temporary hack to circumvent threading deadlock
	// JIRA: NGT-227
	impl_->filteredModel_.setSource( nullptr );
	// End temporary hack

	impl_->setFilter( nullptr );

	// evgenys: reseting impl_ to null first to avoid pure virtual func call in filteredModel_ destructor
	delete impl_.release();
}

IListModel * WGFilteredListModel::getModel() const 
{
	// This component will return the filtered source, not the original source.
	return  impl_ ? &impl_->filteredModel_ : nullptr;
}

void WGFilteredListModel::onSourceChanged()
{
	IListModel * source = nullptr;

	Variant variant = QtHelpers::toVariant( getSource() );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			source = provider.getBase< IListModel >();
		}
	}

	impl_->filteredModel_.setSource( source );
}

QObject * WGFilteredListModel::getFilter() const
{
	return impl_->filter_;
}

void WGFilteredListModel::setFilter( QObject * filter )
{
	auto wgFilter = qobject_cast< WGFilter * >( filter );
	impl_->setFilter( wgFilter );
}

bool WGFilteredListModel::getIsFiltering() const
{
	return impl_->filteredModel_.isFiltering();
}
} // end namespace wgt
