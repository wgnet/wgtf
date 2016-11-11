#include "wg_item_view.hpp"
#include "qt_connection_holder.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "models/extensions/i_model_extension.hpp"
#include "models/qt_abstract_item_model.hpp"

#include <functional>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QDataStream>
#include <QQmlListProperty>
#include <QString>
#include <QUuid>
#include <QtAlgorithms>
#include <QMimeData>

#include <private/qmetaobjectbuilder_p.h>
#include "wg_types/base64.hpp"

namespace wgt
{
ITEMROLE(modelIndex)

namespace WGItemViewDetails
{
const char* itemMimeKey = "application/model-item";
const char* horizontalHeaderMimeKey = "application/model-horizontalHeader";
const char* verticalHeaderMimeKey = "application/model-verticalHeader";
}

namespace
{
class ExtensionData : public IExtensionData
{
	typedef QMap<ItemRole::Id, QVariant> ItemData;

public:
	ExtensionData(QAbstractItemModel* extendedModel) : extendedModel_(extendedModel), internalModel_(nullptr)
	{
	}

	void save()
	{
		persistentData_[internalModel_] = qMakePair(itemData_, indexCache_);
		// TODO save to preference
	}

	void load()
	{
		auto findIt = persistentData_.find(internalModel_);
		if (findIt == persistentData_.end())
		{
			itemData_.clear();
			indexCache_.clear();
			return;
		}
		itemData_ = findIt.value().first;
		indexCache_ = findIt.value().second;
		// TODO load from preference
	}

	void reset(QAbstractItemModel* internalModel)
	{
		save();
		internalModel_ = internalModel;
		load();
	}

	bool encodeRole(ItemRole::Id roleId, int& o_Role) const override
	{
		auto model = internalModel_;
		while (model != nullptr)
		{
			auto roleProvider = dynamic_cast<RoleProvider*>(model);
			if (roleProvider != nullptr && roleProvider->encodeRole(roleId, o_Role))
			{
				return true;
			}

			auto extendedModel = dynamic_cast<QAbstractProxyModel*>(model);
			model = extendedModel != nullptr ? extendedModel->sourceModel() : nullptr;
		}

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

		if (internalModel_ != nullptr)
		{
			auto roleNames = internalModel_->roleNames();
			for (auto it = roleNames.begin(); it != roleNames.end(); ++it)
			{
				if (roleId == ItemRole::compute(it.value().data()))
				{
					o_Role = it.key();
					return true;
				}
			}
		}

		return false;
	}

	bool decodeRole(int role, ItemRole::Id& o_RoleId) const override
	{
		auto model = internalModel_;
		while (model != nullptr)
		{
			auto roleProvider = dynamic_cast<RoleProvider*>(model);
			if (roleProvider != nullptr && roleProvider->decodeRole(role, o_RoleId))
			{
				return true;
			}

			auto extendedModel = dynamic_cast<QAbstractProxyModel*>(model);
			model = extendedModel != nullptr ? extendedModel->sourceModel() : nullptr;
		}

		if (role == Qt::DisplayRole)
		{
			o_RoleId = ItemRole::displayId;
			return true;
		}
		if (role == Qt::DecorationRole)
		{
			o_RoleId = ItemRole::decorationId;
			return true;
		}

		if (internalModel_ != nullptr)
		{
			auto roleNames = internalModel_->roleNames();
			for (auto it = roleNames.begin(); it != roleNames.end(); ++it)
			{
				if (role == it.key())
				{
					o_RoleId = ItemRole::compute(it.value().data());
					return true;
				}
			}
		}

		return false;
	}

	bool encodeRoleExt(ItemRole::Id roleId, int& o_Role) const override
	{
		auto roleProvider = dynamic_cast<RoleProvider*>(extendedModel_);
		assert(roleProvider != nullptr);
		return roleProvider->encodeRole(roleId, o_Role);
	}

	bool decodeRoleExt(int role, ItemRole::Id& o_RoleId) const override
	{
		auto roleProvider = dynamic_cast<RoleProvider*>(extendedModel_);
		assert(roleProvider != nullptr);
		return roleProvider->decodeRole(role, o_RoleId);
	}

	QVariant data(const QModelIndex& index, ItemRole::Id roleId) override
	{
		int role;
		if (!encodeRole(roleId, role))
		{
			return QVariant();
		}
		return extendedModel_->data(index, role);
	}

	bool setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId) override
	{
		int role;
		if (!encodeRole(roleId, role))
		{
			return false;
		}
		return extendedModel_->setData(index, value, role);
	}

	QVariant dataExt(const QModelIndex& index, ItemRole::Id roleId) override
	{
		auto& itemData = getItemData(index);
		return itemData[roleId];
	}

	bool setDataExt(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId) override
	{
		auto& itemData = getItemData(index);
		if (itemData[roleId] == value)
		{
			return false;
		}
		itemData[roleId] = value;

		QVector<int> roles;
		int role;
		if (encodeRoleExt(roleId, role))
		{
			roles.append(role);
			emit extendedModel_->dataChanged(index, index, roles);
		}
		return true;
	}

