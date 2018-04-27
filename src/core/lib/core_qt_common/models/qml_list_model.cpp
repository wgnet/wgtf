#include "qml_list_model.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "qml_item_model_helper.inl"

namespace wgt
{
struct QmlListModel::Implementation : public QmlItemModelHelper
{
	Implementation(QmlListModel& self) : QmlItemModelHelper(self, self.QtListModel::roleNames())
	{
	}

	void addRow(QmlModelRow* row)
	{
		int index = rows_.count();
		auto& self = static_cast<QmlListModel&>(self_);
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
		auto& self = static_cast<QmlListModel&>(self_);
		self.beginResetModel();

		for (auto& connection : columnConnections_)
		{
			connection.reset();
		}

		columnConnections_.clear();
		rows_.clear();
		roleNames_ = self.QtListModel::roleNames();
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

		auto& self = static_cast<QmlListModel&>(self_);
		self.beginResetModel();
		QmlItemModelHelper::flushPendingChanges();
		self.endResetModel();
	}

	QVector<QmlModelRow*> rows_;
	std::vector<QtConnectionHolder> columnConnections_;
};

QmlListModel::QmlListModel() : impl_(new Implementation(*this))
{
}

QmlListModel::~QmlListModel()
{
}

QModelIndex QmlListModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return QModelIndex();
	}

	return this->createIndex(row, column, nullptr);
}

QModelIndex QmlListModel::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int QmlListModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return 0;
	}

	return impl_->rows_.count();
}

int QmlListModel::columnCount(const QModelIndex& parent) const
{
	return impl_->columnCount_;
}

bool QmlListModel::hasChildren(const QModelIndex& parent) const
{
	return false;
}

QVariant QmlListModel::data(const QModelIndex& index, int role) const
{
	const bool invalidRow = index.row() < 0 || index.row() > impl_->rows_.count();

	if (invalidRow)
	{
		return QVariant::Invalid;
	}

	return impl_->data(impl_->rows_[index.row()], index.column(), role);
}

bool QmlListModel::setData(const QModelIndex& index, const QVariant& value, int role)
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

QVariant QmlListModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool QmlListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (impl_->header_ == nullptr)
	{
		return false;
	}

	if (orientation != Qt::Orientation::Horizontal)
	{
		return false;
	}

	if (!impl_->setData(impl_->header_, section, role, value))
	{
		return false;
	}

	headerDataChanged(orientation, section, section);
	return true;
}

QHash<int, QByteArray> QmlListModel::roleNames() const
{
	return impl_->roleNames_;
}

QmlModelRow* QmlListModel::header() const
{
	return impl_->header_;
}

void QmlListModel::setHeader(QmlModelRow* row)
{
	if (row == impl_->header_)
	{
		return;
	}

	impl_->setHeader(row);
	headerChanged();
}

QQmlListProperty<QmlModelRow> QmlListModel::rows() const
{
	return QQmlListProperty<QmlModelRow>(const_cast<QmlListModel*>(this), nullptr, &appendRows, &countRows, &rowAt,
	                                     &clearRows);
}

void QmlListModel::appendRows(QQmlListProperty<QmlModelRow>* property, QmlModelRow* row)
{
	auto model = qobject_cast<QmlListModel*>(property->object);

	if (model == nullptr)
	{
		return;
	}

	model->impl_->addRow(row);
}

int QmlListModel::countRows(QQmlListProperty<QmlModelRow>* property)
{
	auto model = qobject_cast<QmlListModel*>(property->object);

	if (model == nullptr)
	{
		return 0;
	}

	return model->impl_->rows_.count();
}

QmlModelRow* QmlListModel::rowAt(QQmlListProperty<QmlModelRow>* property, int index)
{
	auto model = qobject_cast<QmlListModel*>(property->object);

	if (model == nullptr)
	{
		return nullptr;
	}

	return model->impl_->rows_.at(index);
}

void QmlListModel::clearRows(QQmlListProperty<QmlModelRow>* property)
{
	auto model = qobject_cast<QmlListModel*>(property->object);

	if (model == nullptr)
	{
		return;
	}

	model->impl_->removeRows();
}
} // end namespace wgt
