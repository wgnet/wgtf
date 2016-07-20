#include "qt_item_model.hpp"

#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model_cmds/interfaces/i_item_model_controller.hpp"
#include "core_dependency_system/di_ref.hpp"
#include "helpers/qt_helpers.hpp"

namespace wgt
{
ITEMROLE( display )
ITEMROLE( decoration )

struct QtItemModel::Impl
{
	Impl( IComponentContext & context,
		AbstractItemModel & source )
		: itemModelController_( context )
		, source_( source )
	{
	}

	DIRef< IItemModelController > itemModelController_;

	AbstractItemModel & source_;

	Connection connectPreChange_;
	Connection connectPostChanged_;

	Connection connectPreInsert_;
	Connection connectPostInserted_;

	Connection connectPreErase_;
	Connection connectPostErased_;
};

QtItemModel::QtItemModel( IComponentContext & context,
	AbstractItemModel & source )
	: impl_( new Impl( context, source ) )
{
	// @see AbstractItemModel::DataSignature
	auto preData = 
	[ this ]( const AbstractItemModel::ItemIndex & index,
		size_t role,
		const Variant & newValue )
	{
		auto item = impl_->source_.item( index );
		const QModelIndex modelIndex = createIndex( index.row_, index.column_, item );

		//HACK: should not be explicitly referencing DefinitionRole here
		if (role == DefinitionRole::roleId_)
		{
			QList<QPersistentModelIndex> parents;
			parents.append( modelIndex.isValid() ? modelIndex : QModelIndex() );
			this->layoutAboutToBeChanged( parents, QAbstractItemModel::VerticalSortHint );
			return;
		}
		//END HACK
	};
	impl_->connectPreChange_ =
		impl_->source_.connectPreItemDataChanged( preData );
	
	auto postData = 
	[ this ]( const AbstractItemModel::ItemIndex & index,
		size_t role,
		const Variant & newValue )
	{
		auto item = impl_->source_.item( index );
		const QModelIndex modelIndex = createIndex( index.row_, index.column_, item );

		//HACK: should not be explicitly referencing DefinitionRole here
		if (role == DefinitionRole::roleId_)
		{
			QList<QPersistentModelIndex> parents;
			parents.append( modelIndex.isValid() ? modelIndex : QModelIndex() );
			this->layoutChanged( parents, QAbstractItemModel::VerticalSortHint );
			return;
		}
		//END HACKs

		const QModelIndex topLeft = modelIndex;
		const QModelIndex bottomRight = modelIndex;
		QVector< int > roles;
		if (role == ItemRole::displayId)
		{
			roles.append( Qt::DisplayRole );
		}
		else if (role == ItemRole::decorationId)
		{
			roles.append( Qt::DecorationRole );
		}
		else
		{
			int encodedRole = static_cast< int >( role );
			encodeRole( role, encodedRole );
			roles.append( encodedRole );
		}
		this->dataChanged( topLeft, bottomRight, roles );
	};
	impl_->connectPostChanged_ =
		impl_->source_.connectPostItemDataChanged( postData );

	// @see AbstractItemModel::RangeSignature
	auto preInsert = 
		[ this ]( const AbstractItemModel::ItemIndex & parentIndex,
		int startPos,
		int count )
	{
		auto parentItem = impl_->source_.item( parentIndex );
		const QModelIndex modelIndex = createIndex( parentIndex.row_, parentIndex.column_, parentItem );
		this->beginInsertRows( modelIndex.isValid() ? modelIndex : QModelIndex(), startPos, startPos + count - 1 );
	};
	impl_->connectPreInsert_ =
		impl_->source_.connectPreRowsInserted( preInsert );

	auto postInserted = 
		[ this ]( const AbstractItemModel::ItemIndex & parentIndex,
		int startPos,
		int count )
	{
		this->endInsertRows();
	};
	impl_->connectPostInserted_ =
		impl_->source_.connectPostRowsInserted( postInserted );

	auto preErase = 
		[ this ]( const AbstractItemModel::ItemIndex & parentIndex,
		int startPos,
		int count )
	{
		auto parentItem = impl_->source_.item( parentIndex );
		const QModelIndex modelIndex = createIndex( parentIndex.row_, parentIndex.column_, parentItem );
		this->beginRemoveRows( modelIndex.isValid() ? modelIndex : QModelIndex(), startPos, startPos + count - 1 );
	};
	impl_->connectPreErase_ =
		impl_->source_.connectPreRowsRemoved( preErase );

	auto postErased = 
		[ this ]( const AbstractItemModel::ItemIndex & parentIndex,
		int startPos,
		int count )
	{
		this->endRemoveRows();
	};
	impl_->connectPostErased_ =
		impl_->source_.connectPostRowsRemoved( postErased );
}

QtItemModel::~QtItemModel()
{
	// TODO not removed from list??
	impl_->connectPostChanged_.disconnect();
	impl_->connectPreChange_.disconnect();
}

const AbstractItemModel & QtItemModel::source() const
{
	return impl_->source_;
}

AbstractItemModel & QtItemModel::source()
{
	return impl_->source_;
}

QModelIndex QtItemModel::index( int row, int column, const QModelIndex &parent ) const
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr;

