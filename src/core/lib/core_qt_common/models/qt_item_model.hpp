#ifndef QT_ITEM_MODEL_HPP
#define QT_ITEM_MODEL_HPP

#include "qt_abstract_item_model.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "role_provider.hpp"
#include "core_common/assert.hpp"
#include "core_data_model_cmds/interfaces/i_item_model_controller.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "core_qt_common/models/extensions/custom_model_extension.hpp"
#include "core_qt_common/models/wgt_item_model_base.hpp"
#include "core_qt_common/helpers/wgt_interface_provider.hpp"
#include "core_qt_common/interfaces/i_wgt_item_model.hpp"
#include "wg_types/base64.hpp"
#include "core_variant/variant.hpp"


#include <QMimeData>
#include <memory>
#include <unordered_map>

namespace std {
template <>
struct hash<QModelIndex>
{
	std::size_t operator()(const QModelIndex& k) const
	{
		return qHash(k);
	}
};

}

namespace wgt
{
/** Adapter layer to allow any AbstractItemModel to be used by Qt and QML views.*/
template <class BaseModel>
class QtItemModel : public BaseModel, public RoleProvider
	, public WGTInterfaceProvider
	, public Implements< IWgtItemModel >
{
	// Q_OBJECT This does not support templates.
	DECLARE_QT_MEMORY_HANDLER

public:
	typedef typename BaseModel::SourceType SourceType;

	QtItemModel(SourceType& source);
	virtual ~QtItemModel();

	/** Gets AbstractItemModel that is being adapted to be used by Qt.
	@return A constant reference to the attached AbstractItemModel. */
	const SourceType& source() const;

	/** Gets AbstractItemModel that is being adapted to be used by Qt.
	@return A reference to the attached AbstractItemModel. */
	SourceType& source();

	/** Converts a location in the model into an index.
	@note An invalid index implies the top level of the model.
	@param row The row number for the index.
	@param column The column number for the index.
	@param parent The index this index falls under.
	@return A model index containing the location information. */
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;

	/** Finds the parent index for an index.
	@param child The index to find the parent for.
	@return The index's parent index. */
	QModelIndex parent(const QModelIndex& child) const override;

	/** Gets number of rows in the model, relating to a parent.
	@note An invalid index implies the top level of the model.
	@param parent The parent index.
	@return The number of rows. */
	int rowCount(const QModelIndex& parent) const override;

	/** Gets number of columns in the model, relating to a parent.
	@note An invalid index implies the top level of the model.
	@param parent The parent index.
	@return The number of columns. */
	int columnCount(const QModelIndex& parent) const override;

	/** Determines if there are any indices under a parent index.
	@note An invalid index implies the top level of the model.
	@param parent The parent index to check under.
	@return The number of indices under the parent index. */
	bool hasChildren(const QModelIndex& parent) const override;

	/** Gets role data from an item at an index position.
	This converts the index and role to the format of the source model, calls
	the source model's getData, converts the value to a Qt compatible value,
	and returns that converted value.
	@param index The location of the item.
	@param role The role identifier of the value to get.
	@return The value of the role. */
	QVariant data(const QModelIndex& index, int role) const override;

	bool canUse(QueryHelper & o_Helper) const override { return true; }

	Variant variantData(
		QueryHelper & helper, const QModelIndex& index, int role) const override;

	/** Changes role data for an item at an index position.
	This converts the index, value, and role to the format of the source model, calls
	the source model's setData, and returns whether if was successful or not.
	@param index The location of the item.
	@param value The new value of the role.
	@param role The role identifier of the value to get.
	@return True if successful. */
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	/** Gets role header data at a position in the header.
	This converts the role to the source model's format, calls the source model's getHeaderData,
	converts the value to a Qt compatible value, and returns that converted value.
	@param section The location of the item, either column or row depending on orientation.
	@param orientation The layout of the header, either horizontal or vertical.
	@param role The role identifier of the value to get.
	@return The value of the role. */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	/** Changes role header data at a position in the header.
	This converts the role and value to the source model's format,
	calls the source model's setHeaderData,	and returns whether it was successful or not.
	@param section The location of the item, either column or row depending on orientation.
	@param orientation The layout of the header, either horizontal or vertical.
	@param value The new value of the role.
	@param role The role identifier of the value to get.
	@return True if successful. */
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role) override;

