#include "wg_tree_model.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/models/extensions/deprecated/i_model_extension_old.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_qt_common/qt_image_provider_old.hpp"
#include "qt_model_helpers.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_logging/logging.hpp"

#include <QApplication>
#include <QThread>

namespace wgt
{
class WGTreeModel::Impl : public Depends<IQtFramework, IQtHelpers>
{
public:
	Impl();
	static QModelIndex calculateModelIndex(const WGTreeModel& self, const IItem* pItem, int column);

	ITreeModel* model_;
	QVariant source_;
	QtModelHelpers::Extensions extensions_;
	QtConnectionHolder qtConnections_;
	ConnectionHolder connections_;
	QHash<int, QByteArray> roleNames_;
	QHash<int, QByteArray> defaultRoleNames_;
};

WGTreeModel::Impl::Impl() : model_(nullptr)
{
}

QModelIndex WGTreeModel::Impl::calculateModelIndex(const WGTreeModel& self, const IItem* item, int column)
{
	ITreeModel* model = self.getModel();
	if (item == nullptr || model == nullptr)
	{
		return QModelIndex();
	}

	auto itemIndex = model->index(item);
	const int row = static_cast<int>(itemIndex.first);
	return self.createIndex(row, column, const_cast<IItem*>(item));
}

WGTreeModel::WGTreeModel() : impl_(new Impl())
{
	impl_->defaultRoleNames_ = QAbstractItemModel::roleNames();
	impl_->roleNames_ = QAbstractItemModel::roleNames();

	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::sourceChanged, this, &WGTreeModel::onSourceChanged);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::headerDataChangedThread, this,
	                                          &WGTreeModel::changeHeaderData, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::itemDataAboutToBeChangedThread, this,
	                                          &WGTreeModel::beginChangeData, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::itemDataChangedThread, this,
	                                          &WGTreeModel::endChangeData, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::rowsAboutToBeInsertedThread, this,
	                                          &WGTreeModel::beginInsertRows, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::rowsInsertedThread, this, &WGTreeModel::endInsertRows,
	                                          Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::rowsAboutToBeRemovedThread, this,
	                                          &WGTreeModel::beginRemoveRows, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::rowsRemovedThread, this, &WGTreeModel::endRemoveRows,
	                                          Qt::BlockingQueuedConnection);
}

WGTreeModel::~WGTreeModel()
{
	std::string modelName = this->objectName().toUtf8().constData();
	for (const auto& extension : impl_->extensions_)
	{
		extension->saveStates(modelName.c_str());
	}

	setSource(QVariant());
}

ITreeModel* WGTreeModel::getModel() const
{
	return impl_->model_;
}

void WGTreeModel::registerExtension(IModelExtensionOld* extension)
{
	beginResetModel();
	std::string modelName = this->objectName().toUtf8().constData();
	extension->loadStates(modelName.c_str());
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::itemDataAboutToBeChanged, extension,
	                                          &IModelExtensionOld::onDataAboutToBeChanged);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGTreeModel::itemDataChanged, extension, &IModelExtensionOld::onDataChanged);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::layoutAboutToBeChanged, extension,
	                                          &IModelExtensionOld::onLayoutAboutToBeChanged);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGTreeModel::layoutChanged, extension, &IModelExtensionOld::onLayoutChanged);
	impl_->qtConnections_ += QObject::connect(this, &WGTreeModel::rowsAboutToBeInserted, extension,
	                                          &IModelExtensionOld::onRowsAboutToBeInserted);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGTreeModel::rowsInserted, extension, &IModelExtensionOld::onRowsInserted);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGTreeModel::rowsAboutToBeRemoved, extension, &IModelExtensionOld::onRowsAboutToBeRemoved);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGTreeModel::rowsRemoved, extension, &IModelExtensionOld::onRowsRemoved);
	impl_->extensions_.emplace_back(extension);

	QHashIterator<int, QByteArray> itr(extension->roleNames());

	while (itr.hasNext())
	{
		itr.next();
		impl_->roleNames_.insert(itr.key(), itr.value());
	}

	endResetModel();
}

bool WGTreeModel::decodeRole(int role, ItemRole::Id& o_RoleId) const
{
	for (const auto& extension : impl_->extensions_)
	{
		if (extension->decodeRole(role, o_RoleId))
		{
			return true;
		}
	}

	return false;
}

QHash<int, QByteArray> WGTreeModel::roleNames() const
{
	return impl_->roleNames_;
}

