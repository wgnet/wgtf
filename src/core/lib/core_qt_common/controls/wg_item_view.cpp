#include "wg_item_view.hpp"
#include "qt_connection_holder.hpp"
#include "models/extensions/i_model_extension.hpp"
#include "models/qt_abstract_item_model.hpp"

#include <functional>

#include <QAbstractItemModel>
#include <QQmlListProperty>
#include <QString>
#include <QUuid>
#include <QtAlgorithms>

#include <private/qmetaobjectbuilder_p.h>

namespace wgt
{
ITEMROLE( itemId )
ITEMROLE( modelIndex )

namespace
{
	class ExtensionData : public IExtensionData
	{
	public:
		void save()
		{
			// TODO
		}

		void load()
		{
			// TODO
		}

		void reset()
		{
			indexCache_.clear();
		}

		ItemData & getItemData( const QModelIndex& index )
		{
			{
				auto it = indexCache_.find( index );
				if (it != indexCache_.end())
				{
					return itemData_[*it];
				}
			}

			QVariant id = index.model()->data( index, static_cast< int >( ItemRole::itemIdId ) );
			{
				auto it = itemIds_.find( id );
				if (it != itemIds_.end())
				{
					indexCache_[index] = *it;
					return itemData_[*it];
				}
			}

			if (id.isValid())
			{
				itemIds_[id] = itemData_.count();
			}

			indexCache_[index] = itemData_.count();
			itemData_.push_back( ItemData() );
			return itemData_.back();
		}

	private:
		QVector< ItemData > itemData_;
		QMap< QVariant, int > itemIds_;

		QMap< QPersistentModelIndex, int > indexCache_;
	};

	class ExtendedModel : public QtAbstractItemModel, public RoleProvider
	{
	public:
		ExtendedModel( QStringList & roles, QList< IModelExtension * > & extensions )
			: model_( nullptr )
			, roles_( roles )
			, extensions_( extensions )
		{
		}

		void reset( QAbstractItemModel * model )
		{
			beginResetModel();
			model_ = model;
			connections_.reset();
			roleNames_.clear();
			extensionData_.reset();
			if (model_ != nullptr)
			{
				connections_ += QObject::connect( model, &QAbstractItemModel::dataChanged, this, &ExtendedModel::onDataChanged );
				connections_ += QObject::connect( model, &QAbstractItemModel::layoutAboutToBeChanged, this, &ExtendedModel::onLayoutAboutToBeChanged );
				connections_ += QObject::connect( model, &QAbstractItemModel::layoutChanged, this, &ExtendedModel::onLayoutChanged );
				connections_ += QObject::connect( model, &QAbstractItemModel::rowsAboutToBeInserted, this, &ExtendedModel::onRowsAboutToBeInserted );
				connections_ += QObject::connect( model, &QAbstractItemModel::rowsInserted, this, &ExtendedModel::onRowsInserted );
				connections_ += QObject::connect( model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ExtendedModel::onRowsAboutToBeRemoved );
				connections_ += QObject::connect( model, &QAbstractItemModel::rowsRemoved, this, &ExtendedModel::onRowsRemoved );

				for (auto & extension : extensions_)
				{
					connections_ += QObject::connect( this, &QAbstractItemModel::layoutAboutToBeChanged, extension, &IModelExtension::onLayoutAboutToBeChanged );
					connections_ += QObject::connect( this, &QAbstractItemModel::layoutChanged, extension, &IModelExtension::onLayoutChanged );
					connections_ += QObject::connect( this, &QAbstractItemModel::rowsAboutToBeInserted, extension, &IModelExtension::onRowsAboutToBeInserted );
					connections_ += QObject::connect( this, &QAbstractItemModel::rowsInserted, extension, &IModelExtension::onRowsInserted );
					connections_ += QObject::connect( this, &QAbstractItemModel::rowsAboutToBeRemoved, extension, &IModelExtension::onRowsAboutToBeRemoved );
					connections_ += QObject::connect( this, &QAbstractItemModel::rowsRemoved, extension, &IModelExtension::onRowsRemoved );
				}

				roleNames_ = model_->roleNames();
				registerRole( ItemRole::modelIndexName, roleNames_ );
				for (auto & role : roles_)
				{
					registerRole( role.toUtf8(), roleNames_ );
				}
				for (auto & extension : extensions_)
				{
					QHashIterator<int, QByteArray> itr( extension->roleNames() );
					while (itr.hasNext())
					{
						itr.next();
						registerRole( itr.value(), roleNames_ );
					}
				}

				for (auto & extension : extensions_)
				{
					extension->init( extensionData_ );
				}
			}
			endResetModel();
		}

