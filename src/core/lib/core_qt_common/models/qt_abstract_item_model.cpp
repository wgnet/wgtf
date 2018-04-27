#include "qt_abstract_item_model.hpp"
#include "qt_item_data.hpp"

#include <unordered_map>

namespace wgt
{

struct QtAbstractItemModel::Impl
{
	void resetItemToIndexCache()
	{
		itemToIndexCache_.clear();
	}

	QModelIndex findIndex(quintptr id, const QAbstractItemModel& model, QModelIndex parent = QModelIndex())
	{
		auto found = itemToIndexCache_.find(id);
		auto foundIndex = itemToIndexCache_[id];
		if(found != itemToIndexCache_.end())
		{
			return foundIndex;
		}
		auto rowCount = model.rowCount(parent);
		auto columnCount = model.columnCount(parent);
		if (rowCount == 0 || columnCount == 0)
		{
			return QModelIndex();
		}

		auto index = QModelIndex();
		auto row = rowCount;
		while (row-- != 0)
		{
			auto column = columnCount;
			while (column-- != 0)
			{
				index = index.isValid() ? index.sibling(row, column) : model.index(row, column, parent);
				if (index.internalId() == id)
				{
					itemToIndexCache_[id] = index;
					return index;
				}
			}
			auto childIndex = findIndex(id, model, index);
			if (childIndex.isValid())
			{
				return childIndex;
			}
		}

		return itemToIndexCache_[id] = QModelIndex();
	}

	std::shared_ptr<QtItemData::MetaObject> metaObject_;
	std::unordered_map<quintptr, QModelIndex> itemToIndexCache_;
};

QtAbstractItemModel::QtAbstractItemModel() : impl_(new Impl)
{
	QObject::connect(this, &QAbstractItemModel::modelReset, [&]() {
		impl_->metaObject_.reset();
		impl_->resetItemToIndexCache();
	});
	QObject::connect(this, &QAbstractItemModel::rowsInserted, [&]() { impl_->resetItemToIndexCache(); });
	QObject::connect(this, &QAbstractItemModel::rowsMoved, [&]() { impl_->resetItemToIndexCache(); });
	QObject::connect(this, &QAbstractItemModel::rowsRemoved, [&]() { impl_->resetItemToIndexCache(); });
	QObject::connect(this, &QAbstractItemModel::columnsInserted, [&]() { impl_->resetItemToIndexCache(); });
	QObject::connect(this, &QAbstractItemModel::columnsMoved, [&]() { impl_->resetItemToIndexCache(); });
	QObject::connect(this, &QAbstractItemModel::columnsRemoved, [&]() { impl_->resetItemToIndexCache(); });
	QObject::connect(this, &QAbstractItemModel::layoutAboutToBeChanged, [&]() { impl_->resetItemToIndexCache(); });
}

QtAbstractItemModel::~QtAbstractItemModel()
{
	if (impl_->metaObject_ != nullptr)
	{
		impl_->metaObject_->destroy();
	}
}

QModelIndex QtAbstractItemModel::findIndex(QVariant id) const
{
	return impl_->findIndex(id.value<quintptr>(), *this);
}


QModelIndexList QtAbstractItemModel::findIndexes(QVariantList ids) const
{
	QModelIndexList indexes;
	for (auto& id : ids)
	{
		indexes << findIndex(id);
	}
	return indexes;
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

QList<QObject*> QtAbstractItemModel::indexesToItems(const QModelIndexList& indexes) const
{
	QList<QObject*> items;
	for (auto& index : indexes)
	{
		items << indexToItem(index);
	}
	return items;
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

Qt::ItemFlags QtAbstractItemModel::flags(const QModelIndex& index) const
{
	return QAbstractItemModel::flags(index);
}

QtListModel::QtListModel()
{
}

QObject* QtListModel::item(int row) const
{
	return QtAbstractItemModel::item(row, 0, nullptr);
}

int QtListModel::count() const
{
	return QtAbstractItemModel::rowCount(nullptr);
}

bool QtListModel::insertItem(int row)
{
	return QtAbstractItemModel::insertRow(row, nullptr);
}

bool QtListModel::moveItem(int sourceRow, int destinationRow)
{
	return QtAbstractItemModel::moveRow(nullptr /* sourceParent */, sourceRow, nullptr /* destinationParent */,
	                                    destinationRow);
}

bool QtListModel::removeItem(int row)
{
	return QtAbstractItemModel::removeRow(row, nullptr);
}

QtTreeModel::QtTreeModel()
{
}

QObject* QtTreeModel::item(int row, QObject* parent) const
{
	return QtAbstractItemModel::item(row, 0, parent);
}

int QtTreeModel::count(QObject* parent) const
{
	return QtAbstractItemModel::rowCount(parent);
}

bool QtTreeModel::insertItem(int row, QObject* parent)
{
	return QtAbstractItemModel::insertRow(row, parent);
}

bool QtTreeModel::removeItem(int row, QObject* parent)
{
	return QtAbstractItemModel::removeRow(row, parent);
}

bool QtTreeModel::moveItem(QObject* sourceParent, int sourceRow, QObject* destinationParent, int destinationRow)
{
	return QtAbstractItemModel::moveRow(sourceParent, sourceRow, destinationParent, destinationRow);
}

QtTableModel::QtTableModel()
{
}

QObject* QtTableModel::item(int row, int column) const
{
	return QtAbstractItemModel::item(row, column, nullptr);
}

int QtTableModel::rowCount() const
{
	return QtAbstractItemModel::rowCount(nullptr);
}

int QtTableModel::columnCount() const
{
	return QtAbstractItemModel::columnCount(nullptr);
}

bool QtTableModel::insertRow(int row)
{
	return QtAbstractItemModel::insertRow(row, nullptr);
}

bool QtTableModel::insertColumn(int column)
{
	return QtAbstractItemModel::insertColumn(column, nullptr);
}

bool QtTableModel::removeRow(int row)
{
	return QtAbstractItemModel::removeRow(row, nullptr);
}

bool QtTableModel::removeColumn(int column)
{
	return QtAbstractItemModel::removeColumn(column, nullptr);
}

bool QtTableModel::moveRow(int sourceRow, int destinationRow)
{
	return QtAbstractItemModel::moveRow(nullptr /* sourceParent */, sourceRow, nullptr /* destinationParent */,
	                                    destinationRow);
}
} // end namespace wgt
