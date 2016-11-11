#include "qt_abstract_item_model.hpp"
#include "qt_item_data.hpp"

namespace wgt
{
struct QtAbstractItemModel::Impl
{
	std::shared_ptr<QtItemData::MetaObject> metaObject_;
};

QtAbstractItemModel::QtAbstractItemModel() : impl_(new Impl)
{
	QObject::connect(this, &QAbstractItemModel::modelReset, [&]() { impl_->metaObject_.reset(); });
}

QtAbstractItemModel::~QtAbstractItemModel()
{
}

QModelIndex QtAbstractItemModel::itemToIndex(QObject* item) const
{
	auto itemData = qobject_cast<const QtItemData*>(item);
	return item != nullptr ? itemData->index() : QModelIndex();
}

QObject* QtAbstractItemModel::indexToItem(const QModelIndex& index) const
{
	if (impl_->metaObject_ == nullptr)
	{
		impl_->metaObject_ = QtItemData::getMetaObject(const_cast<QtAbstractItemModel&>(*this));
	}
	return index.isValid() ? new QtItemData(index, impl_->metaObject_) : nullptr;
}

QObject* QtAbstractItemModel::item(int row, int column, QObject* parent) const
{
	auto parentIndex = itemToIndex(parent);
	auto index = this->index(row, column, parentIndex);
	return indexToItem(index);
}

QObject* QtAbstractItemModel::parent(QObject* child) const
{
	auto childIndex = itemToIndex(child);
	auto index = parent(childIndex);
	return indexToItem(index);
}

int QtAbstractItemModel::rowIndex(QObject* item) const
{
	auto index = itemToIndex(item);
	return index.row();
}

int QtAbstractItemModel::columnIndex(QObject* item) const
{
	auto index = itemToIndex(item);
	return index.column();
}

int QtAbstractItemModel::rowCount(QObject* parent) const
{
	auto parentIndex = itemToIndex(parent);
	return rowCount(parentIndex);
}

int QtAbstractItemModel::columnCount(QObject* parent) const
{
	auto parentIndex = itemToIndex(parent);
	return columnCount(parentIndex);
}

bool QtAbstractItemModel::hasChildren(QObject* parent) const
{
	auto parentIndex = itemToIndex(parent);
	return hasChildren(parentIndex);
}

bool QtAbstractItemModel::insertRow(int row, QObject* parent)
{
	auto parentIndex = itemToIndex(parent);
	return QAbstractItemModel::insertRow(row, parentIndex);
}

bool QtAbstractItemModel::insertColumn(int column, QObject* parent)
{
	auto parentIndex = itemToIndex(parent);
	return QAbstractItemModel::insertColumn(column, parentIndex);
}

bool QtAbstractItemModel::removeRow(int row, QObject* parent)
{
	auto parentIndex = itemToIndex(parent);
	return QAbstractItemModel::removeRow(row, parentIndex);
}

bool QtAbstractItemModel::removeColumn(int column, QObject* parent)
{
	auto parentIndex = itemToIndex(parent);
	return QAbstractItemModel::removeColumn(column, parentIndex);
}

bool QtAbstractItemModel::moveRow(QObject* sourceParent, int sourceRow, QObject* destinationParent,
                                  int destinationChild)
{
	const auto sourceParentIndex = itemToIndex(sourceParent);
	const auto destinationParentIndex = itemToIndex(destinationParent);
	const auto result =
	QAbstractItemModel::moveRow(sourceParentIndex, sourceRow, destinationParentIndex, destinationChild);
	return result;
}

bool QtAbstractItemModel::hasChildren(const QModelIndex& parent) const
{
	return QAbstractItemModel::hasChildren(parent);
}
} // end namespace wgt
