#include "wg_list_model.hpp"

#include "core_data_model/i_item.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/models/extensions/deprecated/i_model_extension_old.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_qt_common/qt_image_provider_old.hpp"
#include "qt_model_helpers.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"
#include "core_data_model/collection_model_old.hpp"

#include <QApplication>
#include <QThread>

namespace wgt
{
class WGListModel::Impl
{
public:
	Impl();
	~Impl()
	{
	}
	static QModelIndex calculateModelIndex(const WGListModel& self, const IItem* pItem, int column);

	IQtFramework* qtFramework_;
	std::unique_ptr<IListModel> ownedModel_;
	IListModel* model_;
	QVariant source_;
	QtModelHelpers::Extensions extensions_;
	QtConnectionHolder qtConnections_;
	ConnectionHolder connections_;
	QHash<int, QByteArray> roleNames_;
	QHash<int, QByteArray> defaultRoleNames_;
};

WGListModel::Impl::Impl() : qtFramework_(nullptr), model_(nullptr)
{
}

QModelIndex WGListModel::Impl::calculateModelIndex(const WGListModel& self, const IItem* item, int column)
{
	IListModel* model = self.getModel();
	if (item == nullptr || model == nullptr)
	{
		return QModelIndex();
	}

	auto itemIndex = model->index(item);
	const int row = static_cast<int>(itemIndex);
	return self.createIndex(row, column, const_cast<IItem*>(item));
}

WGListModel::WGListModel() : QAbstractListModel(), impl_(new Impl())
{
	impl_->defaultRoleNames_ = QAbstractListModel::roleNames();
	impl_->roleNames_ = QAbstractListModel::roleNames();

	impl_->qtFramework_ = Context::queryInterface<IQtFramework>();

	impl_->qtConnections_ += QObject::connect(this, &WGListModel::sourceChanged, this, &WGListModel::onSourceChanged);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::headerDataChangedThread, this,
	                                          &WGListModel::changeHeaderData, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::itemDataAboutToBeChangedThread, this,
	                                          &WGListModel::beginChangeData, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::itemDataChangedThread, this,
	                                          &WGListModel::endChangeData, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::rowsAboutToBeInsertedThread, this,
	                                          &WGListModel::beginInsertRows, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::rowsInsertedThread, this, &WGListModel::endInsertRows,
	                                          Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::rowsAboutToBeRemovedThread, this,
	                                          &WGListModel::beginRemoveRows, Qt::BlockingQueuedConnection);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::rowsRemovedThread, this, &WGListModel::endRemoveRows,
	                                          Qt::BlockingQueuedConnection);
}

WGListModel::~WGListModel()
{
	setSource(QVariant());
}

IListModel* WGListModel::getModel() const
{
	return impl_->model_;
}

bool WGListModel::canClear() const
{
	auto m = getModel();

	if (m == nullptr)
	{
		return false;
	}

	return m->canClear();
}

void WGListModel::clear()
{
	auto m = getModel();

	if (m == nullptr)
	{
		return;
	}

	m->clear();
}

QModelIndex WGListModel::index(int row, int column, const QModelIndex& parent) const
{
	IListModel* model = getModel();
	if (model == nullptr)
	{
		return QModelIndex();
	}

	auto item = model->item(row);
	if (item != nullptr && column < model->columnCount())
	{
		return createIndex(row, column, item);
	}

	return QModelIndex();
}

int WGListModel::indexRow(const QModelIndex& index) const
{
	return index.row();
}

QModelIndex WGListModel::find(QVariant data, QString roleName) const
{
	for (auto row = 0; row < rowCount(QModelIndex()); ++row)
	{
		auto index = this->index(row);
		if (data == this->data(index, roleName))
		{
			return index;
		}
	}
	return QModelIndex();
}

QVariant WGListModel::data(const QModelIndex& index, QString roleName) const
{
	auto role = impl_->roleNames_.key(roleName.toUtf8(), -1);
	if (role < 0)
	{
		return QVariant::Invalid;
	}

	return data(index, role);
}

void WGListModel::registerExtension(IModelExtensionOld* extension)
{
	beginResetModel();
	extension->init(impl_->qtFramework_);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::itemDataAboutToBeChanged, extension,
	                                          &IModelExtensionOld::onDataAboutToBeChanged);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGListModel::itemDataChanged, extension, &IModelExtensionOld::onDataChanged);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::layoutAboutToBeChanged, extension,
	                                          &IModelExtensionOld::onLayoutAboutToBeChanged);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGListModel::layoutChanged, extension, &IModelExtensionOld::onLayoutChanged);
	impl_->qtConnections_ += QObject::connect(this, &WGListModel::rowsAboutToBeInserted, extension,
	                                          &IModelExtensionOld::onRowsAboutToBeInserted);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGListModel::rowsInserted, extension, &IModelExtensionOld::onRowsInserted);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGListModel::rowsAboutToBeRemoved, extension, &IModelExtensionOld::onRowsAboutToBeRemoved);
	impl_->qtConnections_ +=
	QObject::connect(this, &WGListModel::rowsRemoved, extension, &IModelExtensionOld::onRowsRemoved);
	impl_->extensions_.emplace_back(extension);

	QHashIterator<int, QByteArray> itr(extension->roleNames());

	while (itr.hasNext())
	{
		itr.next();
		impl_->roleNames_.insert(itr.key(), itr.value());
	}

	endResetModel();
}

