#include "wg_tree_list_adapter.hpp"
#include "qt_model_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/models/extensions/deprecated/i_model_extension_old.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
class WGTreeListAdapter::Impl
{
public:
	Impl();

	IQtFramework* qtFramework_;
	QPersistentModelIndex parentIndex_;
	QModelIndex removedParent_;
	QtModelHelpers::Extensions extensions_;
	QHash<int, QByteArray> roleNames_;
	QtConnectionHolder connections_;
};

WGTreeListAdapter::Impl::Impl() : qtFramework_(nullptr), extensions_()
{
}

WGTreeListAdapter::WGTreeListAdapter() : impl_(new Impl())
{
	impl_->qtFramework_ = Context::queryInterface<IQtFramework>();
}

WGTreeListAdapter::~WGTreeListAdapter()
{
	std::string modelName = this->objectName().toUtf8().constData();
	for (const auto& extension : impl_->extensions_)
	{
		extension->saveStates(modelName.c_str());
	}

	disconnect();
}

QAbstractItemModel* WGTreeListAdapter::parentModel() const
{
	return const_cast<QAbstractItemModel*>(impl_->parentIndex_.model());
}

QAbstractItemModel* WGTreeListAdapter::model() const
{
	auto model = static_cast<const QAbstractItemModel*>(this);
	return const_cast<QAbstractItemModel*>(model);
}

QHash<int, QByteArray> WGTreeListAdapter::roleNames() const
{
	impl_->roleNames_ = QAbstractListModel::roleNames();

	for (const auto& extension : impl_->extensions_)
	{
		QHashIterator<int, QByteArray> itr(extension->roleNames());

		while (itr.hasNext())
		{
			itr.next();
			impl_->roleNames_.insert(itr.key(), itr.value());
		}
	}
	return impl_->roleNames_;
}

QModelIndex WGTreeListAdapter::index(int row, int column, const QModelIndex& parent) const
{
	return IListAdapter::index(row, column, parent);
}

QModelIndex WGTreeListAdapter::adaptedIndex(int row, int column, const QModelIndex& parent) const
{
	auto m = parentModel();

	if (m == nullptr)
	{
		return QModelIndex();
	}

	QModelIndex index = impl_->parentIndex_;
	return m->index(row, column, index);
}

int WGTreeListAdapter::indexRow(const QModelIndex& index) const
{
	return index.row();
}

QVariant WGTreeListAdapter::getParentIndex() const
{
	QModelIndex index = impl_->parentIndex_;
	return QVariant::fromValue(index);
}

void WGTreeListAdapter::setParentIndex(const QVariant& index)
{
	beginResetModel();
	disconnect();

	impl_->parentIndex_ = index.toModelIndex();
	reset();

	connect();
	endResetModel();

	emit parentIndexChanged();
}

void WGTreeListAdapter::connect()
{
	auto model = parentModel();

	if (model == nullptr)
	{
		return;
	}

	impl_->connections_ +=
	QObject::connect(model, &QAbstractItemModel::dataChanged, this, &WGTreeListAdapter::onParentDataChanged);
	impl_->connections_ += QObject::connect(model, &QAbstractItemModel::layoutAboutToBeChanged, this,
	                                        &WGTreeListAdapter::onParentLayoutAboutToBeChanged);
	impl_->connections_ +=
	QObject::connect(model, &QAbstractItemModel::layoutChanged, this, &WGTreeListAdapter::onParentLayoutChanged);
	impl_->connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this,
	                                        &WGTreeListAdapter::onParentRowsAboutToBeInserted);
	impl_->connections_ +=
	QObject::connect(model, &QAbstractItemModel::rowsInserted, this, &WGTreeListAdapter::onParentRowsInserted);
	impl_->connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this,
	                                        &WGTreeListAdapter::onParentRowsAboutToBeRemoved);
	impl_->connections_ +=
	QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, &WGTreeListAdapter::onParentRowsRemoved);
}

void WGTreeListAdapter::disconnect()
{
	impl_->connections_.reset();
}

void WGTreeListAdapter::registerExtension(IModelExtensionOld* extension)
{
	beginResetModel();
	extension->init(impl_->qtFramework_);
	std::string modelName = this->objectName().toUtf8().constData();
	extension->loadStates(modelName.c_str());
	impl_->extensions_.emplace_back(extension);
	endResetModel();
}

int WGTreeListAdapter::rowCount(const QModelIndex& parent) const
{
	if (parent.parent().isValid())
	{
		return 0;
	}

	auto m = parentModel();

	if (m == nullptr)
	{
		return 0;
	}

	return m->rowCount(impl_->parentIndex_);
}

int WGTreeListAdapter::columnCount(const QModelIndex& index) const
{
	auto m = parentModel();

	if (m == nullptr)
	{
		return 0;
	}

	return m->columnCount(impl_->parentIndex_);
}

