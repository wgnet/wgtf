#include "qml_tree_model.hpp"

#include "core_common/assert.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "qml_item_model_helper.inl"

namespace wgt
{
struct QmlTreeModel::Implementation : public QmlItemModelHelper
{
	Implementation(QmlTreeModel& self) : QmlItemModelHelper(self, self.QtTreeModel::roleNames())
	{
	}

	void addRowConnections(QmlModelRow* row, QtConnectionHolder& connections) override
	{
		QmlItemModelHelper::addRowConnections(row, connections);

		if (row->parent() != nullptr)
		{
			return;
		}

		auto addingChildCallback = [this](QmlModelRow* row) { this->preAddRow(row); };

		auto childAddedCallback = [this](QmlModelRow* row) { this->addRow(row); };

		auto clearingChildrenCallback = [this](QmlModelRow* parent) { this->preRemoveChildRows(parent); };

		auto childrenClearedCallback = [this](QmlModelRow* parent) { this->postRemoveChildRows(); };

		connections += self_.connect(row, &QmlModelRow::addingChild, addingChildCallback);
		connections += self_.connect(row, &QmlModelRow::childAdded, childAddedCallback);
		connections += self_.connect(row, &QmlModelRow::clearingChildren, clearingChildrenCallback);
		connections += self_.connect(row, &QmlModelRow::childrenCleared, childrenClearedCallback);
	}

	void preAddRow(QmlModelRow* row)
	{
		QmlModelRow* parent = row->parent();
		int index = parent == nullptr ? rows_.count() : parent->children().count() - 1;
		auto& self = static_cast<QmlTreeModel&>(self_);
		self.beginInsertRows(calculateIndex(parent), index, index);
	}

	void addRow(QmlModelRow* row)
	{
		if (row->parent() == nullptr)
		{
			rows_.append(row);
		}

		auto& self = static_cast<QmlTreeModel&>(self_);
		self.endInsertRows();

		columnConnections_.push_back(QtConnectionHolder());
		addRowConnections(row, columnConnections_.back());

		addRoles(row);
		flushPendingChanges();
	}

	QModelIndex calculateIndex(QmlModelRow* row)
	{
		if (row == nullptr)
		{
			return QModelIndex();
		}

		QmlModelRow* parent = row->parent();
		auto& children = parent == nullptr ? rows_ : parent->children();
		int index = 0;
		bool found = false;

		for (index = 0; index < children.count(); ++index)
		{
			if (row == children[index])
			{
				found = true;
				break;
			}
		}

		TF_ASSERT(found);
		auto& self = static_cast<QmlTreeModel&>(self_);
		return self.createIndex(index, 0, row);
	}

	void preRemoveChildRows(QmlModelRow* row)
	{
		auto& self = static_cast<QmlTreeModel&>(self_);
		self.beginRemoveRows(calculateIndex(row), 0, row->children().count());
	}

	void postRemoveChildRows()
	{
		auto& self = static_cast<QmlTreeModel&>(self_);
		self.endRemoveRows();
	}

	void removeRows()
	{
		auto& self = static_cast<QmlTreeModel&>(self_);
		self.beginResetModel();

		for (auto& connection : columnConnections_)
		{
			connection.reset();
		}

		columnConnections_.clear();
		rowConnections_ = QVector<QtConnectionHolder>();
		rows_.clear();
		roleNames_ = self.QmlTreeModel::roleNames();
		columnCount_ = 0;
		resetPendingChanges();
		self.endResetModel();
	}

	void flushPendingChanges() override
	{
		if (!pendingChanges())
		{
			return;
		}

		auto& self = static_cast<QmlTreeModel&>(self_);
		self.beginResetModel();
		QmlItemModelHelper::flushPendingChanges();
		self.endResetModel();
	}