QModelIndex WGTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0)
	{
		return QModelIndex();
	}

	ITreeModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return QModelIndex();
	}

	auto parentItem = !parent.isValid() ? nullptr : reinterpret_cast<IItem*>(parent.internalPointer());
	auto item = model->item(row, parentItem);
	if (item != nullptr && column < model->columnCount())
	{
		return createIndex(row, column, item);
	}

	return QModelIndex();
}

QModelIndex WGTreeModel::parent(const QModelIndex& child) const
{
	ITreeModel* model = getModel();
	if (model == nullptr)
	{
		return QModelIndex();
	}

	TF_ASSERT(child.isValid());
	auto childItem = reinterpret_cast<IItem*>(child.internalPointer());
	auto itemIndex = model->index(childItem);
	auto parentItem = itemIndex.second;

	if (parentItem != nullptr)
	{
		const int column = 0;
		return Impl::calculateModelIndex(*this, parentItem, column);
	}

	return QModelIndex();
}

QModelIndex WGTreeModel::convertItemToIndex(const QVariant& item) const
{
	auto variant = impl_->get<IQtHelpers>()->toVariant(item);
	auto itemPtr = reinterpret_cast<IItem*>(variant.value<intptr_t>());
	if (itemPtr != nullptr)
	{
		auto itemIndex = impl_->model_->index(itemPtr);
		const int row = static_cast<int>(itemIndex.first);
		return createIndex(row, 0, itemPtr);
	}

	return QModelIndex();
}

int WGTreeModel::rowCount(const QModelIndex& parent) const
{
	ITreeModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return 0;
	}

	auto parentItem = !parent.isValid() ? nullptr : reinterpret_cast<IItem*>(parent.internalPointer());
	return (int)model->size(parentItem);
}

int WGTreeModel::columnCount(const QModelIndex& parent) const
{
	ITreeModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return 0;
	}

	return model->columnCount();
}

QVariant WGTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto model = getModel();

	if (model == nullptr)
	{
		return QVariant::Invalid;
	}

	if (role < Qt::UserRole)
	{
		return QVariant(QVariant::Invalid);
	}

	ItemRole::Id roleId;

	if (!decodeRole(role, roleId))
	{
		return QVariant::Invalid;
	}

	return impl_->get<IQtHelpers>()->toQVariant(model->getData(section, roleId), const_cast<WGTreeModel*>(this));
}

QVariant WGTreeModel::headerData(int column, QString roleName) const
{
	auto roles = roleNames().keys(roleName.toUtf8());

	if (roles.empty())
	{
		return QVariant::Invalid;
	}

	int roleId = roles.first();
	return headerData(column, Qt::Horizontal, roleId);
}

bool WGTreeModel::hasChildren(const QModelIndex& parent) const
{
	ITreeModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return false;
	}

	auto parentItem = !parent.isValid() ? nullptr : reinterpret_cast<IItem*>(parent.internalPointer());
	if (parentItem != nullptr)
	{
		return !(model->empty(parentItem));
	}

	return false;
}

QVariant WGTreeModel::data(const QModelIndex& index, int role) const
{
	if (getModel() == nullptr || !index.isValid())
	{
		return QVariant(QVariant::Invalid);
	}

	auto item = reinterpret_cast<IItem*>(index.internalPointer());
	if (item == nullptr)
	{
		return QVariant(QVariant::Invalid);
	}

	switch (role)
	{
	case Qt::DisplayRole:
		return QString(item->getDisplayText(index.column()));

	case Qt::DecorationRole:
		auto thumbnail = item->getThumbnail(index.column());
		if (thumbnail != nullptr)
		{
			auto qtImageProvider = dynamic_cast<QtImageProviderOld*>(
			impl_->get<IQtFramework>()->qmlEngine()->imageProvider(QtImageProviderOld::providerId()));
			if (qtImageProvider != nullptr)
			{
				auto imagePath = qtImageProvider->encodeImage(thumbnail);
				return qtImageProvider->requestImage(imagePath, nullptr, QSize());
			}
		}
	}

	if (role < Qt::UserRole)
	{
		return QVariant(QVariant::Invalid);
	}

	for (auto& extension : impl_->extensions_)
	{
		auto data = extension->data(index, role);
		if (data.isValid())
		{
			return data;
		}
	}

	return QVariant(QVariant::Invalid);
}

