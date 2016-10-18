#include "qt_item_data.hpp"

#include <QModelIndex>
#include <QUuid>

#include <private/qmetaobjectbuilder_p.h>

namespace wgt
{

QtItemData::MetaObject::MetaObject( QAbstractItemModel & model )
	: model_( model )
{
	QMetaObjectBuilder builder;
	builder.setClassName( QUuid().toByteArray() );
	builder.setSuperClass( &QObject::staticMetaObject );

	builder.addProperty( "row", "QVariant" );
	builder.addProperty( "column", "QVariant" );
	builder.addProperty( "parent", "QVariant" );

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

QtItemData::MetaObject::~MetaObject()
{
	// @see QMetaObjectBuilder::toMetaObject()
	// "The return value should be deallocated using free() once it
	// is no longer needed."
	// Allocation was done by the Qt dll, so use global free()
	// Do not use the NGT allocator
	::free( metaObject_ );
}

struct QtItemData::Impl
{
	Impl( const QModelIndex & index, std::weak_ptr< MetaObject > & metaObject )
		: index_( index )
		, metaObject_( metaObject )
	{
	}

	QModelIndex index_;
	std::weak_ptr< MetaObject > metaObject_;
};

QtItemData::QtItemData( const QModelIndex & index, std::weak_ptr< MetaObject > metaObject )
	: impl_( new Impl( index, metaObject ) )
{
}

const QModelIndex & QtItemData::index() const
{
	return impl_->index_;
}

const QMetaObject * QtItemData::metaObject() const
{
	auto metaObject = impl_->metaObject_.lock();
	if (metaObject == nullptr)
	{
		return nullptr;
	}

	return metaObject->metaObject_;
}

int QtItemData::qt_metacall( QMetaObject::Call c, int id, void **argv )
{
	auto metaObject = impl_->metaObject_.lock();
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
				auto propertyId = id;
				if (propertyId-- == 0)
				{
					if (c == QMetaObject::ReadProperty)
					{
						*value = impl_->index_.row();
					}
				}
				if (propertyId-- == 0)
				{
					if (c == QMetaObject::ReadProperty)
					{
						*value = impl_->index_.column();
					}
				}
				if (propertyId-- == 0)
				{
					if (c == QMetaObject::ReadProperty)
					{
						*value = QVariant::fromValue( new QtItemData( impl_->index_.parent(), impl_->metaObject_ ) );
					}
				}
				if (propertyId >= 0)
				{
					auto role = metaObject->roles_[id - 3];
					if (c == QMetaObject::ReadProperty)
					{
						*value = metaObject->model_.data( impl_->index_, role );
					}
					else
					{
						metaObject->model_.setData( impl_->index_, *value, role );
					}
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

std::shared_ptr< QtItemData::MetaObject > QtItemData::getMetaObject( QAbstractItemModel & model )
{
	return std::make_shared< QtItemData::MetaObject >( model );
}
}