	/** Inserts new blank rows at the specified position for the specified parent.
	@note If row is rowCount(), the rows are added to the end.
	@note An invalid index implies the top level of the model.
	@param row Row position to insert at.
	@param count Number of rows to insert.
	@param parent Parent index to insert under.
	@return True if a row was inserted. */
	bool insertRows(int row, int count, const QModelIndex& parent) override;

	/** Inserts new columns at the specified position for the specified parent.
	@note If column is columnCount(), the column is added to the end.
	@note An invalid index implies the top level of the model.
	@param column Column position to insert at.
	@param count Number of columns to insert.
	@param parent Parent index to insert under.
	@return True if a column was inserted. */
	bool insertColumns(int column, int count, const QModelIndex& parent) override;

	/** Removes rows at the specified position for the specified parent.
	@note An invalid index implies the top level of the model.
	@param row Row position to remove.
	@param count Number of rows to remove.
	@param parent Parent index to remove from.
	@return True if a row was removed. */
	bool removeRows(int row, int count, const QModelIndex& parent) override;

	/** Removes columns at the specified position for the specified parent.
	@note An invalid index implies the top level of the model.
	@param column Column position to remove.
	@param count Number of columns to remove.
	@param parent Parent index to remove from.
	@return True if a column was removed. */
	bool removeColumns(int column, int count, const QModelIndex& parent) override;

	/** Moves a number of rows from one position and parent
	to another position and parent.
	@note An invalid index implies the top level of the model.
	@param sourceParent The parent index containing the rows to move.
	@param sourceRow The starting row number to move.
	@param count The amount of rows to move.
	@param destinationParent The parent index to which the rows are moved.
	@param destinationChild The target row number for the first row to move.
	@return True if the rows were moved. */
	bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent,
	              int destinationChild) override;

	virtual QStringList mimeTypes() const override;
	virtual QMimeData* mimeData(const QModelIndexList& indexes) const override;
	virtual bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
	                             const QModelIndex& parent) const override;
	virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
	                          const QModelIndex& parent) override;

	/** Returns the supported flags for this model.
	The supported flags include the default flags, as well the the following:
	For valid indices:
	    Qt::ItemIsEnabled
	    Qt::ItemIsSelectable
	    Qt::ItemIsEditable
	    Qt::ItemIsDragEnabled
	    Qt::ItemIsDropEnabled
	For invalid indices:
	    Qt::ItemIsDropEnabled
	More information about these can be found here: http://doc.qt.io/qt-5/qt.html#ItemFlag-enum
	This field can contain multiple bit flags.
	@ingroup qmlaccessible
	@param index The index to which the flags apply to.
	@return The combination of flags. */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** Returns a map of supported roles to be used in QML.
	These are mapped from role identifier to role name.
	@return The roles map. */
	QHash<int, QByteArray> roleNames() const override;

	virtual void revert() override;
	virtual bool submit() override;

	virtual QModelIndex findIndex(QVariant id) const override;

	bool encodeRole(ItemRole::Id roleId, int& o_Role) const override;
	bool decodeRole(int role, ItemRole::Id& o_RoleId) const override;

	static QtItemModel<BaseModel>* fromQVariant(const QVariant& variant);

private:

	QModelIndex getCachedParentIndex(const QModelIndex& childIndex) const;

	mutable std::unordered_map<QModelIndex, QModelIndex>	childToParentIndexCache_;
	AbstractItemModel&										source_;
	ConnectionHolder										connections_;

	struct Dependencies : public Depends<IItemModelController, IQtHelpers>
	{
		IItemModelController* itemModelController() const
		{
			return get<IItemModelController>();
		}
		IQtHelpers* qtHelpers() const
		{
			return get<IQtHelpers>();
		}
	} dependencies_;
};

template <class BaseModel = AbstractItemModel>
struct QtItemModelTypeHelper
{
	typedef QtItemModel<QtAbstractItemModel> WrapperType;
};

template <>
struct QtItemModelTypeHelper<AbstractListModel>
{
	typedef QtItemModel<QtListModel> WrapperType;
};

template <>
struct QtItemModelTypeHelper<AbstractTreeModel>
{
	typedef QtItemModel<QtTreeModel> WrapperType;
};

template <>
struct QtItemModelTypeHelper<AbstractTableModel>
{
	typedef QtItemModel<QtTableModel> WrapperType;
};