	void dataExtChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
	                    const QVector<ItemRole::Id> roleIds) override
	{
		QVector<int> roles;
		for (auto roleId : roleIds)
		{
			int role;
			auto res = encodeRoleExt(roleId, role);
			assert(res);
			roles.append(role);
		}
		emit extendedModel_->dataChanged(topLeft, bottomRight, roles);
	}

private:
	ItemData& getItemData(const QModelIndex& index)
	{
		auto it = indexCache_.find(index);
		if (it != indexCache_.end())
		{
			return itemData_[*it];
		}

		indexCache_[index] = itemData_.count();
		itemData_.push_back(ItemData());
		return itemData_.back();
	}

private:
	QAbstractItemModel* extendedModel_;
	QAbstractItemModel* internalModel_;

	QVector<ItemData> itemData_;
	QMap<QPersistentModelIndex, int> indexCache_;
	QMap<QAbstractItemModel*, QPair<QVector<ItemData>, QMap<QPersistentModelIndex, int>>> persistentData_;
};

class ExtendedModel : public QtAbstractItemModel, public RoleProvider
{
	DECLARE_QT_MEMORY_HANDLER
public:
	ExtendedModel(QList<IModelExtension*>& extensions) : model_(nullptr), extensions_(extensions), extensionData_(this)
	{
	}