QVariant WGTreeListAdapter::data(const QModelIndex& index, int role) const
{
	if (parentModel() == nullptr)
	{
		return QVariant(QVariant::Invalid);
	}

	assert(index.isValid());

	if (role == Qt::DisplayRole || role == Qt::DecorationRole)
	{
		return parentModel()->data(index, role);
	}

	if (role < Qt::UserRole)
	{
		return QVariant(QVariant::Invalid);
	}

	for (const auto& extension : impl_->extensions_)
	{
		auto data = extension->data(index, role);
		if (data.isValid())
		{
			return data;
		}
	}

	return QVariant(QVariant::Invalid);
}

bool WGTreeListAdapter::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (parentModel() == nullptr)
	{
		return false;
	}

	for (auto& extension : impl_->extensions_)
	{
		if (extension->setData(index, value, role))
		{
			return true;
		}
	}

	return false;
}

QQmlListProperty<IModelExtensionOld> WGTreeListAdapter::getExtensions() const
{
	return QQmlListProperty<IModelExtensionOld>(const_cast<WGTreeListAdapter*>(this), nullptr, &appendExtension,
	                                            &countExtensions, &extensionAt, &clearExtensions);
}

void WGTreeListAdapter::appendExtension(QQmlListProperty<IModelExtensionOld>* property, IModelExtensionOld* value)
{
	auto treeListAdapter = qobject_cast<WGTreeListAdapter*>(property->object);
	if (treeListAdapter == nullptr)
	{
		return;
	}

	treeListAdapter->registerExtension(value);
}

IModelExtensionOld* WGTreeListAdapter::extensionAt(QQmlListProperty<IModelExtensionOld>* property, int index)
{
	auto treeListAdapter = qobject_cast<WGTreeListAdapter*>(property->object);
	if (treeListAdapter == nullptr)
	{
		return nullptr;
	}

	return treeListAdapter->impl_->extensions_[index];
}

void WGTreeListAdapter::clearExtensions(QQmlListProperty<IModelExtensionOld>* property)
{
	auto treeListAdapter = qobject_cast<WGTreeListAdapter*>(property->object);
	if (treeListAdapter == nullptr)
	{
		return;
	}

	treeListAdapter->beginResetModel();
	treeListAdapter->impl_->extensions_.clear();
	treeListAdapter->endResetModel();
}

int WGTreeListAdapter::countExtensions(QQmlListProperty<IModelExtensionOld>* property)
{
	auto treeListAdapter = qobject_cast<WGTreeListAdapter*>(property->object);
	if (treeListAdapter == nullptr)
	{
		return 0;
	}

	return static_cast<int>(treeListAdapter->impl_->extensions_.size());
}

void WGTreeListAdapter::onParentDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                            const QVector<int>& roles)
{
	if (topLeft.parent() == impl_->parentIndex_ && bottomRight.parent() == impl_->parentIndex_)
	{
		emit dataChanged(createIndex(topLeft.row(), topLeft.column()),
		                 createIndex(bottomRight.row(), bottomRight.column()), roles);
	}
}

void WGTreeListAdapter::onParentLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                                       QAbstractItemModel::LayoutChangeHint hint)
{
	auto resetLayout = parents.empty();
	if (!resetLayout)
	{
		auto item = static_cast<QModelIndex>(impl_->parentIndex_).internalPointer();
		for (auto it = parents.cbegin(); it != parents.cend(); ++it)
		{
			if (static_cast<QModelIndex>(*it).internalPointer() == item)
			{
				resetLayout = true;
				break;
			}
		}
	}

	if (resetLayout)
	{
		reset();
		emit layoutAboutToBeChanged();
	}
}

void WGTreeListAdapter::onParentLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                              QAbstractItemModel::LayoutChangeHint hint)
{
	auto resetLayout = parents.empty();
	if (!resetLayout)
	{
		auto item = static_cast<QModelIndex>(impl_->parentIndex_).internalPointer();
		for (auto it = parents.cbegin(); it != parents.cend(); ++it)
		{
			if (static_cast<QModelIndex>(*it).internalPointer() == item)
			{
				resetLayout = true;
				break;
			}
		}
	}

	if (resetLayout)
	{
		emit layoutChanged();
	}
}

void WGTreeListAdapter::onParentRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	if (parent != impl_->parentIndex_)
	{
		return;
	}

	beginInsertRows(QModelIndex(), first, last);
}

void WGTreeListAdapter::onParentRowsInserted(const QModelIndex& parent, int first, int last)
{
	if (parent != impl_->parentIndex_)
	{
		return;
	}

	reset();
	endInsertRows();
}

void WGTreeListAdapter::onParentRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	if (parent != impl_->parentIndex_)
	{
		return;
	}

	impl_->removedParent_ = impl_->parentIndex_;
	beginRemoveRows(QModelIndex(), first, last);
}

void WGTreeListAdapter::onParentRowsRemoved(const QModelIndex& parent, int first, int last)
{
	if (impl_->removedParent_ == QModelIndex())
	{
		return;
	}

	impl_->removedParent_ = QModelIndex();
	reset();
	endRemoveRows();
}
} // end namespace wgt
