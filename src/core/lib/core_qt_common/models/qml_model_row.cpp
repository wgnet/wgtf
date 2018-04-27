#include "qml_model_row.hpp"

namespace wgt
{
QmlModelRow::QmlModelRow() : parent_(nullptr)
{
}

QmlModelRow::~QmlModelRow()
{
}

const QVector<QmlModelData*>& QmlModelRow::columns() const
{
	return columns_;
}

const QVector<QmlModelRow*>& QmlModelRow::children() const
{
	return children_;
}

QmlModelRow* QmlModelRow::parent() const
{
	return parent_;
}

QQmlListProperty<QmlModelData> QmlModelRow::columnsProperty() const
{
	return QQmlListProperty<QmlModelData>(const_cast<QmlModelRow*>(this), nullptr, &appendColumns, &countColumns,
	                                      &columnAt, &clearColumns);
}

void QmlModelRow::appendColumns(QQmlListProperty<QmlModelData>* property, QmlModelData* column)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return;
	}

	int index = row->columns_.count();
	auto callback = [row, index, column]() { row->columnChanged(index, column); };

	row->columns_.append(column);
	row->columnAdded(index, column);
	row->columnConnections_ += row->connect(column, &QmlModelData::rolesChanged, callback);
}

int QmlModelRow::countColumns(QQmlListProperty<QmlModelData>* property)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return 0;
	}

	return row->columns_.count();
}

QmlModelData* QmlModelRow::columnAt(QQmlListProperty<QmlModelData>* property, int index)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return nullptr;
	}

	return row->columns_.at(index);
}

void QmlModelRow::clearColumns(QQmlListProperty<QmlModelData>* property)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return;
	}

	row->columnConnections_.reset();
	row->columns_.clear();
	row->columnsCleared();
}

QQmlListProperty<QmlModelRow> QmlModelRow::childrenProperty() const
{
	return QQmlListProperty<QmlModelRow>(const_cast<QmlModelRow*>(this), nullptr, &appendChildren, &countChildren,
	                                     &childAt, &clearChildren);
}

void QmlModelRow::appendChildren(QQmlListProperty<QmlModelRow>* property, QmlModelRow* child)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return;
	}

	auto columnAddedCallback = [row](int index, QmlModelData* column) { row->columnAdded(index, column); };

	auto columnChangedCallback = [row](int index, QmlModelData* column) { row->columnChanged(index, column); };

	row->rowConnections_ += row->connect(child, &QmlModelRow::columnAdded, columnAddedCallback);
	row->rowConnections_ += row->connect(child, &QmlModelRow::columnChanged, columnChangedCallback);

	child->parent_ = row;
	row->addingChild(child);
	row->children_.append(child);
	row->childAdded(child);

	auto addingChildCallback = [row](QmlModelRow* child) { row->addingChild(child); };

	auto childAddedCallback = [row](QmlModelRow* child) { row->childAdded(child); };

	auto clearingChildrenCallback = [row](QmlModelRow* parent) { row->clearingChildren(parent); };

	auto childrenClearedCallback = [row](QmlModelRow* parent) { row->childrenCleared(parent); };

	row->rowConnections_ += row->connect(child, &QmlModelRow::addingChild, addingChildCallback);
	row->rowConnections_ += row->connect(child, &QmlModelRow::childAdded, childAddedCallback);
	row->rowConnections_ += row->connect(child, &QmlModelRow::clearingChildren, clearingChildrenCallback);
	row->rowConnections_ += row->connect(child, &QmlModelRow::childrenCleared, childrenClearedCallback);
}

int QmlModelRow::countChildren(QQmlListProperty<QmlModelRow>* property)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return 0;
	}

	return row->children_.count();
}

QmlModelRow* QmlModelRow::childAt(QQmlListProperty<QmlModelRow>* property, int index)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return nullptr;
	}

	return row->children_.at(index);
}

void QmlModelRow::clearChildren(QQmlListProperty<QmlModelRow>* property)
{
	auto row = qobject_cast<QmlModelRow*>(property->object);

	if (row == nullptr)
	{
		return;
	}

	row->clearingChildren(row);
	row->rowConnections_.reset();
	row->children_.clear();
	row->childrenCleared(row);
}
} // end namespace wgt