bool WGTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (getModel() == nullptr)
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

const QVariant& WGTreeModel::getSource() const
{
	return impl_->source_;
}

void WGTreeModel::setSource(const QVariant& source)
{
	beginResetModel();
	impl_->connections_.clear();
	impl_->source_ = source;
	emit sourceChanged();
	auto model = getModel();
	if (model != nullptr)
	{
		using namespace std::placeholders;
		impl_->connections_ += model->signalDestructing.connect(std::bind(&WGTreeModel::onDestructing, this));
		impl_->connections_ +=
		model->signalModelDataChanged.connect(std::bind(&WGTreeModel::onModelDataChanged, this, _1, _2, _3));
		impl_->connections_ +=
		model->signalPreItemDataChanged.connect(std::bind(&WGTreeModel::onPreItemDataChanged, this, _1, _2, _3, _4));
		impl_->connections_ +=
		model->signalPostItemDataChanged.connect(std::bind(&WGTreeModel::onPostItemDataChanged, this, _1, _2, _3, _4));
		impl_->connections_ +=
		model->signalPreItemsInserted.connect(std::bind(&WGTreeModel::onPreItemsInserted, this, _1, _2, _3));
		impl_->connections_ +=
		model->signalPostItemsInserted.connect(std::bind(&WGTreeModel::onPostItemsInserted, this, _1, _2, _3));
		impl_->connections_ +=
		model->signalPreItemsRemoved.connect(std::bind(&WGTreeModel::onPreItemsRemoved, this, _1, _2, _3));
		impl_->connections_ +=
		model->signalPostItemsRemoved.connect(std::bind(&WGTreeModel::onPostItemsRemoved, this, _1, _2, _3));
	}
	endResetModel();
}

void WGTreeModel::onSourceChanged()
{
	ITreeModel* source = nullptr;

	auto qtHelpers = impl_->get<IQtHelpers>();
	if (qtHelpers == nullptr && impl_->model_ == nullptr)
	{
		return;
	}
	Variant variant = qtHelpers->toVariant(getSource());
	if (variant.typeIs<ITreeModel>())
	{
		source = const_cast<ITreeModel*>(variant.cast<const ITreeModel*>());
	}
	else if (variant.typeIs<ObjectHandle>())
	{
		NGT_WARNING_MSG("Double wrapping found, please wrap ITreeModel pointer as Variant directly.\n");
		ObjectHandle provider;
		if (variant.tryCast(provider))
		{
			source = provider.getBase<ITreeModel>();
		}
	}

	impl_->model_ = source;
}

QQmlListProperty<IModelExtensionOld> WGTreeModel::getExtensions() const
{
	return QQmlListProperty<IModelExtensionOld>(const_cast<WGTreeModel*>(this), nullptr, &appendExtension,
	                                            &countExtensions, &extensionAt, &clearExtensions);
}

void WGTreeModel::appendExtension(QQmlListProperty<IModelExtensionOld>* property, IModelExtensionOld* value)
{
	auto treeModel = qobject_cast<WGTreeModel*>(property->object);
	if (treeModel == nullptr)
	{
		return;
	}

	treeModel->registerExtension(value);
}

IModelExtensionOld* WGTreeModel::extensionAt(QQmlListProperty<IModelExtensionOld>* property, int index)
{
	auto treeModel = qobject_cast<WGTreeModel*>(property->object);
	if (treeModel == nullptr)
	{
		return nullptr;
	}

	return treeModel->impl_->extensions_[index];
}

void WGTreeModel::clearExtensions(QQmlListProperty<IModelExtensionOld>* property)
{
	auto treeModel = qobject_cast<WGTreeModel*>(property->object);
	if (treeModel == nullptr)
	{
		return;
	}

	treeModel->beginResetModel();
	treeModel->impl_->qtConnections_.reset();
	treeModel->impl_->extensions_.clear();
	treeModel->impl_->roleNames_ = treeModel->impl_->defaultRoleNames_;
	treeModel->endResetModel();
}

int WGTreeModel::countExtensions(QQmlListProperty<IModelExtensionOld>* property)
{
	auto treeModel = qobject_cast<WGTreeModel*>(property->object);
	if (treeModel == nullptr)
	{
		return 0;
	}

	return static_cast<int>(treeModel->impl_->extensions_.size());
}

void WGTreeModel::onDestructing()
{
	setSource(QVariant());
}