//==============================================================================
template <class BaseModel>
QtItemModel<BaseModel>::QtItemModel(SourceType& source)
	: WGTInterfaceProvider( this )
	, source_(source)
{
	registerInterface(*this);
	auto changed = [this]() {
		this->modelChanged();
		this->childToParentIndexCache_.clear();
	};
	connections_.add(source_.connectModelChanged(changed));

	auto preReset = [this]() {
		this->beginResetModel();
	};
	connections_.add(source_.connectPreModelReset(preReset));

	auto postReset = [this]() {
		this->childToParentIndexCache_.clear();
		this->endResetModel();
		this->modelResetComplete();
	};
	connections_.add(source_.connectPostModelReset(postReset));

	auto preLayoutChanged = [this](const AbstractItemModel::ItemIndex& index) {
		auto item = source_.item(index);
		const QModelIndex modelIndex = this->createIndex(index.row_, index.column_, item);

		QList<QPersistentModelIndex> parents;
		parents.append(modelIndex.isValid() ? modelIndex : QModelIndex());
		this->layoutAboutToBeChanged(parents, QAbstractItemModel::VerticalSortHint);
	};
	connections_.add(source_.connectPreLayoutChanged(preLayoutChanged));

	auto postLayoutChanged = [this](const AbstractItemModel::ItemIndex& index) {
		auto item = source_.item(index);
		const QModelIndex modelIndex = this->createIndex(index.row_, index.column_, item);

		QList<QPersistentModelIndex> parents;
		parents.append(modelIndex.isValid() ? modelIndex : QModelIndex());
		this->childToParentIndexCache_.clear();
		this->layoutChanged(parents, QAbstractItemModel::VerticalSortHint);
		this->layoutChangedComplete();
	};
	connections_.add(source_.connectPostLayoutChanged(postLayoutChanged));

	auto postItemData = [this](const AbstractItemModel::ItemIndex& index, ItemRole::Id roleId,
	                           const Variant& newValue) {
		auto item = source_.item(index);
		const QModelIndex modelIndex = this->createIndex(index.row_, index.column_, item);

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
	connections_.add(source_.connectPostItemDataChanged(postItemData));

	// @see AbstractItem::DataSignature
	auto postModelData = [this](int row, int column, ItemRole::Id roleId, const Variant& value) {
		bool validHorizontal = row == -1 && column >= 0 && column < source_.columnCount(nullptr);
		bool validVertical = column == -1 && row >= 0 && row < source_.rowCount(nullptr);

		if (validHorizontal)
		{
			this->headerDataChanged(Qt::Orientation::Horizontal, column, column);
		}
		else if (validVertical)
		{
			this->headerDataChanged(Qt::Orientation::Vertical, row, row);
		}
	};
	connections_.add(source_.connectPostDataChanged(postModelData));

	// @see AbstractItemModel::RangeSignature
	auto preInsert = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		auto parentItem = source_.item(parentIndex);
		const QModelIndex modelIndex = this->createIndex(parentIndex.row_, parentIndex.column_, parentItem);
		this->beginInsertRows(modelIndex.isValid() ? modelIndex : QModelIndex(), startPos, startPos + count - 1);
	};
	connections_.add(source_.connectPreRowsInserted(preInsert));

	auto postInserted = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		this->childToParentIndexCache_.clear();
		this->endInsertRows();
	};
	connections_.add(source_.connectPostRowsInserted(postInserted));

	auto preErase = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		auto parentItem = source_.item(parentIndex);
		const QModelIndex modelIndex = this->createIndex(parentIndex.row_, parentIndex.column_, parentItem);
		this->beginRemoveRows(modelIndex.isValid() ? modelIndex : QModelIndex(), startPos, startPos + count - 1);
	};
	connections_.add(source_.connectPreRowsRemoved(preErase));

	auto postErased = [this](const AbstractItemModel::ItemIndex& parentIndex, int startPos, int count) {
		this->childToParentIndexCache_.clear();
		this->endRemoveRows();
	};
	connections_.add(source_.connectPostRowsRemoved(postErased));

	auto preMove = [this](const AbstractItemModel::ItemIndex& sourceParentIndex, int sourceFirst, int sourceLast,
	                      const AbstractItemModel::ItemIndex& destinationParentIndex, int destinationRow) {
		const auto pSourceParentItem = source_.item(sourceParentIndex);
		const QModelIndex sourceModelIndex =
		this->createIndex(sourceParentIndex.row_, sourceParentIndex.column_, pSourceParentItem);
		const auto pDestinationParentItem = source_.item(destinationParentIndex);
		const QModelIndex destinationModelIndex =
		this->createIndex(destinationParentIndex.row_, destinationParentIndex.column_, pDestinationParentItem);
		const auto success =
		this->beginMoveRows(sourceModelIndex, sourceFirst, sourceLast, destinationModelIndex, destinationRow);
		TF_ASSERT(success);
	};
	connections_.add(source_.connectPreRowsMoved(preMove));

	auto postMoved = [this](const AbstractItemModel::ItemIndex& sourceParentIndex, int sourceFirst, int sourceLast,
	                        const AbstractItemModel::ItemIndex& destinationParentIndex,
	                        int destinationRow) {
		this->endMoveRows();
		this->childToParentIndexCache_.clear();
	};
	connections_.add(source_.connectPostRowsMoved(postMoved));
}

