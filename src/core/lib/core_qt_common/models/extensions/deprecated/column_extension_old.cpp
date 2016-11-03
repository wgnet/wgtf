#include "column_extension_old.hpp"
#include "core_qt_common/models/adapters/column_list_adapter.hpp"
#include "core_qt_common/models/adapters/indexed_adapter.hpp"

namespace wgt
{
struct ColumnExtensionOld::Implementation
{
	std::vector< IndexedAdapter< ColumnListAdapter > > columnModels_;
	std::vector< std::unique_ptr< ColumnListAdapter > > redundantColumnModels_;
};


ColumnExtensionOld::ColumnExtensionOld()
	: impl_( new Implementation() )
{
}


ColumnExtensionOld::~ColumnExtensionOld()
{
}


QHash< int, QByteArray > ColumnExtensionOld::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	this->registerRole( ColumnModelRole::roleName_, roleNames );
	return roleNames;
}


QVariant ColumnExtensionOld::data( const QModelIndex &index, int role ) const
{
	ItemRole::Id roleId;
	if (!this->decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId != ColumnModelRole::roleId_)
	{
		return QVariant( QVariant::Invalid );
	}

	auto it = std::find( impl_->columnModels_.begin(), 
		impl_->columnModels_.end(), index );
	if (it != impl_->columnModels_.end())
	{
		return QVariant::fromValue< QAbstractItemModel* >( it->data_.get() );
	}
	else
	{
		auto pColumnModel = new ColumnListAdapter( index );
		impl_->columnModels_.emplace_back( index, pColumnModel );
		return QVariant::fromValue< QAbstractItemModel* >( pColumnModel );
	}
}


bool ColumnExtensionOld::setData( 
	const QModelIndex &index, const QVariant &value, int role )
{
	return false;
}


void ColumnExtensionOld::onLayoutAboutToBeChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		isolateRedundantIndices( 
			*it, impl_->columnModels_, impl_->redundantColumnModels_ );
	}
}


void ColumnExtensionOld::onLayoutChanged(
	const QList<QPersistentModelIndex> & parents, 
	QAbstractItemModel::LayoutChangeHint hint )
{
	impl_->redundantColumnModels_.clear();
}


void ColumnExtensionOld::onRowsAboutToBeRemoved( 
	const QModelIndex& parent, int first, int last )
{
	isolateRedundantIndices( parent, first, last,
		impl_->columnModels_, impl_->redundantColumnModels_ );
}


void ColumnExtensionOld::onRowsRemoved(
	const QModelIndex & parent, int first, int last )
{
	impl_->redundantColumnModels_.clear();
}
} // end namespace wgt
