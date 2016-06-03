#include "qt_abstract_item_model.hpp"

#include <QUuid>

#include <private/qmetaobjectbuilder_p.h>

namespace wgt
{
namespace
{
	class ItemData : public QObject
	{
	public:
		struct MetaObject
		{
			MetaObject( QAbstractItemModel & model )
				: model_( model )
			{
				QMetaObjectBuilder builder;
				builder.setClassName( QUuid().toByteArray() );
				builder.setSuperClass( &QObject::staticMetaObject );

				QHashIterator<int, QByteArray> itr( model_.roleNames() );
				while (itr.hasNext())
				{
					itr.next();
					roles_.append( itr.key() );
					auto property = builder.addProperty( itr.value(), "QVariant" );
					property.setNotifySignal( builder.addSignal( itr.value() + "Changed(QVariant)" ) );
				}

				metaObject_ = builder.toMetaObject();
			}

			virtual ~MetaObject()
			{
				// @see QMetaObjectBuilder::toMetaObject()
				// "The return value should be deallocated using free() once it
				// is no longer needed."
				// Allocation was done by the Qt dll, so use global free()
				// Do not use the NGT allocator
				::free( metaObject_ );
			}

			QAbstractItemModel & model_;
			QList< int > roles_;
			QMetaObject * metaObject_;
		};

		ItemData( const QModelIndex & index, std::weak_ptr< MetaObject > metaObject )
			: index_( index )
			, metaObject_( metaObject )
		{
		}

		const QMetaObject * metaObject() const override
		{
			auto metaObject = metaObject_.lock();
			if (metaObject == nullptr)
			{
				return nullptr;
			}

			return metaObject->metaObject_;
		}

		int qt_metacall( QMetaObject::Call c, int id, void **argv ) override
		{
			auto metaObject = metaObject_.lock();
			if (metaObject == nullptr)
			{
				return -1;
			}

			id = QObject::qt_metacall( c, id, argv );
			if (id < 0)
			{
				return id;
			}

			switch (c) 
			{
			case QMetaObject::InvokeMetaMethod:
				{
					auto methodCount = metaObject->metaObject_->methodCount() - metaObject->metaObject_->methodOffset();
					if (id < methodCount)
					{
						metaObject->metaObject_->activate( this, id + metaObject->metaObject_->methodOffset(), argv );
					}
					id -= methodCount;
					break;
				}
			case QMetaObject::ReadProperty:
			case QMetaObject::WriteProperty:
				{
					auto propertyCount = metaObject->metaObject_->propertyCount() - metaObject->metaObject_->propertyOffset();
					if (id < propertyCount)
					{
						auto value = reinterpret_cast< QVariant * >( argv[0] );
						auto role = metaObject->roles_[id];
						if (c == QMetaObject::ReadProperty)
						{
							*value = metaObject->model_.data( index_, role );
						}
						else
						{
							metaObject->model_.setData( index_, *value, role );
						}
					}
					id -= propertyCount;
					break;
				}
			default:
				break;
			}

			return id;
		}

		QModelIndex index_;
		std::weak_ptr< MetaObject > metaObject_;
	};
}

struct QtAbstractItemModel::Impl
{
	std::shared_ptr< ItemData::MetaObject > metaObject_;
};

QtAbstractItemModel::QtAbstractItemModel()
	: impl_( new Impl )
{
	QObject::connect( this, &QAbstractItemModel::modelReset, [&]() 
	{ 
		impl_->metaObject_.reset(); 
	});
}

QtAbstractItemModel::~QtAbstractItemModel()
{
}

QModelIndex QtAbstractItemModel::itemToIndex( QObject * item ) const
{
	auto itemData = qobject_cast< const ItemData * >( item );
	return item != nullptr ? itemData->index_ : QModelIndex();
}

QObject * QtAbstractItemModel::indexToItem( const QModelIndex &index ) const
{
	if (impl_->metaObject_ == nullptr)
	{
		impl_->metaObject_.reset( new ItemData::MetaObject( const_cast< QtAbstractItemModel & >( *this ) ) );
	}
	return index.isValid() ? new ItemData( index, impl_->metaObject_ ) : nullptr;
}

QObject * QtAbstractItemModel::item( int row, int column, QObject * parent ) const
{
	auto parentIndex = itemToIndex( parent );
	auto index = this->index( row, column, parentIndex );
	return indexToItem( index );
}

QObject * QtAbstractItemModel::parent( QObject * child ) const
{
	auto childIndex = itemToIndex( child );
	auto index = parent( childIndex );
	return indexToItem( index );
}

int QtAbstractItemModel::rowIndex( QObject * item ) const
{
	auto index = itemToIndex( item );
	return index.row();
}

int QtAbstractItemModel::columnIndex( QObject * item ) const
{
	auto index = itemToIndex( item );
	return index.column();
}

int QtAbstractItemModel::rowCount( QObject * parent ) const
{
	auto parentIndex = itemToIndex( parent );
	return rowCount( parentIndex );
}

int QtAbstractItemModel::columnCount( QObject * parent ) const
{
	auto parentIndex = itemToIndex( parent );
	return columnCount( parentIndex );
}

bool QtAbstractItemModel::hasChildren( QObject * parent ) const
{
	auto parentIndex = itemToIndex( parent );
	return hasChildren( parentIndex );
}

bool QtAbstractItemModel::insertRow( int row, QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::insertRow( row, parentIndex );
}

bool QtAbstractItemModel::insertColumn( int column, QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::insertColumn( column, parentIndex );
}

bool QtAbstractItemModel::removeRow( int row, QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::removeRow( row, parentIndex );
}

bool QtAbstractItemModel::removeColumn( int column, QObject * parent )
{
	auto parentIndex = itemToIndex( parent );
	return QAbstractItemModel::removeColumn( column, parentIndex );
}

bool QtAbstractItemModel::hasChildren(const QModelIndex &parent) const
{
	return QAbstractItemModel::hasChildren( parent );
}
} // end namespace wgt