	private:
		QModelIndex index( int row, int column, const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return QModelIndex();
			}

			return extendedIndex( model_->index( row, column, modelIndex( parent ) ) );
		}

		QModelIndex parent( const QModelIndex &child ) const override
		{
			if (model_ == nullptr)
			{
				return QModelIndex();
			}

			return extendedIndex( model_->parent( modelIndex( child ) ) );
		}

		int rowCount( const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return 0;
			}

			return model_->rowCount( modelIndex( parent ) );
		}

		int columnCount( const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return 0;
			}

			return model_->columnCount( modelIndex( parent ) );
		}

		bool hasChildren( const QModelIndex &parent ) const override
		{
			if (model_ == nullptr)
			{
				return false;
			}
			
			return model_->hasChildren( modelIndex( parent ) );
		}

		QVariant data( const QModelIndex &index, int role ) const override
		{
			if (model_ == nullptr)
			{
				return QVariant();
			}

			for (auto & extension : extensions_)
			{
				auto data = extension->data( index, role );
				if (data.isValid())
				{
					return data;
				}
			}

			size_t roleId;
			if (decodeRole( role, roleId ))
			{
				if (roleId == ItemRole::modelIndexId)
				{
					return index;
				}
				role = static_cast< int >( roleId );
			}

			return model_->data( modelIndex( index ), role );
		}

		bool setData( const QModelIndex &index, const QVariant &value, int role ) override
		{
			if (model_ == nullptr)
			{
				return false;
			}
			
			for (auto & extension : extensions_)
			{
				if (extension->setData( index, value, role ) )
				{
					return true;
				}
			}

			size_t roleId;
			if (decodeRole( role, roleId ))
			{
				role = static_cast< int >( roleId );
			}

			return model_->setData( modelIndex( index ), value, role );
		}

		QVariant headerData( int section, Qt::Orientation orientation, int role ) const override
		{
			if (model_ == nullptr)
			{
				return QVariant();
			}

			for (auto & extension : extensions_)
			{
				auto data = extension->headerData( section, orientation, role );
				if (data.isValid())
				{
					return data;
				}
			}

            size_t roleId;
            if (decodeRole( role, roleId ))
            {
                role = static_cast< int >( roleId );
            }

			return model_->headerData( section, orientation, role );
		}

		bool setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role ) override
		{
			if (model_ == nullptr)
			{
				return false;
			}

			for (auto & extension : extensions_)
			{
				if (extension->setHeaderData( section, orientation, value, role ) )
				{
					return true;
				}
			}

            size_t roleId;
            if (decodeRole( role, roleId ))
            {
                role = static_cast< int >( roleId );
            }

			return model_->setHeaderData( section, orientation, value, role );
		}

		QHash< int, QByteArray > roleNames() const override
		{
			return roleNames_;
		}

		void onDataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles )
		{
			QVector<int> encodedRoles;
			for (auto & role : roles)
			{
				int encodedRole;
				encodedRoles.append( encodeRole(role, encodedRole) ? encodedRole : role );
			}
			dataChanged( extendedIndex( topLeft ), extendedIndex( bottomRight ), encodedRoles );
		}

		void onLayoutAboutToBeChanged( const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint )
		{
			QList<QPersistentModelIndex> extendedParents;
			for (auto & parent : parents)
			{
				extendedParents.append( extendedIndex( parent ) );
			}
			layoutAboutToBeChanged( extendedParents, hint );
		}

		void onLayoutChanged( const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint )
		{
			QList<QPersistentModelIndex> extendedParents;
			for (auto & parent : parents)
			{
				extendedParents.append( extendedIndex( parent ) );
			}
			layoutChanged( extendedParents, hint );
		}

		void onRowsAboutToBeInserted( const QModelIndex &parent, int first, int last )
		{
			beginInsertRows( extendedIndex( parent ), first, last );
		}

		void onRowsInserted()
		{
			endInsertRows();
		}

		void onRowsAboutToBeRemoved( const QModelIndex &parent, int first, int last )
		{
			beginRemoveRows( extendedIndex( parent ), first, last );
		}

		void onRowsRemoved()
		{
			endRemoveRows();
		}

		QModelIndex modelIndex( const QModelIndex & extendedIndex ) const
		{
			if (!extendedIndex.isValid())
			{
				return QModelIndex();
			}

			assert( extendedIndex.model() == this );
			// To convert from an extended modelIndex to an internal modelIndex we have 2 options -
			// 1. Use the public index functions on model_ using the row and column of the extended modelIndex.
			//    The problem with this however is that this requires the parent of the extended modelIndex,
			//    however to calculate the parent of the extended modelIndex we first need to convert the 
			//    extended modelIndex to an internal modelIndex and we get stuck in a loop
			// 2. Create a map of internal persistentModelIndices to extended persistentModelIndices that is 
			//    populated by calls to extendedIndex and used as a lookup in modelIndex.
			//    The problem with this is that extendedIndex is called by the index function of the extended model
			//    and the index function is called whenever a remap of persistent indices is required (eg a call to
			//    beginRemoveRows/endRemoveRows). Instantiating a persistent index during a remap of a models
			//    persistent indices results in a corruption of the persistent index table and asserts during destruction
			// As such we need to resort to a third option -
			// 3. Access the protected createIndex function of model_ and call it directly.
			//    This is actually the most performant of all options and is completely safe with the current implementation
			//    of the QAbstractItemModel classes. All the createIndex function does is invoke the private constructor
			//    of QModelIndex, passing the models this pointer in as an argument.
			//    To do this we take the address of the extended models createIndex function and bind the this pointer
			//    to model_. Dodgy but necessary.
			QModelIndex (QAbstractItemModel::*createIndexFunc)(int, int, void*) const = &ExtendedModel::createIndex;
			using namespace std::placeholders;
			auto createIndex = std::bind( createIndexFunc, model_, _1, _2, _3 );
			return createIndex( extendedIndex.row(), extendedIndex.column(), extendedIndex.internalPointer() );
		}

		QModelIndex extendedIndex( const QModelIndex & modelIndex ) const
		{
			if (!modelIndex.isValid())
			{
				return QModelIndex();
			}

			assert( modelIndex.model() == model_ );
			return createIndex( modelIndex.row(), modelIndex.column(), modelIndex.internalId() );
		}

		QAbstractItemModel * model_;
		QtConnectionHolder connections_;
		QStringList & roles_;
		QList< IModelExtension * > & extensions_;
		QHash< int, QByteArray > roleNames_;
		ExtensionData extensionData_;
	};

	class HeaderData : public QObject
	{
	public:
		HeaderData( QAbstractItemModel & model, int section, Qt::Orientation orientation )
			: model_( model )
			, section_( section )
			, orientation_( orientation )
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
        ~HeaderData()
        {
            free( metaObject_ );
            metaObject_ = nullptr;
        }

	private:
		const QMetaObject * metaObject() const override
		{
			return metaObject_;
		}

		int qt_metacall( QMetaObject::Call c, int id, void **argv ) override
		{
			id = QObject::qt_metacall( c, id, argv );
			if (id < 0)
			{
				return id;
			}

			switch (c) 
			{
			case QMetaObject::InvokeMetaMethod:
				{
					auto methodCount = metaObject_->methodCount() - metaObject_->methodOffset();
					if (id < methodCount)
					{
						metaObject_->activate( this, id + metaObject_->methodOffset(), argv );
					}
					id -= methodCount;
					break;
				}
			case QMetaObject::ReadProperty:
			case QMetaObject::WriteProperty:
				{
					auto propertyCount = metaObject_->propertyCount() - metaObject_->propertyOffset();
					if (id < propertyCount)
					{
						auto value = reinterpret_cast< QVariant * >( argv[0] );
						auto role = roles_[id];
						if (c == QMetaObject::ReadProperty)
						{
							*value = model_.headerData( section_, orientation_, role );
						}
						else
						{
							model_.setHeaderData( section_, orientation_, *value, role );
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

		QAbstractItemModel & model_;
		int section_;
		Qt::Orientation orientation_;
		QList< int > roles_;
		QMetaObject* metaObject_;
	};
}

struct WGItemView::Impl
{
	Impl() : model_( nullptr ) {}

	QAbstractItemModel * model_;
	QList< IModelExtension * > extensions_;
	QStringList roles_;
	std::unique_ptr< ExtendedModel > extendedModel_;
	QList< QObject* > headerData_;
};

WGItemView::WGItemView()
	: impl_( new Impl() )
{
	impl_->extendedModel_.reset( new ExtendedModel( impl_->roles_, impl_->extensions_ ) );
}

WGItemView::~WGItemView()
{

}

QAbstractItemModel * WGItemView::getModel() const
{
	return impl_->model_;
}

void WGItemView::setModel( QAbstractItemModel * model )
{
	impl_->model_ = model;
	emit modelChanged();
	refresh();
}

QStringList WGItemView::getRoles() const
{
	return impl_->roles_;
}

void WGItemView::setRoles( const QStringList & roles )
{
	impl_->roles_ = roles;
	refresh();
}

QQmlListProperty< IModelExtension > WGItemView::getExtensions() const
{
	return QQmlListProperty< IModelExtension >(
		const_cast< WGItemView * >( this ),
		nullptr,
		&appendExtension, 
		&countExtensions,
		&extensionAt, 
		&clearExtensions );
}

void WGItemView::appendExtension( QQmlListProperty< IModelExtension > * property, IModelExtension * value )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return;
	}

	itemView->impl_->extensions_.append( value );
	itemView->refresh();
}

int WGItemView::countExtensions( QQmlListProperty< IModelExtension > * property )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return 0;
	}

	return itemView->impl_->extensions_.count();
}

