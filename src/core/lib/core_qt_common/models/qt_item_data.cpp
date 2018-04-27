#include "qt_item_data.hpp"

#include <QModelIndex>
#include <QUuid>

#include <private/qmetaobjectbuilder_p.h>

namespace wgt
{
QtItemData::MetaObject::MetaObject(QAbstractItemModel& model) : model_(&model)
{
	QMetaObjectBuilder builder;
	builder.setClassName(QUuid().toByteArray());
	builder.setSuperClass(&QObject::staticMetaObject);

	auto addExtraProperty = [&builder](const char* name, const char* type) {
		auto property = builder.addProperty(name, type);
		property.setWritable(false);
		property.setConstant(true);
	};

	addExtraProperty("row", "QVariant");
	addExtraProperty("column", "QVariant");
	addExtraProperty("parent", "QVariant");

	QHashIterator<int, QByteArray> itr(model_->roleNames());
	while (itr.hasNext())
	{
		itr.next();
		roles_.append(itr.key());
		auto property = builder.addProperty(itr.value(), "QVariant");
		property.setNotifySignal(builder.addSignal(itr.value() + "Changed(QVariant)"));
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
	::free(metaObject_);
}

void QtItemData::MetaObject::destroy()
{
	model_ = nullptr;
}

QtItemData::QtItemData(const QModelIndex& index, std::shared_ptr<MetaObject> & metaObject)
    : index_( index )
	, metaObject_( metaObject )
{
}

QtItemData::~QtItemData()
{
}

const QModelIndex& QtItemData::index() const
{
	return index_;
}

const QMetaObject* QtItemData::metaObject() const
{
	auto && metaObject = metaObject_;
	if (metaObject == nullptr)
	{
		return nullptr;
	}

	return metaObject->metaObject_;
}

int QtItemData::qt_metacall(QMetaObject::Call c, int id, void** argv)
{
	auto && metaObject = metaObject_;
	if (metaObject == nullptr)
	{
		return -1;
	}

	id = QObject::qt_metacall(c, id, argv);
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
			metaObject->metaObject_->activate(this, id + metaObject->metaObject_->methodOffset(), argv);
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
			auto value = reinterpret_cast<QVariant*>(argv[0]);
			auto propertyId = id;
			if (propertyId-- == 0)
			{
				if (c == QMetaObject::ReadProperty)
				{
					*value = index_.row();
				}
			}
			if (propertyId-- == 0)
			{
				if (c == QMetaObject::ReadProperty)
				{
					*value = index_.column();
				}
			}
			if (propertyId-- == 0)
			{
				if (c == QMetaObject::ReadProperty)
				{
					*value = QVariant::fromValue(new QtItemData(index_.parent(), metaObject_));
				}
			}
			if (propertyId >= 0)
			{
				auto role = metaObject->roles_[id - 3];
				if (metaObject->model_ != nullptr)
				{
					if (c == QMetaObject::ReadProperty)
					{
						*value = metaObject->model_->data(index_, role);
					}
					else
					{
						metaObject->model_->setData(index_, *value, role);
					}
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

std::shared_ptr<QtItemData::MetaObject> QtItemData::getMetaObject(QAbstractItemModel& model)
{
	return std::make_shared<QtItemData::MetaObject>(model);
}
}