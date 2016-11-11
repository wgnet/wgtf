#include "qt_item_model.hpp"

#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "helpers/qt_helpers.hpp"
#include "wg_types/base64.hpp"
#include <QMimeData>

namespace wgt
{
struct QtItemModel::Impl
{
	Impl(QtItemModel& self, AbstractItemModel& source) : self_(self), source_(source)
	{
	}

	QtItemModel& self_;
	AbstractItemModel& source_;

	Connection connectPreModelReset_;
	Connection connectPostModelReset_;

	Connection connectPreChange_;
	Connection connectPostChanged_;

	Connection connectPreInsert_;
	Connection connectPostInserted_;

	Connection connectPreErase_;
	Connection connectPostErased_;

	Connection connectPreMove_;
	Connection connectPostMoved_;
};

QtItemModel::QtItemModel(IComponentContext& context, AbstractItemModel& source)
    : itemModelController_(context), impl_(new Impl(*this, source))
{
	auto preReset = [this]() { this->beginResetModel(); };
	impl_->connectPreModelReset_ = impl_->source_.connectPreModelReset(preReset);

	auto postReset = [this]() { this->endResetModel(); };
	impl_->connectPostModelReset_ = impl_->source_.connectPostModelReset(postReset);

	// @see AbstractItemModel::DataSignature
	auto preData = [this](const AbstractItemModel::ItemIndex& index, ItemRole::Id role, const Variant& newValue) {
		auto item = impl_->source_.item(index);
		const QModelIndex modelIndex = createIndex(index.row_, index.column_, item);

		// HACK: should not be explicitly referencing DefinitionRole here
		if (role == DefinitionRole::roleId_)
		{
			QList<QPersistentModelIndex> parents;
			parents.append(modelIndex.isValid() ? modelIndex : QModelIndex());
			this->layoutAboutToBeChanged(parents, QAbstractItemModel::VerticalSortHint);
			return;
		}
		// END HACK
	};
	impl_->connectPreChange_ = impl_->source_.connectPreItemDataChanged(preData);

	auto postData = [this](const AbstractItemModel::ItemIndex& index, ItemRole::Id roleId, const Variant& newValue) {
		auto item = impl_->source_.item(index);
		const QModelIndex modelIndex = createIndex(index.row_, index.column_, item);

		// HACK: should not be explicitly referencing DefinitionRole here
		if (roleId == DefinitionRole::roleId_)
		{
			QList<QPersistentModelIndex> parents;
			parents.append(modelIndex.isValid() ? modelIndex : QModelIndex());
			this->layoutChanged(parents, QAbstractItemModel::VerticalSortHint);
			return;
		}
		// END HACKs

		int encodedRole;
		if (encodeRole(roleId, encodedRole))
		{
			const QModelIndex topLeft = modelIndex;
			const QModelIndex bottomRight = modelIndex;
			QVector<int> encodedRoles;
			encodedRoles.append(encodedRole);
			this->dataChanged(topLeft, bottomRight, encodedRoles);
		}
	};
	impl_->connectPostChanged_ = impl_->source_.connectPostItemDataChanged(postData);

	// @see AbstractItemModel::RangeSignature
	auto preInsert = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		auto parentItem = impl_->source_.item(parentIndex);
		const QModelIndex modelIndex = createIndex(parentIndex.row_, parentIndex.column_, parentItem);
		this->beginInsertRows(modelIndex.isValid() ? modelIndex : QModelIndex(), startPos, startPos + count - 1);
	};
	impl_->connectPreInsert_ = impl_->source_.connectPreRowsInserted(preInsert);