	AbstractItemModel::ItemIndex itemIndex( row, column, parentItem );
	auto item = impl_->source_.item( itemIndex );
	if (item == nullptr)
	{
		return QModelIndex();
	}

	return createIndex( row, column, item );
}

QModelIndex QtItemModel::parent( const QModelIndex &child ) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}

	auto childItem = reinterpret_cast< AbstractItem * >( child.internalId() );
	AbstractItemModel::ItemIndex childIndex;
	impl_->source_.index( childItem, childIndex );
	if (!childIndex.isValid())
	{
		return QModelIndex();
	}

	auto parentItem = const_cast< AbstractItem * >( childIndex.parent_ );
	if (parentItem == nullptr)
	{
		return QModelIndex();
	}

	AbstractItemModel::ItemIndex parentIndex;
	impl_->source_.index( parentItem, parentIndex );
	if (!parentIndex.isValid())
	{
		return QModelIndex();
	}

	return createIndex( parentIndex.row_, parentIndex.column_, parentItem );
}

int QtItemModel::rowCount( const QModelIndex &parent ) const
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr;

	return impl_->source_.rowCount( parentItem );
}

int QtItemModel::columnCount( const QModelIndex &parent ) const
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr;

	return impl_->source_.columnCount( parentItem );
}

bool QtItemModel::hasChildren( const QModelIndex &parent ) const
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr;

	return impl_->source_.hasChildren( parentItem );
}

QVariant QtItemModel::data( const QModelIndex &index, int role ) const
{
	auto item = index.isValid() ?
		reinterpret_cast< AbstractItem * >( index.internalId() ) : nullptr; 

	if (item == nullptr)
	{
		return QVariant();
	}
	
	size_t roleId = role;
	switch (role)
	{
	case Qt::DisplayRole:
		roleId = ItemRole::displayId;
		break;

	case Qt::DecorationRole:
		roleId = ItemRole::decorationId;
		break;

	default:
		decodeRole( role, roleId );
		break;
	}

	auto data = item->getData( index.row(), index.column(), roleId );
	return QtHelpers::toQVariant( data, const_cast<QtItemModel*>(this) );
}

bool QtItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
	auto item = index.isValid() ?
		reinterpret_cast< AbstractItem * >( index.internalId() ) : nullptr; 

	if (item == nullptr)
	{
		return false;
	}

	size_t roleId = role;
	switch (role)
	{
	case Qt::DisplayRole:
		roleId = ItemRole::displayId;
		break;

	case Qt::DecorationRole:
		roleId = ItemRole::decorationId;
		break;

	default:
		decodeRole( role, roleId );
		break;
	}

	// Use QVariant for comparison
	// because QVariant has lower precision than Variant
	auto oldValue = QtHelpers::toQVariant( item->getData( index.row(), index.column(), roleId ), this );
	if (value == oldValue)
	{
		return true;
	}

	auto data = QtHelpers::toVariant( value );

	// Item already uses the Command System or the Command System is not available
	if (item->hasController() || (impl_->itemModelController_ == nullptr))
	{
		// Set property directly
		return item->setData( index.row(), index.column(), roleId, data );
	}

	// Queue with Command System, to register undo/redo data
	auto pParent = index.parent().isValid() ?
		reinterpret_cast< AbstractItem * >( index.parent().internalId() ) : nullptr; 
	AbstractItemModel::ItemIndex dataModelIndex( index.row(),
		index.column(),
		pParent );

	return impl_->itemModelController_->setValue( impl_->source_,
		dataModelIndex,
		roleId,
		data );
}

QVariant QtItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	auto row = orientation == Qt::Vertical ? section : -1;
	auto column = orientation == Qt::Horizontal ? section : -1;
	auto data = impl_->source_.getData( row, column, role );
	return QtHelpers::toQVariant( data, nullptr );
}