	void reset(QAbstractItemModel* model)
	{
		beginResetModel();
		model_ = model;
		connections_.reset();
		roleNames_.clear();
		extensionData_.reset(model);
		columnMappings_.clear();
		if (model_ != nullptr)
		{
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::dataChanged, this, &ExtendedModel::onDataChanged);
			connections_ += QObject::connect(model, &QAbstractItemModel::layoutAboutToBeChanged, this,
			                                 &ExtendedModel::onLayoutAboutToBeChanged);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::layoutChanged, this, &ExtendedModel::onLayoutChanged);
			connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this,
			                                 &ExtendedModel::onRowsAboutToBeInserted);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::rowsInserted, this, &ExtendedModel::onRowsInserted);
			connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this,
			                                 &ExtendedModel::onRowsAboutToBeRemoved);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, &ExtendedModel::onRowsRemoved);
			connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this,
			                                 &ExtendedModel::onRowsAboutToBeMoved);
			connections_ += QObject::connect(model, &QAbstractItemModel::rowsMoved, this, &ExtendedModel::onRowsMoved);
			connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeInserted, this,
			                                 &ExtendedModel::onColumnsAboutToBeInserted);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::columnsInserted, this, &ExtendedModel::onColumnsInserted);
			connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeRemoved, this,
			                                 &ExtendedModel::onColumnsAboutToBeRemoved);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::columnsRemoved, this, &ExtendedModel::onColumnsRemoved);
			connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeMoved, this,
			                                 &ExtendedModel::onColumnsAboutToBeMoved);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::columnsMoved, this, &ExtendedModel::onColumnsMoved);

			for (auto& extension : extensions_)
			{
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::dataChanged, extension, &IModelExtension::onDataChanged);
				connections_ += QObject::connect(this, &QAbstractItemModel::layoutAboutToBeChanged, extension,
				                                 &IModelExtension::onLayoutAboutToBeChanged);
				connections_ += QObject::connect(this, &QAbstractItemModel::layoutChanged, extension,
				                                 &IModelExtension::onLayoutChanged);
				connections_ += QObject::connect(this, &QAbstractItemModel::rowsAboutToBeInserted, extension,
				                                 &IModelExtension::onRowsAboutToBeInserted);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::rowsInserted, extension, &IModelExtension::onRowsInserted);
				connections_ += QObject::connect(this, &QAbstractItemModel::rowsAboutToBeRemoved, extension,
				                                 &IModelExtension::onRowsAboutToBeRemoved);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::rowsRemoved, extension, &IModelExtension::onRowsRemoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::rowsAboutToBeMoved, extension,
				                                 &IModelExtension::onRowsAboutToBeMoved);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::rowsMoved, extension, &IModelExtension::onRowsMoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsAboutToBeInserted, extension,
				                                 &IModelExtension::onColumnsAboutToBeInserted);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsInserted, extension,
				                                 &IModelExtension::onColumnsInserted);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsAboutToBeRemoved, extension,
				                                 &IModelExtension::onColumnsAboutToBeRemoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsRemoved, extension,
				                                 &IModelExtension::onColumnsRemoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsAboutToBeMoved, extension,
				                                 &IModelExtension::onColumnsAboutToBeMoved);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::columnsMoved, extension, &IModelExtension::onColumnsMoved);
			}

			roleNames_ = model_->roleNames();
			registerRole(ItemRole::modelIndexName, roleNames_);
			for (auto& extension : extensions_)
			{
				auto& roles = extension->roles();
				for (auto& role : roles)
				{
					registerRole(role.c_str(), roleNames_);
				}
			}

			for (auto& extension : extensions_)
			{
				extension->init(extensionData_);
			}

			auto columnCount = model_->columnCount();
			for (auto i = 0; i < columnCount; ++i)
			{
				columnMappings_.append(QPair<int, bool>(i, true));
			}
		}
		endResetModel();
	}

	QStringList mimeTypes() const override
	{
		if (model_ == nullptr)
		{
			return QStringList();
		}

		QStringList result = model_->mimeTypes();
		result.push_back(WGItemViewDetails::itemMimeKey);
		return result;
	}

	QMimeData* mimeData(const QModelIndexList& indexes) const override
	{
		if (model_ == nullptr)
		{
			return nullptr;
		}

		auto extendedData = new QMimeData();

		QByteArray buffer;
		QDataStream stream(&buffer, QIODevice::WriteOnly);
		stream << indexes.size();
		for (auto index : indexes)
		{
			stream << index.row();
			stream << index.column();
			stream << index.internalId();
		}
		std::string encodedIndexes = Base64::encode(buffer.constData(), buffer.length());
		extendedData->setData(WGItemViewDetails::itemMimeKey, encodedIndexes.c_str());

		QModelIndexList sourceIndexes;
		for (auto& index : indexes)
		{
			sourceIndexes.append(sourceIndex(index));
		}
		auto modelData = model_->mimeData(sourceIndexes);
		if (modelData != nullptr)
		{
			for (auto format : modelData->formats())
			{
				extendedData->setData(format, modelData->data(format));
			}
			delete modelData;
		}

		return extendedData;
	}

	bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
	                     const QModelIndex& parent) const override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		auto sourceParent = sourceIndex(parent);
		if (model_->canDropMimeData(data, action, row, sourceColumn(column), sourceParent))
		{
			return true;
		}

		return data->hasFormat(WGItemViewDetails::itemMimeKey) && action == Qt::MoveAction;
	}

	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
	                  const QModelIndex& parent) override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		auto sourceParent = sourceIndex(parent);
		if (model_->canDropMimeData(data, action, row, sourceColumn(column), sourceParent))
		{
			return model_->dropMimeData(data, action, row, sourceColumn(column), sourceParent);
		}

		bool canDrop = (data->hasFormat(WGItemViewDetails::itemMimeKey) && action == Qt::MoveAction);
		if (!canDrop)
		{
			return false;
		}

		QModelIndexList indexes;
		std::string decoded;
		Base64::decode(data->data(WGItemViewDetails::itemMimeKey).data(), decoded);
		QByteArray buffer(decoded.c_str(), static_cast<int>(decoded.length()));
		QDataStream stream(buffer);
		int numIndexes;
		stream >> numIndexes;
		for (int i = 0; i < numIndexes; ++i)
		{
			int row;
			stream >> row;
			int column;
			stream >> column;
			quintptr internalId;
			stream >> internalId;
			indexes.append(createIndex(row, column, internalId));
		}

		QList<QPersistentModelIndex> persistentIndexes;
		for (auto& index : indexes)
		{
			if (index.parent() != parent)
			{
				return true;
			}
			persistentIndexes.append(index);
		}
		std::sort(persistentIndexes.begin(), persistentIndexes.end());
		if (persistentIndexes.empty())
		{
			return true;
		}
		QPersistentModelIndex persistentDestination =
		index(row > persistentIndexes[0].row() ? row + 1 : row, column, parent);

		for (QModelIndex index : persistentIndexes)
		{
			model_->moveRow(sourceParent, index.row(), sourceParent, persistentDestination.row());
		}
		return true;
	}

	bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent,
	              int destinationChild) override
	{
		assert(false && "Not Implemented");
		return false;
	}

	bool moveColumns(const QModelIndex& sourceParent, int sourceColumn, int count, const QModelIndex& destinationParent,
	                 int destinationChild) override
	{
		assert(!sourceParent.isValid() && !destinationParent.isValid());
		if (!beginMoveColumns(sourceParent, sourceColumn, sourceColumn + count - 1, destinationParent,
		                      destinationChild))
		{
			return false;
		}

		auto sourceOffset = 0;
		auto destinationOffset = 0;
		for (int i = 0; i < columnMappings_.size(); ++i)
		{
			if (!columnMappings_[i].second || columnMappings_[i].first < 0)
			{
				if (i <= sourceColumn)
				{
					++sourceOffset;
				}
				if (i <= destinationChild)
				{
					++destinationChild;
				}
			}
		}

		for (int i = 0; i < count; ++i)
		{
			auto source = sourceColumn + sourceOffset;
			auto destination = destinationChild + destinationOffset;

			if (destination > source)
			{
				columnMappings_.move(source, destination - 1);
				do
				{
					++destinationOffset;
				} while (!columnMappings_[destinationOffset].second || columnMappings_[destinationOffset].first < 0);
			}
			else
			{
				columnMappings_.move(source, destination);
				do
				{
					++sourceOffset;
				} while (!columnMappings_[sourceOffset].second || columnMappings_[sourceOffset].first < 0);
			}
		}
		endMoveColumns();
		return true;
	}

	int sourceColumn(int index) const
	{
		for (auto& columnMapping : columnMappings_)
		{
			if (!columnMapping.second || columnMapping.first < 0)
			{
				continue;
			}

			if (index-- == 0)
			{
				return columnMapping.first;
			}
		}
		return -1;
	}

	int extendedColumn(int index) const
	{
		int column = 0;
		for (auto& columnMapping : columnMappings_)
		{
			if (!columnMapping.second || columnMapping.first < 0)
			{
				continue;
			}

			if (columnMapping.first == index)
			{
				return column;
			}

			++column;
		}
		return -1;
	}

	void hideColumn(int index)
	{
		layoutAboutToBeChanged();
		int first = index;
		int last = index;
		for (auto& columnMapping : columnMappings_)
		{
			if (!columnMapping.second || columnMapping.first < 0)
			{
				continue;
			}

			if (index-- == 0)
			{
				beginRemoveColumns(QModelIndex(), first, last);
				columnMapping.second = false;
				endRemoveColumns();
				break;
			}
		}
		layoutChanged();
		headerDataChanged(Qt::Horizontal, index, columnCount(QModelIndex()));
	}

	void showAllColumns()
	{
		layoutAboutToBeChanged();
		int first = -1;
		int last = -1;
		int column = 0;
		int index = 0;
		for (auto& columnMapping : columnMappings_)
		{
			if (columnMapping.first < 0)
			{
				continue;
			}

			if (!columnMapping.second)
			{
				if (first == -1)
				{
					first = column;
				}
				last = column;
			}
			else if (first != -1)
			{
				beginInsertColumns(QModelIndex(), first, last);
				for (; index < columnMappings_.size();)
				{
					columnMappings_[index].second = true;
					if (&columnMappings_[index++] == &columnMapping)
					{
						break;
					}
				}
				endInsertColumns();
				first = -1;
				last = -1;
			}

			++column;
		}

		if (first != -1)
		{
			beginInsertColumns(QModelIndex(), first, last);
			for (; index < columnMappings_.size(); ++index)
			{
				columnMappings_[index].second = true;
			}
			endInsertColumns();
		}
		layoutChanged();
		headerDataChanged(Qt::Horizontal, 0, columnCount(QModelIndex()));
	}

	QModelIndex sourceIndex(const QModelIndex& extendedIndex, int row = -1, int column = -1) const
	{
		if (!extendedIndex.isValid())
		{
			return QModelIndex();
		}

		assert(extendedIndex.model() == this);
		// To convert from an extended modelIndex to an internal modelIndex we have 2 options -
		// 1. Use the public index functions on model_ using the row and column of the extended modelIndex.
		//    The problem with this however is that this requires the parent of the extended modelIndex,
		//    however to calculate the parent of the extended modelIndex we first need to convert the
		//    extended modelIndex to an internal modelIndex and we get stuck in a loop
		// 2. Create a map of internal persistentModelIndices to extended persistentModelIndices that is
		//    populated by calls to extendedIndex and used as a lookup in modelIndex.
		//    The problem with this is that extendedIndex is called by the index function of the extended model
		//    and the index function is called whenever a remap of persistent indices is required (eg a call to
		//    beginRemoveRows/endRemoveRows). Instantiating a persistent index during a remap of a models
		//    persistent indices results in a corruption of the persistent index table and asserts during destruction
		// As such we need to resort to a third option -
		// 3. Access the protected createIndex function of model_ and call it directly.
		//    This is actually the most performant of all options and is completely safe with the current implementation
		//    of the QAbstractItemModel classes. All the createIndex function does is invoke the private constructor
		//    of QModelIndex, passing the models this pointer in as an argument.
		//    To do this we take the address of the extended models createIndex function and bind the this pointer
		//    to model_. Dodgy but necessary.
		QModelIndex (QAbstractItemModel::*createIndexFunc)(int, int, void*) const = &ExtendedModel::createIndex;
		using namespace std::placeholders;
		auto createIndex = std::bind(createIndexFunc, model_, _1, _2, _3);
		return createIndex(row == -1 ? extendedIndex.row() : row,
		                   column == -1 ? sourceColumn(extendedIndex.column()) : column,
		                   extendedIndex.internalPointer());
	}

	QModelIndex extendedIndex(const QModelIndex& modelIndex, int row = -1, int column = -1) const
	{
		if (!modelIndex.isValid())
		{
			return QModelIndex();
		}

		if (modelIndex.model() == this)
		{
			return modelIndex;
		}

		assert(modelIndex.model() == model_);
		return createIndex(row == -1 ? modelIndex.row() : row,
		                   column == -1 ? extendedColumn(modelIndex.column()) : column, modelIndex.internalId());
	}