template <class BaseModel>
QtItemModel<BaseModel>::~QtItemModel()
{
}

template <class BaseModel>
const typename QtItemModel<BaseModel>::SourceType& QtItemModel<BaseModel>::source() const
{
	return static_cast<SourceType&>(source_);
}

template <class BaseModel>
typename QtItemModel<BaseModel>::SourceType& QtItemModel<BaseModel>::source()
{
	return static_cast<SourceType&>(source_);
}

template <class BaseModel>
QModelIndex QtItemModel<BaseModel>::index(int row, int column, const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	AbstractItemModel::ItemIndex itemIndex(row, column, parentItem);
	auto item = source_.item(itemIndex);
	if (item == nullptr)
	{
		return QModelIndex();
	}

	return this->createIndex(row, column, item);
}

template <class BaseModel>
QModelIndex QtItemModel<BaseModel>::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}
	return getCachedParentIndex(child);

}

template <class BaseModel>
int QtItemModel<BaseModel>::rowCount(const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	return source_.rowCount(parentItem);
}

template <class BaseModel>
int QtItemModel<BaseModel>::columnCount(const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	return source_.columnCount(parentItem);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::hasChildren(const QModelIndex& parent) const
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;

	return source_.hasChildren(parentItem);
}


//------------------------------------------------------------------------------
template <class BaseModel>
Variant QtItemModel<BaseModel>::variantData(
	QueryHelper & helper,
	const QModelIndex& index, int role) const 
{
	auto item = index.isValid() ? reinterpret_cast<AbstractItem*>(index.internalId()) : nullptr;

	if (item == nullptr)
	{
		return Variant();
	}

	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		if (role == RoleProvider::convertRole( ItemRole::itemIdId))
		{
			return intptr_t( index.internalId() );
		}
		return Variant();
	}
	return item->getData(index.row(), index.column(), roleId);
}


//------------------------------------------------------------------------------
template <class BaseModel>
QVariant QtItemModel<BaseModel>::data(const QModelIndex& index, int role) const
{
	QueryHelper helper;
	auto variant = variantData(helper, index, role);
	auto qtHelpers = dependencies_.qtHelpers();
	TF_ASSERT(qtHelpers != nullptr);
	return qtHelpers->toQVariant(variant, const_cast<QtItemModel*>(this));
}


//------------------------------------------------------------------------------
template <class BaseModel>
bool QtItemModel<BaseModel>::setData(const QModelIndex& index, const QVariant& value, int role)
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

	auto qtHelpers = dependencies_.qtHelpers();
	// Use QVariant for comparison
	// because QVariant has lower precision than Variant
	auto oldValue = qtHelpers->toQVariant(item->getData(index.row(), index.column(), roleId), this);

	// Prevent binding loops with self-assignment check
	// Note that self-assignment check will not detect floating-point error
	if (value == oldValue)
	{
		return true;
	}

	auto data = qtHelpers->toVariant(value);
	auto controller = dependencies_.itemModelController();

	// Item already uses the Command System or the Command System is not available
	if (item->hasController() || controller == nullptr)
	{
		// Set property directly
		return item->setData(index.row(), index.column(), roleId, data);
	}

	// Queue with Command System, to register undo/redo data
	auto pParent = index.parent().isValid() ? reinterpret_cast<AbstractItem*>(index.parent().internalId()) : nullptr;
	AbstractItemModel::ItemIndex dataModelIndex(index.row(), index.column(), pParent);

	return controller->setValue(source_, dataModelIndex, roleId, data);
}