IModelExtension * WGItemView::extensionAt( QQmlListProperty< IModelExtension > * property, int index )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return nullptr;
	}

	return itemView->impl_->extensions_[index];
}

void WGItemView::clearExtensions( QQmlListProperty< IModelExtension > * property )
{
	auto itemView = qobject_cast< WGItemView * >( property->object );
	if (itemView == nullptr)
	{
		return;
	}

	itemView->impl_->extensions_.clear();
	itemView->refresh();
}

QAbstractItemModel * WGItemView::getExtendedModel() const
{
	return impl_->extendedModel_.get();
}

QList< QObject* > WGItemView::getHeaderData() const
{
    return impl_->headerData_;
}

void WGItemView::refresh()
{
	impl_->extendedModel_->reset( impl_->model_ );

	//Enable for headers once body works.
	qDeleteAll( impl_->headerData_);
	impl_->headerData_.clear();
	if (impl_->extendedModel_ != nullptr)
	{
        int columnCount = getExtendedModel()->columnCount();
        for( int i = 0; i < columnCount; i++)
        {
		    impl_->headerData_.append( new HeaderData( *impl_->extendedModel_, i, Qt::Horizontal ) );
        }
	}
	emit headerDataChanged();
}

int WGItemView::getRow( const QModelIndex & index ) const
{
	return index.row();
}

int WGItemView::getColumn( const QModelIndex & index ) const
{
	return index.column();
}

QModelIndex WGItemView::getParent( const QModelIndex & index ) const
{
	return index.parent();
}
} // end namespace wgt