private:
	QModelIndex index(int row, int column, const QModelIndex& parent) const override
	{
		if (model_ == nullptr)
		{
			return QModelIndex();
		}

		return extendedIndex(model_->index(row, sourceColumn(column), sourceIndex(parent, -1, 0)));
	}

	QModelIndex parent(const QModelIndex& child) const override
	{
		if (model_ == nullptr)
		{
			return QModelIndex();
		}

		return extendedIndex(model_->parent(sourceIndex(child)), -1, 0);
	}

	int rowCount(const QModelIndex& parent) const override
	{
		if (model_ == nullptr)
		{
			return 0;
		}

		return model_->rowCount(sourceIndex(parent, -1, 0));
	}

	int columnCount(const QModelIndex& parent) const override
	{
		int columnCount = 0;
		for (auto& columnMapping : columnMappings_)
		{
			if (!columnMapping.second || columnMapping.first < 0)
			{
				continue;
			}
			++columnCount;
		}
		return columnCount;
	}

	bool insertRows(int row, int count, const QModelIndex& parent) override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		return model_->insertRows(row, count, sourceIndex(parent, -1, 0));
	}

	bool insertColumns(int column, int count, const QModelIndex& parent) override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		return model_->insertColumns(column, count, sourceIndex(parent, -1, 0));
	}

	bool removeRows(int row, int count, const QModelIndex& parent) override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		return model_->removeRows(row, count, sourceIndex(parent, -1, 0));
	}

	bool removeColumns(int column, int count, const QModelIndex& parent) override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		return model_->removeColumns(column, count, sourceIndex(parent, -1, 0));
	}

	bool hasChildren(const QModelIndex& parent) const override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		return model_->hasChildren(sourceIndex(parent, -1, 0));
	}

	QVariant data(const QModelIndex& index, int role) const override
	{
		if (model_ == nullptr)
		{
			return QVariant();
		}

		ItemRole::Id roleId;
		if (decodeRole(role, roleId))
		{
			if (roleId == ItemRole::modelIndexId)
			{
				return index;
			}
			for (auto& extension : extensions_)
			{
				auto data = extension->data(index, roleId);
				if (data.isValid())
				{
					return data;
				}
			}
		}

		return model_->data(sourceIndex(index), role);
	}

	bool setData(const QModelIndex& index, const QVariant& value, int role) override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		ItemRole::Id roleId;
		if (decodeRole(role, roleId))
		{
			for (auto& extension : extensions_)
			{
				if (extension->setData(index, value, roleId))
				{
					return true;
				}
			}
		}

		return model_->setData(sourceIndex(index), value, role);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override
	{
		if (model_ == nullptr)
		{
			return QVariant();
		}

		ItemRole::Id roleId;
		if (decodeRole(role, roleId))
		{
			for (auto& extension : extensions_)
			{
				auto data = extension->headerData(section, orientation, roleId);
				if (data.isValid())
				{
					return data;
				}
			}
		}

		return model_->headerData(orientation == Qt::Horizontal ? sourceColumn(section) : section, orientation, role);
	}

	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role) override
	{
		if (model_ == nullptr)
		{
			return false;
		}

		ItemRole::Id roleId;
		if (decodeRole(role, roleId))
		{
			for (auto& extension : extensions_)
			{
				if (extension->setHeaderData(section, orientation, value, roleId))
				{
					return true;
				}
			}
		}

		return model_->setHeaderData(orientation == Qt::Horizontal ? sourceColumn(section) : section, orientation,
		                             value, role);
	}

	QHash<int, QByteArray> roleNames() const override
	{
		return roleNames_;
	}

	void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
	{
		QVector<int> encodedRoles;
		for (auto& role : roles)
		{
			int encodedRole;
			encodedRoles.append(encodeRole(role, encodedRole) ? encodedRole : role);
		}
		dataChanged(extendedIndex(topLeft), extendedIndex(bottomRight), encodedRoles);
	}

	void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                              QAbstractItemModel::LayoutChangeHint hint)
	{
		QList<QPersistentModelIndex> extendedParents;
		for (auto& parent : parents)
		{
			extendedParents.append(extendedIndex(parent));
		}
		layoutAboutToBeChanged(extendedParents, hint);
	}

	void onLayoutChanged(const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint)
	{
		QList<QPersistentModelIndex> extendedParents;
		for (auto& parent : parents)
		{
			extendedParents.append(extendedIndex(parent));
		}
		layoutChanged(extendedParents, hint);
	}

	void onRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
	{
		beginInsertRows(extendedIndex(parent), first, last);
	}

	void onRowsInserted(const QModelIndex& parent, int first, int last)
	{
		endInsertRows();
		headerDataChanged(Qt::Vertical, first, rowCount(QModelIndex()));
	}

	void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
		beginRemoveRows(extendedIndex(parent), first, last);
	}

	void onRowsRemoved(const QModelIndex& parent, int first, int last)
	{
		endRemoveRows();
		headerDataChanged(Qt::Vertical, first, rowCount(QModelIndex()));
	}

	void onRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                          const QModelIndex& destinationParent, int destinationRow)
	{
		beginMoveRows(extendedIndex(sourceParent), sourceFirst, sourceLast, extendedIndex(destinationParent),
		              destinationRow);
	}

	void onRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                 const QModelIndex& destinationParent, int destinationRow)
	{
		endMoveRows();
		headerDataChanged(Qt::Vertical, 0, rowCount(QModelIndex()));
	}

	void onColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last)
	{
		assert(!parent.isValid());
	}

	void onColumnsInserted(const QModelIndex& parent, int first, int last)
	{
		assert(!parent.isValid());
		beginInsertColumns(QModelIndex(), first, last);
		int count = last - first + 1;
		for (auto& columnMapping : columnMappings_)
		{
			if (columnMapping.first >= first)
			{
				columnMapping.first += count;
			}
		}
		for (int i = 0; i < count; ++i)
		{
			columnMappings_.insert(first + i, QPair<int, bool>(first + i, true));
		}
		endInsertColumns();
		headerDataChanged(Qt::Horizontal, first, columnCount(QModelIndex()));
	}

	void onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
		assert(!parent.isValid());
		int count = last - first + 1;
		for (auto& columnMapping : columnMappings_)
		{
			if (columnMapping.first >= first + count)
			{
				columnMapping.first -= count;
			}
			else if (columnMapping.first >= first)
			{
				columnMapping.first = -1;
			}
		}

		first = -1;
		last = -1;
		for (int i = 0; i < columnMappings_.size(); ++i)
		{
			if (columnMappings_[i].first == -1)
			{
				if (first == -1)
				{
					first = i;
				}
				last = i;
				continue;
			}

			if (first != -1)
			{
				beginRemoveColumns(QModelIndex(), first, last);
				columnMappings_.erase(columnMappings_.begin() + first, columnMappings_.begin() + last + 1);
				endRemoveColumns();
				i -= last - first + 1;
				first = -1;
				last = -1;
			}
		}
		if (first != -1)
		{
			beginRemoveColumns(QModelIndex(), first, last);
			columnMappings_.erase(columnMappings_.begin() + first, columnMappings_.end());
			endRemoveColumns();
		}
	}

	void onColumnsRemoved(const QModelIndex& parent, int first, int last)
	{
		assert(!parent.isValid());
		headerDataChanged(Qt::Horizontal, first, columnCount(QModelIndex()));
	}

	void onColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                             const QModelIndex& destinationParent, int destinationColumn)
	{
		assert(!sourceParent.isValid() && !destinationParent.isValid());
		layoutAboutToBeChanged(QList<QPersistentModelIndex>(), QAbstractItemModel::HorizontalSortHint);
	}

	void onColumnsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                    const QModelIndex& destinationParent, int destinationColumn)
	{
		assert(!sourceParent.isValid() && !destinationParent.isValid());
		layoutChanged(QList<QPersistentModelIndex>(), QAbstractItemModel::HorizontalSortHint);
		headerDataChanged(Qt::Horizontal, 0, columnCount(QModelIndex()));
	}

	QAbstractItemModel* model_;
	QtConnectionHolder connections_;
	QList<IModelExtension*>& extensions_;
	QHash<int, QByteArray> roleNames_;
	ExtensionData extensionData_;
	QList<QPair<int, bool>> columnMappings_;
};

