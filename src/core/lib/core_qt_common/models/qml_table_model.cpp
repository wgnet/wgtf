#include "qml_table_model.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "qml_item_model_helper.inl"

namespace wgt
{
struct QmlTableModel::Implementation : public QmlItemModelHelper
{
	Implementation(QmlTableModel& self) : QmlItemModelHelper(self, self.QtTableModel::roleNames())
	{
	}

	void addRow(QmlModelRow* row)
	{
		int index = rows_.count();
		auto& self = static_cast<QmlTableModel&>(self_);
		self.beginInsertRows(QModelIndex(), index, index);
		rows_.append(row);
		self.endInsertRows();

		columnConnections_.push_back(QtConnectionHolder());
		addRowConnections(row, columnConnections_.back());

		addRoles(row);
		flushPendingChanges();
	}

	void removeRows()
	{
		auto& self = static_cast<QmlTableModel&>(self_);
		self.beginResetModel();

		for (auto& connection : columnConnections_)
		{
			connection.reset();
		}

		columnConnections_.clear();
		rows_.clear();
		roleNames_ = self.QmlTableModel::roleNames();
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

		auto& self = static_cast<QmlTableModel&>(self_);
		self.beginResetModel();
		QmlItemModelHelper::flushPendingChanges();
		self.endResetModel();
	}

	QVector<QmlModelRow*> rows_;
	std::vector<QtConnectionHolder> columnConnections_;
};

QmlTableModel::QmlTableModel() : impl_(new Implementation(*this))
{
}

QmlTableModel::~QmlTableModel()
{
}

QModelIndex QmlTableModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return QModelIndex();
	}

	return this->createIndex(row, column, nullptr);
}

QModelIndex QmlTableModel::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int QmlTableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return 0;
	}

	return impl_->rows_.count();
}

int QmlTableModel::columnCount(const QModelIndex& parent) const
{
	return impl_->columnCount_;
}

bool QmlTableModel::hasChildren(const QModelIndex& parent) const
{
	return false;
}

QVariant QmlTableModel::data(const QModelIndex& index, int role) const
{
	const bool invalidRow = index.row() < 0 || index.row() > impl_->rows_.count();

	if (invalidRow)
	{
		return QVariant::Invalid;
	}

	return impl_->data(impl_->rows_[index.row()], index.column(), role);
}

bool QmlTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	const bool invalidRow = index.row() < 0 || index.row() > impl_->rows_.count();

	if (invalidRow)
	{
		return false;
	}

	if (!impl_->setData(impl_->rows_[index.row()], index.column(), role, value))
	{
		return false;
	}

	dataChanged(index, index);
	return true;
}

QVariant QmlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool QmlTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
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

QHash<int, QByteArray> QmlTableModel::roleNames() const
{
	return impl_->roleNames_;
}

QmlModelRow* QmlTableModel::header() const
{
	return impl_->header_;
}

void QmlTableModel::setHeader(QmlModelRow* row)
{
	if (row == impl_->header_)
	{
		return;
	}

	impl_->setHeader(row);
	headerChanged();
}

QQmlListProperty<QmlModelRow> QmlTableModel::rows() const
{
	return QQmlListProperty<QmlModelRow>(const_cast<QmlTableModel*>(this), nullptr, &appendRows, &countRows, &rowAt,
	                                     &clearRows);
}

void QmlTableModel::appendRows(QQmlListProperty<QmlModelRow>* property, QmlModelRow* row)
{
	auto model = qobject_cast<QmlTableModel*>(property->object);

	if (model == nullptr)
	{
		return;
	}

	model->impl_->addRow(row);
}

int QmlTableModel::countRows(QQmlListProperty<QmlModelRow>* property)
{
	auto model = qobject_cast<QmlTableModel*>(property->object);

	if (model == nullptr)
	{
		return 0;
	}

	return model->impl_->rows_.count();
}

QmlModelRow* QmlTableModel::rowAt(QQmlListProperty<QmlModelRow>* property, int index)
{
	auto model = qobject_cast<QmlTableModel*>(property->object);

	if (model == nullptr)
	{
		return nullptr;
	}

	return model->impl_->rows_.at(index);
}

void QmlTableModel::clearRows(QQmlListProperty<QmlModelRow>* property)
{
	auto model = qobject_cast<QmlTableModel*>(property->object);

	if (model == nullptr)
	{
		return;
	}

	model->impl_->removeRows();
}
} // end namespace wgt