	auto postInserted = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		this->endInsertRows();
	};
	impl_->connectPostInserted_ = impl_->source_.connectPostRowsInserted(postInserted);

	auto preErase = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		auto parentItem = impl_->source_.item(parentIndex);
		const QModelIndex modelIndex = createIndex(parentIndex.row_, parentIndex.column_, parentItem);
		this->beginRemoveRows(modelIndex.isValid() ? modelIndex : QModelIndex(), startPos, startPos + count - 1);
	};
	impl_->connectPreErase_ = impl_->source_.connectPreRowsRemoved(preErase);

	auto postErased = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		this->endRemoveRows();
	};
	impl_->connectPostErased_ = impl_->source_.connectPostRowsRemoved(postErased);

	auto preMove = [this](const AbstractItemModel::ItemIndex& sourceParentIndex, int sourceFirst, int sourceLast,
	                      const AbstractItemModel::ItemIndex& destinationParentIndex, int destinationRow) {
		const auto pSourceParentItem = impl_->source_.item(sourceParentIndex);
		const QModelIndex sourceModelIndex =
		createIndex(sourceParentIndex.row_, sourceParentIndex.column_, pSourceParentItem);
		const auto pDestinationParentItem = impl_->source_.item(destinationParentIndex);
		const QModelIndex destinationModelIndex =
		createIndex(destinationParentIndex.row_, destinationParentIndex.column_, pDestinationParentItem);
		const auto success =
		this->beginMoveRows(sourceModelIndex, sourceFirst, sourceLast, destinationModelIndex, destinationRow);
		assert(success);
	};
	impl_->connectPreMove_ = impl_->source_.connectPreRowsMoved(preMove);

	auto postMoved = [this](const AbstractItemModel::ItemIndex& sourceParentIndex, int sourceFirst, int sourceLast,
	                        const AbstractItemModel::ItemIndex& destinationParentIndex,
	                        int destinationRow) { this->endMoveRows(); };
	impl_->connectPostMoved_ = impl_->source_.connectPostRowsMoved(postMoved);
}

QtItemModel::~QtItemModel()
{
	impl_->connectPostMoved_.disconnect();
	impl_->connectPreMove_.disconnect();

	impl_->connectPostErased_.disconnect();
	impl_->connectPreErase_.disconnect();

	impl_->connectPostInserted_.disconnect();
	impl_->connectPreInsert_.disconnect();

	// TODO not removed from list??
	impl_->connectPostChanged_.disconnect();
	impl_->connectPreChange_.disconnect();

	impl_->connectPostModelReset_.disconnect();
	impl_->connectPreModelReset_.disconnect();
}

const AbstractItemModel& QtItemModel::source() const
{
	return impl_->source_;
}

AbstractItemModel& QtItemModel::source()
{
	return impl_->source_;
}

QModelIndex QtItemModel::index(int row, int column, const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	AbstractItemModel::ItemIndex itemIndex(row, column, parentItem);
	auto item = impl_->source_.item(itemIndex);
	if (item == nullptr)
	{
		return QModelIndex();
	}

	return createIndex(row, column, item);
}

QModelIndex QtItemModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}

	auto childItem = reinterpret_cast<AbstractItem*>(child.internalId());
	AbstractItemModel::ItemIndex childIndex;
	impl_->source_.index(childItem, childIndex);
	if (!childIndex.isValid())
	{
		return QModelIndex();
	}

	auto parentItem = const_cast<AbstractItem*>(childIndex.parent_);
	if (parentItem == nullptr)
	{
		return QModelIndex();
	}

	AbstractItemModel::ItemIndex parentIndex;
	impl_->source_.index(parentItem, parentIndex);
	if (!parentIndex.isValid())
	{
		return QModelIndex();
	}

	return createIndex(parentIndex.row_, parentIndex.column_, parentItem);
}

int QtItemModel::rowCount(const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	return impl_->source_.rowCount(parentItem);
}

int QtItemModel::columnCount(const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	return impl_->source_.columnCount(parentItem);
}

bool QtItemModel::hasChildren(const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	return impl_->source_.hasChildren(parentItem);
}

QVariant QtItemModel::data(const QModelIndex& index, int role) const
{
	auto item = index.isValid() ? reinterpret_cast<AbstractItem*>(index.internalId()) : nullptr;

	if (item == nullptr)
	{
		return QVariant();
	}

	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return QVariant();
	}

	auto data = item->getData(index.row(), index.column(), roleId);
	return QtHelpers::toQVariant(data, const_cast<QtItemModel*>(this));
}

bool QtItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	auto item = index.isValid() ? reinterpret_cast<AbstractItem*>(index.internalId()) : nullptr;

	if (item == nullptr)
	{
		return false;
	}

	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return false;
	}

	// Use QVariant for comparison
	// because QVariant has lower precision than Variant
	auto oldValue = QtHelpers::toQVariant(item->getData(index.row(), index.column(), roleId), this);

	// Prevent binding loops with self-assignment check
	// Note that self-assignment check will not detect floating-point error
	if (value == oldValue)
	{
		return true;
	}

	auto data = QtHelpers::toVariant(value);

	// Item already uses the Command System or the Command System is not available
	if (item->hasController() || (itemModelController_ == nullptr))
	{
		// Set property directly
		return item->setData(index.row(), index.column(), roleId, data);
	}

	// Queue with Command System, to register undo/redo data
	auto pParent = index.parent().isValid() ? reinterpret_cast<AbstractItem*>(index.parent().internalId()) : nullptr;
	AbstractItemModel::ItemIndex dataModelIndex(index.row(), index.column(), pParent);

	return itemModelController_->setValue(impl_->source_, dataModelIndex, roleId, data);
}