bool WGListModel::decodeRole(int role, ItemRole::Id& o_RoleId) const
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

QHash<int, QByteArray> WGListModel::roleNames() const
{
	return impl_->roleNames_;
}

int WGListModel::rowCount(const QModelIndex& parent) const
{
	// Qt Lists never have a valid parent
	// because it can't have child items
	IListModel* model = getModel();
	if (model == nullptr || parent.isValid())
	{
		return 0;
	}

	return (int)model->size();
}

int WGListModel::columnCount(const QModelIndex& parent) const
{
	IListModel* model = getModel();
	if (model == nullptr || parent.column() > 0)
	{
		return 0;
	}

	return (int)model->columnCount();
}

QVariant WGListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto model = getModel();

	if (model == nullptr)
	{
		return QVariant::Invalid;
	}

	if (role < Qt::UserRole)
	{
		return QVariant::Invalid;
	}

	ItemRole::Id roleId;

	if (!decodeRole(role, roleId))
	{
		return QVariant::Invalid;
	}

	return QtHelpers::toQVariant(model->getData(section, roleId), const_cast<WGListModel*>(this));
}

QVariant WGListModel::headerData(int column, QString roleName) const
{
	auto roles = roleNames().keys(roleName.toUtf8());

	if (roles.empty())
	{
		return QVariant::Invalid;
	}

	int role = roles.first();
	return headerData(column, Qt::Horizontal, role);
}