bool QtItemModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role )
{
	auto row = orientation == Qt::Vertical ? section : -1;
	auto column = orientation == Qt::Horizontal ? section : -1;
	auto data = QtHelpers::toVariant( value );
	return impl_->source_.setData( row, column, role, data );
}

bool QtItemModel::insertRows( int row, int count, const QModelIndex &parent )
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr; 
	return impl_->source_.insertRows( row, count, parentItem );
}

bool QtItemModel::insertColumns( int column, int count, const QModelIndex &parent )
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr; 
	return impl_->source_.insertColumns( column, count, parentItem );
}

bool QtItemModel::removeRows( int row, int count, const QModelIndex &parent )
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr; 
	return impl_->source_.removeRows( row, count, parentItem );
}

bool QtItemModel::removeColumns( int column, int count, const QModelIndex &parent )
{
	auto parentItem = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr; 
	return impl_->source_.removeColumns( column, count, parentItem );
}

QHash< int, QByteArray > QtItemModel::roleNames() const
{
	auto roles = impl_->source_.roles();
	auto roleNames = QAbstractItemModel::roleNames();
	for (auto & role : roles)
	{
		registerRole( role.c_str(), roleNames );
	}
	return roleNames;
}

QtListModel::QtListModel( IComponentContext & context,
	AbstractListModel & source ) 
	: QtItemModel( context, source ) 
{}

const AbstractListModel & QtListModel::source() const
{
	return static_cast< const AbstractListModel & >( QtItemModel::source() );
}

AbstractListModel & QtListModel::source()
{
	return static_cast< AbstractListModel & >( QtItemModel::source() );
}

QObject * QtListModel::item( int row ) const 
{
	return QtAbstractItemModel::item( row, 0, nullptr ); 
}

int QtListModel::count() const 
{ 
	return QtAbstractItemModel::rowCount( nullptr ); 
}


bool QtListModel::insertItem( int row )
{
	return QtAbstractItemModel::insertRow( row, nullptr ); 
}


bool QtListModel::removeItem( int row )
{
	return QtAbstractItemModel::removeRow( row, nullptr ); 
}


QtTreeModel::QtTreeModel( IComponentContext & context, AbstractTreeModel & source ) 
	: QtItemModel( context, source ) 
{}

const AbstractTreeModel & QtTreeModel::source() const
{
	return static_cast< const AbstractTreeModel & >( QtItemModel::source() );
}

AbstractTreeModel & QtTreeModel::source()
{
	return static_cast< AbstractTreeModel & >( QtItemModel::source() );
}

QObject * QtTreeModel::item( int row, QObject * parent ) const 
{ 
	return QtAbstractItemModel::item( row, 0, parent ); 
}

int QtTreeModel::count( QObject * parent ) const 
{ 
	return QtAbstractItemModel::rowCount( parent ); 
}


bool QtTreeModel::insertItem( int row, QObject * parent )
{
	return QtAbstractItemModel::insertRow( row, parent ); 
}


bool QtTreeModel::removeItem( int row, QObject * parent )
{
	return QtAbstractItemModel::removeRow( row, parent ); 
}


QtTableModel::QtTableModel( IComponentContext & context,
	AbstractTableModel & source ) 
	: QtItemModel( context, source ) 
{}

const AbstractTableModel & QtTableModel::source() const
{
	return static_cast< const AbstractTableModel & >( QtItemModel::source() );
}

AbstractTableModel & QtTableModel::source()
{
	return static_cast< AbstractTableModel & >( QtItemModel::source() );
}

QObject * QtTableModel::item( int row, int column ) const 
{
	return QtAbstractItemModel::item( row, column, nullptr ); 
}

int QtTableModel::rowCount() const 
{ 
	return QtAbstractItemModel::rowCount( nullptr ); 
}

int QtTableModel::columnCount() const 
{ 
	return QtAbstractItemModel::columnCount( nullptr ); 
}


bool QtTableModel::insertRow( int row )
{
	return QtAbstractItemModel::insertRow( row, nullptr ); 
}


bool QtTableModel::insertColumn( int column )
{
	return QtAbstractItemModel::insertColumn( column, nullptr ); 
}


bool QtTableModel::removeRow( int row )
{
	return QtAbstractItemModel::removeRow( row, nullptr ); 
}


bool QtTableModel::removeColumn( int column )
{
	return QtAbstractItemModel::removeColumn( column, nullptr ); 
}
} // end namespace wgt