void WGTreeModel::onModelDataChanged(int column, ItemRole::Id roleId, const Variant& data)
{
	auto model = getModel();
	TF_ASSERT(model != nullptr);
	this->changeHeaderData(Qt::Orientation::Horizontal, column, column);
}

void WGTreeModel::onPreItemDataChanged(const IItem* item, int column, ItemRole::Id roleId, const Variant& data)
{
	auto model = getModel();
	TF_ASSERT(model != nullptr);
	if (item == nullptr)
	{
		return;
	}

	const int role = QtModelHelpers::encodeRole(roleId, impl_->extensions_);
	if (role < Qt::UserRole)
	{
		return;
	}

	auto index = Impl::calculateModelIndex(*this, item, column);
	// NGT-1619 Temporary workaround from @s_yuan
	auto value = impl_->get<IQtHelpers>()->toQVariant(data, this);
	// this->beginChangeData( index, role, value );
	this->beginChangeData(index, role, value);
}

void WGTreeModel::onPostItemDataChanged(const IItem* item, int column, ItemRole::Id roleId, const Variant& data)
{
	auto model = getModel();
	TF_ASSERT(model != nullptr);
	if (item == nullptr)
	{
		return;
	}

	const int role = QtModelHelpers::encodeRole(roleId, impl_->extensions_);
	if (role < Qt::UserRole)
	{
		return;
	}

	auto index = Impl::calculateModelIndex(*this, item, column);
	// NGT-1619 Temporary workaround from @s_yuan
	auto value = impl_->get<IQtHelpers>()->toQVariant(data, this);
	this->endChangeData(index, role, value);
	// this->endChangeData( index, role, QVariant() );
}

void WGTreeModel::onPreItemsInserted(const IItem* parent, size_t index, size_t count)
{
	TF_ASSERT(getModel() != nullptr);
	auto parentIndex = Impl::calculateModelIndex(*this, parent, 0);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->beginInsertRows(parentIndex, first, last);
}

void WGTreeModel::onPostItemsInserted(const IItem* parent, size_t index, size_t count)
{
	TF_ASSERT(getModel() != nullptr);
	auto parentIndex = Impl::calculateModelIndex(*this, parent, 0);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->endInsertRows(parentIndex, first, last);
}

void WGTreeModel::onPreItemsRemoved(const IItem* parent, size_t index, size_t count)
{
	TF_ASSERT(getModel() != nullptr);
	auto parentIndex = Impl::calculateModelIndex(*this, parent, 0);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->beginRemoveRows(parentIndex, first, last);
}

void WGTreeModel::onPostItemsRemoved(const IItem* parent, size_t index, size_t count)
{
	TF_ASSERT(getModel() != nullptr);
	auto parentIndex = Impl::calculateModelIndex(*this, parent, 0);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->endRemoveRows(parentIndex, first, last);
}

void WGTreeModel::changeHeaderData(Qt::Orientation orientation, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit headerDataChanged(orientation, first, last);
	}
	else
	{
		emit headerDataChangedThread(orientation, first, last, QPrivateSignal());
	}
}

void WGTreeModel::beginChangeData(const QModelIndex& index, int role, const QVariant& value)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit itemDataAboutToBeChanged(index, role, value);
	}
	else
	{
		emit itemDataAboutToBeChangedThread(index, role, value, QPrivateSignal());
	}
}

void WGTreeModel::endChangeData(const QModelIndex& index, int role, const QVariant& value)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit itemDataChanged(index, role, value);
	}
	else
	{
		emit itemDataChangedThread(index, role, value, QPrivateSignal());
	}
}

void WGTreeModel::beginInsertRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractItemModel::beginInsertRows(parent, first, last);
	}
	else
	{
		emit rowsAboutToBeInsertedThread(parent, first, last, QPrivateSignal());
	}
}

void WGTreeModel::endInsertRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractItemModel::endInsertRows();
	}
	else
	{
		emit rowsInsertedThread(parent, first, last, QPrivateSignal());
	}
}

void WGTreeModel::beginRemoveRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractItemModel::beginRemoveRows(parent, first, last);
	}
	else
	{
		emit rowsAboutToBeRemovedThread(parent, first, last, QPrivateSignal());
	}
}

void WGTreeModel::endRemoveRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractItemModel::endRemoveRows();
	}
	else
	{
		emit rowsRemovedThread(parent, first, last, QPrivateSignal());
	}
}
} // end namespace wgt