template <class BaseModel>
QVariant QtItemModel<BaseModel>::headerData(int section, Qt::Orientation orientation, int role) const
{
	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return QVariant();
	}

	auto row = orientation == Qt::Vertical ? section : -1;
	auto column = orientation == Qt::Horizontal ? section : -1;
	auto data = source_.getData(row, column, roleId);
	auto qtHelpers = dependencies_.qtHelpers();
	TF_ASSERT(qtHelpers != nullptr);
	return qtHelpers->toQVariant(data, const_cast<QtItemModel*>(this));
}

template <class BaseModel>
bool QtItemModel<BaseModel>::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return false;
	}

	auto row = orientation == Qt::Vertical ? section : -1;
	auto column = orientation == Qt::Horizontal ? section : -1;
	auto qtHelpers = dependencies_.qtHelpers();
	TF_ASSERT(qtHelpers != nullptr);
	auto data = qtHelpers->toVariant(value);
	auto controller = dependencies_.itemModelController();

	// Item already uses the Command System or the Command System is not available
	if (source_.hasController() || controller == nullptr)
	{
		// Set property directly
		return source_.setData(row, column, roleId, data);
	}

	// Queue with Command System, to register undo/redo data
	return controller->setModelData(source_, row, column, roleId, data);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::insertRows(int row, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	auto controller = dependencies_.itemModelController();

	// Item already uses the Command System or the Command System is not available
	if (source_.hasController() || controller == nullptr)
	{
		// Set property directly
		return source_.insertRows(row, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return controller->insertRows(source_, row, count, parentItem);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::insertColumns(int column, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	auto controller = dependencies_.itemModelController();

	// Item already uses the Command System or the Command System is not available
	if (source_.hasController() || controller == nullptr)
	{
		// Set property directly
		return source_.insertColumns(column, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return controller->insertColumns(source_, column, count, parentItem);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::removeRows(int row, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	auto controller = dependencies_.itemModelController();

	// Item already uses the Command System or the Command System is not available
	if (source_.hasController() || controller == nullptr)
	{
		// Set property directly
		return source_.removeRows(row, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return controller->removeRows(source_, row, count, parentItem);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::removeColumns(int column, int count, const QModelIndex& parent)
{
	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	auto controller = dependencies_.itemModelController();

	// Item already uses the Command System or the Command System is not available
	if (source_.hasController() || controller == nullptr)
	{
		// Set property directly
		return source_.removeColumns(column, count, parentItem);
	}

	// Queue with Command System, to register undo/redo data
	return controller->removeColumns(source_, column, count, parentItem);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
                                      const QModelIndex& destinationParent, int destinationChild) /* override */
{
	const auto sourceParentItem =
	sourceParent.isValid() ? reinterpret_cast<AbstractItem*>(sourceParent.internalId()) : nullptr;
	const auto destinationParentItem =
	destinationParent.isValid() ? reinterpret_cast<AbstractItem*>(destinationParent.internalId()) : nullptr;
	auto controller = dependencies_.itemModelController();

	// Item already uses the Command System or the Command System is not available
	if (source_.hasController() || controller == nullptr)
	{
		// Set property directly
		return source_.moveRows(sourceParentItem, sourceRow, count, destinationParentItem, destinationChild);
	}

	// Queue with Command System, to register undo/redo data
	return controller->moveRows(source_, sourceParentItem, sourceRow, count, destinationParentItem, destinationChild);
}

//------------------------------------------------------------------------------
template <class BaseModel>
QStringList QtItemModel<BaseModel>::mimeTypes() const
{
	QStringList result;
	source_.iterateMimeTypes([&result](const char* mimeType) {
		QString qstr = mimeType;
		result.push_back(qstr);
	});
	return result;
}

//------------------------------------------------------------------------------
template <class BaseModel>
QMimeData* QtItemModel<BaseModel>::mimeData(const QModelIndexList& indexes) const
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

	MimeData mimeData = source_.mimeData(itemIndices);
	for (auto it = mimeData.begin(); it != mimeData.end(); ++it)
	{
		QByteArray buffer(it->second.data(), (int)it->second.size());
		data->setData(it->first.c_str(), buffer);
	}

	return data;
}

template <class BaseModel>
bool QtItemModel<BaseModel>::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                             const QModelIndex& parent) const
{
	TF_ASSERT(data != nullptr);

	MimeData mimeData;
	for (auto& format : data->formats())
	{
		const QByteArray& dataArr = data->data(format);
		const char* dataBytes = dataArr.constData();
		std::vector<char> buffer(dataBytes, dataBytes + dataArr.size());
		mimeData[format.toUtf8().data()] = buffer;
	}

	DropAction dropAction = DropAction::InvalidAction;
	switch (action)
	{
	case Qt::MoveAction:
		dropAction = DropAction::MoveAction;
		break;

	case Qt::CopyAction:
		dropAction = DropAction::CopyAction;
		break;

	default:
		break;
	}

	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	AbstractItemModel::ItemIndex itemIndex(row, column, parentItem);

	return source_.canDropMimeData(mimeData, dropAction, itemIndex);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                          const QModelIndex& parent)
{
	TF_ASSERT(data != nullptr);

	MimeData mimeData;
	for (auto& format : data->formats())
	{
		const QByteArray& dataArr = data->data(format);
		const char* dataBytes = dataArr.constData();
		std::vector<char> buffer(dataBytes, dataBytes + dataArr.size());
		mimeData[format.toUtf8().data()] = buffer;
	}

	DropAction dropAction = DropAction::InvalidAction;
	switch (action)
	{
	case Qt::MoveAction:
		dropAction = DropAction::MoveAction;
		break;

	case Qt::CopyAction:
		dropAction = DropAction::CopyAction;
		break;

	default:
		break;
	}

	auto parentItem = parent.isValid() ? reinterpret_cast<AbstractItem*>(parent.internalId()) : nullptr;
	AbstractItemModel::ItemIndex itemIndex(row, column, parentItem);

	return source_.dropMimeData(mimeData, dropAction, itemIndex);
}

template <class BaseModel>
Qt::ItemFlags QtItemModel<BaseModel>::flags(const QModelIndex& index) const
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

template <class BaseModel>
QHash<int, QByteArray> QtItemModel<BaseModel>::roleNames() const
{
	auto roleNames = QAbstractItemModel::roleNames();
	source_.iterateRoles([this, &roleNames](const char* roleName) { registerRole(roleName, roleNames); });
	return roleNames;
}

template <class BaseModel>
void QtItemModel<BaseModel>::revert()
{
	source_.revert();
}

template <class BaseModel>
bool QtItemModel<BaseModel>::submit()
{
	return source_.submit();
}

template <class BaseModel>
QModelIndex QtItemModel<BaseModel>::findIndex(QVariant id) const
{
	auto item = reinterpret_cast<AbstractItem*>(id.value<quintptr>());
	if (item == nullptr)
	{
		return QModelIndex();
	}

	AbstractItemModel::ItemIndex index;
	source_.index(item, index);
	if (!index.isValid())
	{
		return QModelIndex();
	}

	return this->createIndex(index.row_, index.column_, item);
}

template <class BaseModel>
bool QtItemModel<BaseModel>::encodeRole(ItemRole::Id roleId, int& o_Role) const
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

template <class BaseModel>
bool QtItemModel<BaseModel>::decodeRole(int role, ItemRole::Id& o_RoleId) const
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

template <class BaseModel>
QtItemModel<BaseModel>* QtItemModel<BaseModel>::fromQVariant(const QVariant& variant)
{
	if (!variant.canConvert<QtItemModel<BaseModel>*>())
	{
		return nullptr;
	}

	// The following doesn't work because it requires QtItemModel to have the Q_OBJECT macro.
	// auto model = qVariant.value<QtItemModel<QtTableModel>*>();

	auto baseModel = variant.value<QtAbstractItemModel*>();
	return static_cast<QtItemModel<BaseModel>*>(baseModel);
}

template <class BaseModel>
QModelIndex QtItemModel<BaseModel>::getCachedParentIndex(const QModelIndex &child) const
{
	auto found = childToParentIndexCache_.find(child);
	if (found != childToParentIndexCache_.end())
	{
		return found->second;
	}

	auto childItem = reinterpret_cast<AbstractItem*>(child.internalId());
	AbstractItemModel::ItemIndex childIndex;
	source_.index(childItem, childIndex);
	if (!childIndex.isValid())
	{
		return childToParentIndexCache_[child] = QModelIndex();
	}

	auto parentItem = const_cast<AbstractItem*>(childIndex.parent_);
	if (parentItem == nullptr)
	{
		return childToParentIndexCache_[child] = QModelIndex();
	}

	AbstractItemModel::ItemIndex parentIndex;
	source_.index(parentItem, parentIndex);
	if (!parentIndex.isValid())
	{
		return childToParentIndexCache_[child] = QModelIndex();
	}

	return childToParentIndexCache_[child] = this->createIndex(parentIndex.row_, parentIndex.column_, parentItem);
}
} // end namespace wgt
#endif // QT_ITEM_MODEL_HPP