QVariant WGListModel::data(const QModelIndex& index, int role) const
{
	if (getModel() == nullptr)
	{
		return QVariant(QVariant::Invalid);
	}

	assert(index.isValid());
	auto item = reinterpret_cast<IItem*>(index.internalPointer());

	switch (role)
	{
	case Qt::DisplayRole:
		return QString(item->getDisplayText(index.column()));

	case Qt::DecorationRole:
		auto thumbnail = item->getThumbnail(index.column());
		if (thumbnail != nullptr)
		{
			auto qtImageProvider = dynamic_cast<QtImageProviderOld*>(
			impl_->qtFramework_->qmlEngine()->imageProvider(QtImageProviderOld::providerId()));
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

bool WGListModel::setData(const QModelIndex& index, const QVariant& value, int role)
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

const QVariant& WGListModel::getSource() const
{
	return impl_->source_;
}

void WGListModel::setSource(const QVariant& source)
{
	beginResetModel();
	impl_->connections_.clear();
	impl_->source_ = source;
	emit sourceChanged();
	auto model = getModel();
	if (model != nullptr)
	{
		using namespace std::placeholders;
		impl_->connections_ += model->signalDestructing.connect(std::bind(&WGListModel::onDestructing, this));
		impl_->connections_ +=
		model->signalModelDataChanged.connect(std::bind(&WGListModel::onModelDataChanged, this, _1, _2, _3));
		impl_->connections_ +=
		model->signalPreItemDataChanged.connect(std::bind(&WGListModel::onPreItemDataChanged, this, _1, _2, _3, _4));
		impl_->connections_ +=
		model->signalPostItemDataChanged.connect(std::bind(&WGListModel::onPostItemDataChanged, this, _1, _2, _3, _4));
		impl_->connections_ +=
		model->signalPreItemsInserted.connect(std::bind(&WGListModel::onPreItemsInserted, this, _1, _2));
		impl_->connections_ +=
		model->signalPostItemsInserted.connect(std::bind(&WGListModel::onPostItemsInserted, this, _1, _2));
		impl_->connections_ +=
		model->signalPreItemsRemoved.connect(std::bind(&WGListModel::onPreItemsRemoved, this, _1, _2));
		impl_->connections_ +=
		model->signalPostItemsRemoved.connect(std::bind(&WGListModel::onPostItemsRemoved, this, _1, _2));
	}
	endResetModel();
}

void WGListModel::onSourceChanged()
{
	IListModel* source = nullptr;
	impl_->ownedModel_.reset();

	Variant variant = QtHelpers::toVariant(getSource());
	if (variant.typeIs<IListModel>())
	{
		source = const_cast<IListModel*>(variant.cast<const IListModel*>());
	}
	else if (variant.typeIs<ObjectHandle>())
	{
		NGT_WARNING_MSG("Double wrapping found, please wrap IListModel pointer as Variant directly.\n");
		ObjectHandle provider;
		if (variant.tryCast(provider))
		{
			source = provider.getBase<IListModel>();
		}
	}
	else if (variant.typeIs<Collection>())
	{
		Collection collection = variant.cast<Collection>();
		auto collectionModel = new CollectionModelOld();
		collectionModel->setSource(collection);
		impl_->ownedModel_.reset(collectionModel);
		source = impl_->ownedModel_.get();
	}
	impl_->model_ = source;
}

QQmlListProperty<IModelExtensionOld> WGListModel::getExtensions() const
{
	return QQmlListProperty<IModelExtensionOld>(const_cast<WGListModel*>(this), nullptr, &appendExtension,
	                                            &countExtensions, &extensionAt, &clearExtensions);
}

void WGListModel::appendExtension(QQmlListProperty<IModelExtensionOld>* property, IModelExtensionOld* value)
{
	auto listModel = qobject_cast<WGListModel*>(property->object);
	if (listModel == nullptr)
	{
		return;
	}

	listModel->registerExtension(value);
}

IModelExtensionOld* WGListModel::extensionAt(QQmlListProperty<IModelExtensionOld>* property, int index)
{
	auto listModel = qobject_cast<WGListModel*>(property->object);
	if (listModel == nullptr)
	{
		return nullptr;
	}

	return listModel->impl_->extensions_[index];
}

void WGListModel::clearExtensions(QQmlListProperty<IModelExtensionOld>* property)
{
	auto listModel = qobject_cast<WGListModel*>(property->object);
	if (listModel == nullptr)
	{
		return;
	}

	listModel->beginResetModel();
	listModel->impl_->qtConnections_.reset();
	listModel->impl_->extensions_.clear();
	listModel->impl_->roleNames_ = listModel->impl_->defaultRoleNames_;
	listModel->endResetModel();
}

int WGListModel::countExtensions(QQmlListProperty<IModelExtensionOld>* property)
{
	auto listModel = qobject_cast<WGListModel*>(property->object);
	if (listModel == nullptr)
	{
		return 0;
	}

	return static_cast<int>(listModel->impl_->extensions_.size());
}

void WGListModel::onDestructing()
{
	setSource(QVariant());
}

void WGListModel::onModelDataChanged(int column, ItemRole::Id roleId, const Variant& data)
{
	auto model = getModel();
	assert(model != nullptr);
	changeHeaderData(Qt::Orientation::Horizontal, column, column);
}

void WGListModel::onPreItemDataChanged(const IItem* item, int column, ItemRole::Id roleId, const Variant& data)
{
	auto model = getModel();
	assert(model != nullptr);
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
	auto value = QtHelpers::toQVariant(data, this);
	this->beginChangeData(index, role, value);
}

void WGListModel::onPostItemDataChanged(const IItem* item, int column, ItemRole::Id roleId, const Variant& data)
{
	auto model = getModel();
	assert(model != nullptr);
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
	auto value = QtHelpers::toQVariant(data, this);
	this->endChangeData(index, role, value);
}

void WGListModel::onPreItemsInserted(size_t index, size_t count)
{
	assert(getModel() != nullptr);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->beginInsertRows(QModelIndex(), first, last);
}

void WGListModel::onPostItemsInserted(size_t index, size_t count)
{
	assert(getModel() != nullptr);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->endInsertRows(QModelIndex(), first, last);
}

void WGListModel::onPreItemsRemoved(size_t index, size_t count)
{
	assert(getModel() != nullptr);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->beginRemoveRows(QModelIndex(), first, last);
}

void WGListModel::onPostItemsRemoved(size_t index, size_t count)
{
	assert(getModel() != nullptr);
	const int first = QtModelHelpers::calculateFirst(index);
	const int last = QtModelHelpers::calculateLast(index, count);
	this->endRemoveRows(QModelIndex(), first, last);
}

void WGListModel::changeHeaderData(Qt::Orientation orientation, int first, int last)
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

void WGListModel::beginChangeData(const QModelIndex& index, int role, const QVariant& value)
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

void WGListModel::endChangeData(const QModelIndex& index, int role, const QVariant& value)
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

void WGListModel::beginInsertRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractListModel::beginInsertRows(parent, first, last);
	}
	else
	{
		emit rowsAboutToBeInsertedThread(parent, first, last, QPrivateSignal());
	}
}

void WGListModel::endInsertRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractListModel::endInsertRows();
	}
	else
	{
		emit rowsInsertedThread(parent, first, last, QPrivateSignal());
	}
}

void WGListModel::beginRemoveRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractListModel::beginRemoveRows(parent, first, last);
	}
	else
	{
		emit rowsAboutToBeRemovedThread(parent, first, last, QPrivateSignal());
	}
}

void WGListModel::endRemoveRows(const QModelIndex& parent, int first, int last)
{
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		emit QAbstractListModel::endRemoveRows();
	}
	else
	{
		emit rowsRemovedThread(parent, first, last, QPrivateSignal());
	}
}
} // end namespace wgt