QVariant QtItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return QVariant();
	}

	auto row = orientation == Qt::Vertical ? section : -1;
	auto column = orientation == Qt::Horizontal ? section : -1;
	auto data = impl_->source_.getData(row, column, roleId);
	return QtHelpers::toQVariant(data, const_cast<QtItemModel*>(this));
}

bool QtItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return false;
	}

	auto row = orientation == Qt::Vertical ? section : -1;
	auto column = orientation == Qt::Horizontal ? section : -1;
	auto data = QtHelpers::toVariant(value);

	// Item already uses the Command System or the Command System is not available
	if (impl_->source_.hasController() || (itemModelController_ == nullptr))
	{
		// Set property directly
		return impl_->source_.setData(row, column, roleId, data);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->setModelData(impl_->source_, row, column, roleId, data);
}

bool QtItemModel::insertRows(int row, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	// Item already uses the Command System or the Command System is not available
	if (impl_->source_.hasController() || (itemModelController_ == nullptr))
	{
		// Set property directly
		return impl_->source_.insertRows(row, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->insertRows(impl_->source_, row, count, parentItem);
}

bool QtItemModel::insertColumns(int column, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	// Item already uses the Command System or the Command System is not available
	if (impl_->source_.hasController() || (itemModelController_ == nullptr))
	{
		// Set property directly
		return impl_->source_.insertColumns(column, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->insertColumns(impl_->source_, column, count, parentItem);
}

bool QtItemModel::removeRows(int row, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	// Item already uses the Command System or the Command System is not available
	if (impl_->source_.hasController() || (itemModelController_ == nullptr))
	{
		// Set property directly
		return impl_->source_.removeRows(row, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->removeRows(impl_->source_, row, count, parentItem);
}

bool QtItemModel::removeColumns(int column, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	// Item already uses the Command System or the Command System is not available
	if (impl_->source_.hasController() || (itemModelController_ == nullptr))
	{
		// Set property directly
		return impl_->source_.removeColumns(column, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->removeColumns(impl_->source_, column, count, parentItem);
}

bool QtItemModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
                           const QModelIndex& destinationParent, int destinationChild) /* override */
{
	const auto sourceParentItem =
	sourceParent.isValid() ? reinterpret_cast<AbstractItem*>(sourceParent.internalId()) : nullptr;
	const auto destinationParentItem =
	destinationParent.isValid() ? reinterpret_cast<AbstractItem*>(destinationParent.internalId()) : nullptr;

	// Item already uses the Command System or the Command System is not available
	if (impl_->source_.hasController() || (itemModelController_ == nullptr))
	{
		// Set property directly
		return impl_->source_.moveRows(sourceParentItem, sourceRow, count, destinationParentItem, destinationChild);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->moveRows(impl_->source_, sourceParentItem, sourceRow, count, destinationParentItem,
	                                      destinationChild);
}

QStringList QtItemModel::mimeTypes() const
{
	std::vector<std::string> mimeTypes = impl_->source_.mimeTypes();

	QStringList result;
	for (auto& str : mimeTypes)
	{
		QString qstr = str.c_str();
		result.push_back(qstr);
	}

	return result;
}

QMimeData* QtItemModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* data = new QMimeData();

	std::vector<AbstractItemModel::ItemIndex> itemIndices;
	for (auto& index : indexes)
	{
		auto parentIndex = index.parent();
		auto parentItem = parentIndex.isValid() ? reinterpret_cast<AbstractItem*>(parentIndex.internalId()) : nullptr;
		AbstractItemModel::ItemIndex itemIndex(index.row(), index.column(), parentItem);
		itemIndices.push_back(itemIndex);
	}

	MimeData mimeData = impl_->source_.mimeData(itemIndices);
	for (auto it = mimeData.begin(); it != mimeData.end(); ++it)
	{
		QByteArray buffer(it->second.data(), (int)it->second.size());
		data->setData(it->first.c_str(), buffer);
	}

	return data;
}

bool QtItemModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                  const QModelIndex& parent) const
{
	assert(data != nullptr);

	MimeData mimeData;
	for (auto& format : data->formats())
	{
		const QByteArray& dataArr = data->data(format);
		const char* dataBytes = dataArr.constData();
		std::vector<char> buffer(dataBytes, dataBytes + dataArr.size());
		mimeData[format.toUtf8().data()] = buffer;
	}

	DropAction dropAction = InvalidAction;
	switch (action)
	{
	case Qt::MoveAction:
		dropAction = MoveAction;
		break;

	case Qt::CopyAction:
		dropAction = CopyAction;
		break;

	default:
		break;
	}

	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	AbstractItemModel::ItemIndex itemIndex(row, column, parentItem);

	return impl_->source_.canDropMimeData(mimeData, dropAction, itemIndex);
}

bool QtItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                               const QModelIndex& parent)
{
	assert(data != nullptr);

	MimeData mimeData;
	for (auto& format : data->formats())
	{
		const QByteArray& dataArr = data->data(format);
		const char* dataBytes = dataArr.constData();
		std::vector<char> buffer(dataBytes, dataBytes + dataArr.size());
		mimeData[format.toUtf8().data()] = buffer;
	}

	DropAction dropAction = InvalidAction;
	switch (action)
	{
	case Qt::MoveAction:
		dropAction = MoveAction;
		break;

	case Qt::CopyAction:
		dropAction = CopyAction;
		break;

	default:
		break;
	}

	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	AbstractItemModel::ItemIndex itemIndex(row, column, parentItem);

	return impl_->source_.dropMimeData(mimeData, dropAction, itemIndex);
}

Qt::ItemFlags QtItemModel::flags(const QModelIndex& index) const /* override */
{
	const auto defaultFlags = QAbstractItemModel::flags(index);

	if (index.isValid())
	{
		const auto readOnlyFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | defaultFlags;
		const auto editableFlags = Qt::ItemIsEditable | readOnlyFlags;
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | editableFlags;
	}
	else
	{
		return Qt::ItemIsDropEnabled | defaultFlags;
	}
}

QHash<int, QByteArray> QtItemModel::roleNames() const
{
	auto roles = impl_->source_.roles();
	auto roleNames = QAbstractItemModel::roleNames();
	for (auto& role : roles)
	{
		registerRole(role.c_str(), roleNames);
	}
	return roleNames;
}

bool QtItemModel::encodeRole(ItemRole::Id roleId, int& o_Role) const
{
	if (roleId == ItemRole::displayId)
	{
		o_Role = Qt::DisplayRole;
		return true;
	}
	if (roleId == ItemRole::decorationId)
	{
		o_Role = Qt::DecorationRole;
		return true;
	}
	return RoleProvider::encodeRole(roleId, o_Role);
}

bool QtItemModel::decodeRole(int role, ItemRole::Id& o_RoleId) const
{
	switch (role)
	{
	case Qt::DisplayRole:
		o_RoleId = ItemRole::displayId;
		return true;

	case Qt::DecorationRole:
		o_RoleId = ItemRole::decorationId;
		return true;

	default:
		return RoleProvider::decodeRole(role, o_RoleId);
	}
}

QtListModel::QtListModel(IComponentContext& context, AbstractListModel& source) : QtItemModel(context, source)
{
}

const AbstractListModel& QtListModel::source() const
{
	return static_cast<const AbstractListModel&>(QtItemModel::source());
}

AbstractListModel& QtListModel::source()
{
	return static_cast<AbstractListModel&>(QtItemModel::source());
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

QtTreeModel::QtTreeModel(IComponentContext& context, AbstractTreeModel& source) : QtItemModel(context, source)
{
}

const AbstractTreeModel& QtTreeModel::source() const
{
	return static_cast<const AbstractTreeModel&>(QtItemModel::source());
}

AbstractTreeModel& QtTreeModel::source()
{
	return static_cast<AbstractTreeModel&>(QtItemModel::source());
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

QtTableModel::QtTableModel(IComponentContext& context, AbstractTableModel& source) : QtItemModel(context, source)
{
}

const AbstractTableModel& QtTableModel::source() const
{
	return static_cast<const AbstractTableModel&>(QtItemModel::source());
}

AbstractTableModel& QtTableModel::source()
{
	return static_cast<AbstractTableModel&>(QtItemModel::source());
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