	QVector<QmlModelRow*> rows_;
	std::vector<QtConnectionHolder> columnConnections_;
	QVector<QtConnectionHolder> rowConnections_;
};

QmlTreeModel::QmlTreeModel() : impl_(new Implementation(*this))
{
}

QmlTreeModel::~QmlTreeModel()
{
}

QModelIndex QmlTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0)
	{
		return QModelIndex();
	}

	const QVector<QmlModelRow*>& rows =
	parent.isValid() ? reinterpret_cast<QmlModelRow*>(parent.internalPointer())->children() : impl_->rows_;

	if (row >= rows.count())
	{
		return QModelIndex();
	}

	return this->createIndex(row, column, rows[row]);
}

QModelIndex QmlTreeModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}

	QmlModelRow* row = reinterpret_cast<QmlModelRow*>(child.internalPointer());
	return impl_->calculateIndex(row->parent());
}

int QmlTreeModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		return impl_->rows_.count();
	}

	QmlModelRow* row = reinterpret_cast<QmlModelRow*>(parent.internalPointer());
	return row->children().count();
}

int QmlTreeModel::columnCount(const QModelIndex& parent) const
{
	return impl_->columnCount_;
}

bool QmlTreeModel::hasChildren(const QModelIndex& parent) const
{
	return rowCount(parent) > 0;
}

QVariant QmlTreeModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0)
	{
		return QVariant::Invalid;
	}

	QmlModelRow* row = reinterpret_cast<QmlModelRow*>(index.internalPointer());
	return impl_->data(row, index.column(), role);
}

bool QmlTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.row() < 0)
	{
		return false;
	}

	QmlModelRow* row = reinterpret_cast<QmlModelRow*>(index.internalPointer());

	if (!impl_->setData(row, index.column(), role, value))
	{
		return false;
	}

	dataChanged(index, index);
	return true;
}

QVariant QmlTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (impl_->header_ == nullptr)
	{
		return QVariant::Invalid;
	}

	if (orientation != Qt::Orientation::Horizontal)
	{
		return QVariant::Invalid;
	}

	return impl_->data(impl_->header_, section, role);
}

bool QmlTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (impl_->header_ == nullptr)
	{
		return false;
	}

	if (orientation != Qt::Orientation::Horizontal)
	{
		return QVariant::Invalid;
	}

	if (!impl_->setData(impl_->header_, section, role, value))
	{
		return false;
	}

	headerDataChanged(orientation, section, section);
	return true;
}

QHash<int, QByteArray> QmlTreeModel::roleNames() const
{
	return impl_->roleNames_;
}

QmlModelRow* QmlTreeModel::header() const
{
	return impl_->header_;
}

void QmlTreeModel::setHeader(QmlModelRow* row)
{
	if (row == impl_->header_)
	{
		return;
	}

	impl_->setHeader(row);
	headerChanged();
}

QQmlListProperty<QmlModelRow> QmlTreeModel::rows() const
{
	return QQmlListProperty<QmlModelRow>(const_cast<QmlTreeModel*>(this), nullptr, &appendRows, &countRows, &rowAt,
	                                     &clearRows);
}

void QmlTreeModel::appendRows(QQmlListProperty<QmlModelRow>* property, QmlModelRow* row)
{
	auto model = qobject_cast<QmlTreeModel*>(property->object);

	if (model == nullptr)
	{
		return;
	}

	model->impl_->preAddRow(row);
	model->impl_->addRow(row);
}

int QmlTreeModel::countRows(QQmlListProperty<QmlModelRow>* property)
{
	auto model = qobject_cast<QmlTreeModel*>(property->object);

	if (model == nullptr)
	{
		return 0;
	}

	return model->impl_->rows_.count();
}

QmlModelRow* QmlTreeModel::rowAt(QQmlListProperty<QmlModelRow>* property, int index)
{
	auto model = qobject_cast<QmlTreeModel*>(property->object);

	if (model == nullptr)
	{
		return nullptr;
	}

	return model->impl_->rows_.at(index);
}

void QmlTreeModel::clearRows(QQmlListProperty<QmlModelRow>* property)
{
	auto model = qobject_cast<QmlTreeModel*>(property->object);

	if (model == nullptr)
	{
		return;
	}

	model->impl_->removeRows();
}
} // end namespace wgt