class HeaderData : public QObject
{
	DECLARE_QT_MEMORY_HANDLER
public:
	HeaderData(QAbstractItemModel& model, int section, Qt::Orientation orientation)
	    : model_(model), section_(section), orientation_(orientation)
	{
		QMetaObjectBuilder builder;
		builder.setClassName(QUuid().toByteArray());
		builder.setSuperClass(&QObject::staticMetaObject);

		QHashIterator<int, QByteArray> itr(model_.roleNames());
		while (itr.hasNext())
		{
			itr.next();
			roles_.append(itr.key());
			auto property = builder.addProperty(itr.value(), "QVariant");
			property.setNotifySignal(builder.addSignal(itr.value() + "Changed(QVariant)"));
		}

		metaObject_ = builder.toMetaObject();
	}
	~HeaderData()
	{
		free(metaObject_);
		metaObject_ = nullptr;
	}

private:
	const QMetaObject* metaObject() const override
	{
		return metaObject_;
	}

	int qt_metacall(QMetaObject::Call c, int id, void** argv) override
	{
		id = QObject::qt_metacall(c, id, argv);
		if (id < 0)
		{
			return id;
		}

		switch (c)
		{
		case QMetaObject::InvokeMetaMethod:
		{
			auto methodCount = metaObject_->methodCount() - metaObject_->methodOffset();
			if (id < methodCount)
			{
				metaObject_->activate(this, id + metaObject_->methodOffset(), argv);
			}
			id -= methodCount;
			break;
		}
		case QMetaObject::ReadProperty:
		case QMetaObject::WriteProperty:
		{
			auto propertyCount = metaObject_->propertyCount() - metaObject_->propertyOffset();
			if (id < propertyCount)
			{
				auto value = reinterpret_cast<QVariant*>(argv[0]);
				auto role = roles_[id];
				if (c == QMetaObject::ReadProperty)
				{
					*value = model_.headerData(section_, orientation_, role);
				}
				else
				{
					model_.setHeaderData(section_, orientation_, *value, role);
				}
			}
			id -= propertyCount;
			break;
		}
		default:
			break;
		}

		return id;
	}

	QAbstractItemModel& model_;
	int section_;
	Qt::Orientation orientation_;
	QList<int> roles_;
	QMetaObject* metaObject_;
};
}

struct WGItemView::Impl
{
	Impl() : model_(nullptr)
	{
	}

	QAbstractItemModel* model_;
	QtConnectionHolder connections_;
	QList<IModelExtension*> extensions_;
	std::unique_ptr<ExtendedModel> extendedModel_;
	QList<QObject*> headerData_;
};

WGItemView::WGItemView() : impl_(new Impl())
{
	impl_->extendedModel_.reset(new ExtendedModel(impl_->extensions_));

	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::modelReset, [&]() {
		rowCountChanged();
		columnCountChanged();
		refreshHeaderData();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::rowsInserted, [&]() {
		rowCountChanged();
		refreshHeaderData();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::rowsRemoved, [&]() {
		rowCountChanged();
		refreshHeaderData();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::rowsMoved, [&]() { refreshHeaderData(); });
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::columnsInserted, [&]() {
		columnCountChanged();
		refreshHeaderData();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::columnsRemoved, [&]() {
		columnCountChanged();
		refreshHeaderData();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::columnsMoved, [&]() { refreshHeaderData(); });
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::headerDataChanged,
	                 [&](Qt::Orientation orientation) {
		                 if (orientation == Qt::Horizontal)
		                 {
			                 emit headerDataChanged();
		                 }
		             });
}

WGItemView::~WGItemView()
{
	qDeleteAll(impl_->headerData_);
	impl_->headerData_.clear();
	emit headerDataChanged();
}

QAbstractItemModel* WGItemView::getModel() const
{
	return impl_->model_;
}

void WGItemView::setModel(QAbstractItemModel* model)
{
	impl_->model_ = model;
	emit modelChanged();

	impl_->extendedModel_->reset(impl_->model_);
	impl_->connections_.reset();
	if (impl_->model_ != nullptr)
	{
		impl_->connections_ += QObject::connect(impl_->model_, &QAbstractItemModel::modelReset,
		                                        [&]() { impl_->extendedModel_->reset(impl_->model_); });
	}
}

QQmlListProperty<IModelExtension> WGItemView::getExtensions() const
{
	return QQmlListProperty<IModelExtension>(const_cast<WGItemView*>(this), nullptr, &appendExtension, &countExtensions,
	                                         &extensionAt, &clearExtensions);
}

void WGItemView::appendExtension(QQmlListProperty<IModelExtension>* property, IModelExtension* value)
{
	auto itemView = qobject_cast<WGItemView*>(property->object);
	if (itemView == nullptr)
	{
		return;
	}

	itemView->impl_->extensions_.append(value);
	itemView->impl_->extendedModel_->reset(itemView->impl_->model_);
}

int WGItemView::countExtensions(QQmlListProperty<IModelExtension>* property)
{
	auto itemView = qobject_cast<WGItemView*>(property->object);
	if (itemView == nullptr)
	{
		return 0;
	}

	return itemView->impl_->extensions_.count();
}

IModelExtension* WGItemView::extensionAt(QQmlListProperty<IModelExtension>* property, int index)
{
	auto itemView = qobject_cast<WGItemView*>(property->object);
	if (itemView == nullptr)
	{
		return nullptr;
	}

	return itemView->impl_->extensions_[index];
}

void WGItemView::clearExtensions(QQmlListProperty<IModelExtension>* property)
{
	auto itemView = qobject_cast<WGItemView*>(property->object);
	if (itemView == nullptr)
	{
		return;
	}

	itemView->impl_->extensions_.clear();
	itemView->impl_->extendedModel_->reset(itemView->impl_->model_);
}

QAbstractItemModel* WGItemView::getExtendedModel() const
{
	return impl_->extendedModel_.get();
}

QList<QObject*> WGItemView::getHeaderData() const
{
	return impl_->headerData_;
}

int WGItemView::getRowCount() const
{
	QtAbstractItemModel* extendedModel = impl_->extendedModel_.get();
	assert(extendedModel != nullptr);
	return extendedModel->rowCount(nullptr);
}

int WGItemView::getColumnCount() const
{
	QtAbstractItemModel* extendedModel = impl_->extendedModel_.get();
	assert(extendedModel != nullptr);
	return extendedModel->columnCount(nullptr);
}

void WGItemView::refreshHeaderData()
{
	// TODO: this is terribly inefficient. This function gets called more often than it needs to be
	// and the HeaderData object could be made much more lightweight.
	qDeleteAll(impl_->headerData_);
	impl_->headerData_.clear();
	QtAbstractItemModel* extendedModel = impl_->extendedModel_.get();
	assert(extendedModel != nullptr);
	auto columnCount = extendedModel->columnCount();
	for (auto i = 0; i < columnCount; i++)
	{
		impl_->headerData_.append(new HeaderData(*extendedModel, i, Qt::Horizontal));
	}
	emit headerDataChanged();
}

int WGItemView::getRow(const QModelIndex& index) const
{
	return index.row();
}

int WGItemView::getColumn(const QModelIndex& index) const
{
	return index.column();
}

QModelIndex WGItemView::getParent(const QModelIndex& index) const
{
	return index.parent();
}

int WGItemView::sourceColumn(int index) const
{
	return impl_->extendedModel_->sourceColumn(index);
}

void WGItemView::hideColumn(int index)
{
	impl_->extendedModel_->hideColumn(index);
}

void WGItemView::showAllColumns()
{
	impl_->extendedModel_->showAllColumns();
}

QModelIndex WGItemView::sourceIndex(const QModelIndex& index) const
{
	return impl_->extendedModel_->sourceIndex(index);
}

QModelIndex WGItemView::extendedIndex(const QModelIndex& index) const
{
	return impl_->extendedModel_->extendedIndex(index);
}

QStringList WGItemView::mimeTypes() const
{
	return impl_->extendedModel_->mimeTypes();
}

QVariantMap WGItemView::mimeData(const QModelIndexList& indexes)
{
	QVariantMap data;
	auto mimeData = impl_->extendedModel_->mimeData(indexes);
	if (mimeData != nullptr)
	{
		for (auto& format : mimeData->formats())
		{
			data.insert(format, mimeData->data(format));
		}
		delete mimeData;
	}
	return data;
}

bool WGItemView::canDropMimeData(const QVariantMap& data, Qt::DropAction action, const QModelIndex& index) const
{
	QMimeData mimeData;
	for (auto it = data.constBegin(); it != data.constEnd(); ++it)
	{
		mimeData.setData(it.key(), it.value().toByteArray());
	}

	return impl_->extendedModel_->canDropMimeData(&mimeData, action, index.row(), index.column(), index.parent());
}

bool WGItemView::dropMimeData(const QVariantMap& data, Qt::DropAction action, const QModelIndex& index)
{
	QMimeData mimeData;
	for (auto it = data.constBegin(); it != data.constEnd(); ++it)
	{
		mimeData.setData(it.key(), it.value().toByteArray());
	}

	return impl_->extendedModel_->dropMimeData(&mimeData, action, index.row(), index.column(), index.parent());
}

QStringList WGItemView::headerMimeTypes(Qt::Orientation orientation) const
{
	QStringList mimeTypes;
	if (orientation == Qt::Horizontal)
	{
		mimeTypes.append(WGItemViewDetails::horizontalHeaderMimeKey);
	}
	else if (orientation == Qt::Vertical)
	{
		mimeTypes.append(WGItemViewDetails::verticalHeaderMimeKey);
	}
	return mimeTypes;
}

QVariantMap WGItemView::headerMimeData(const QList<int>& sections, Qt::Orientation orientation) const
{
	QByteArray buffer;
	QDataStream stream(&buffer, QIODevice::WriteOnly);
	stream << sections;
	std::string encodedSections = Base64::encode(buffer.constData(), buffer.length());

	QVariantMap data;
	data.insert(orientation == Qt::Horizontal ? WGItemViewDetails::horizontalHeaderMimeKey :
	                                            WGItemViewDetails::verticalHeaderMimeKey,
	            encodedSections.c_str());
	return data;
}

bool WGItemView::canDropHeaderMimeData(const QVariantMap& data, Qt::DropAction action, int section,
                                       Qt::Orientation orientation) const
{
	return data.find(orientation == Qt::Horizontal ? WGItemViewDetails::horizontalHeaderMimeKey :
	                                                 WGItemViewDetails::verticalHeaderMimeKey) != data.end() &&
	action == Qt::MoveAction;
}

bool WGItemView::dropHeaderMimeData(const QVariantMap& data, Qt::DropAction action, int section,
                                    Qt::Orientation orientation)
{
	if (!canDropHeaderMimeData(data, action, section, orientation))
	{
		return false;
	}

	QList<int> sections;
	std::string decoded;
	auto it = data.find(orientation == Qt::Horizontal ? WGItemViewDetails::horizontalHeaderMimeKey :
	                                                    WGItemViewDetails::verticalHeaderMimeKey);
	Base64::decode(it.value().toByteArray().data(), decoded);
	QByteArray buffer(decoded.c_str(), static_cast<int>(decoded.length()));
	QDataStream stream(buffer);
	stream >> sections;
	if (sections.empty() || sections.contains(section))
	{
		return false;
	}

	std::sort(sections.begin(), sections.end());
	auto last = std::unique(sections.begin(), sections.end());
	sections.erase(last, sections.end());

	impl_->extendedModel_->layoutAboutToBeChanged();
	int firstIndex = 0;
	int sourceOffset = 0;
	int destinationOffset = sections[0] > section ? 0 : 1;
	for (int index = 0; index < sections.count(); ++index)
	{
		int count = index - firstIndex;
		if (sections[index] - sections[firstIndex] == count)
		{
			continue;
		}
		int source = sections[firstIndex] + sourceOffset;
		int destination = section + destinationOffset;

		orientation == Qt::Horizontal ?
		impl_->extendedModel_->moveColumns(QModelIndex(), source, count, QModelIndex(), destination) :
		impl_->extendedModel_->moveRows(QModelIndex(), source, count, QModelIndex(), destination);
		if (source > destination)
		{
			destinationOffset += count;
		}
		else
		{
			sourceOffset += count;
		}
		firstIndex = index;
	}

	{
		int count = sections.count() - firstIndex;
		int source = sections[firstIndex] + sourceOffset;
		int destination = section + destinationOffset;

		orientation == Qt::Horizontal ?
		impl_->extendedModel_->moveColumns(QModelIndex(), source, count, QModelIndex(), destination) :
		impl_->extendedModel_->moveRows(QModelIndex(), source, count, QModelIndex(), destination);
	}
	impl_->extendedModel_->layoutChanged();

	return true;
}
} // end namespace wgt
