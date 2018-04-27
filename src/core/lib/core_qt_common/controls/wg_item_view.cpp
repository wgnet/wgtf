#include "wg_item_view.hpp"

#include "helpers/qt_helpers.hpp"
#include "models/extensions/custom_model_extension.hpp"
#include "models/extensions/model_extension_manager.hpp"
#include "models/extensions/qt_model_extension.hpp"
#include "models/qt_abstract_item_model.hpp"
#include "models/role_provider.hpp"
#include "qt_connection_holder.hpp"
#include "qt_scripting_engine_base.hpp"

#include <core_common/assert.hpp>
#include <core_copy_paste/i_copy_paste_manager.hpp>
#include <core_data_model/common_data_roles.hpp>
#include <core_dependency_system/depends.hpp>
#include <wg_types/base64.hpp>

#include <functional>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QDataStream>
#include <QQmlListProperty>
#include <QString>
#include <QUuid>
#include <QtAlgorithms>
#include <QMimeData>
#include <QQmlEngine>
#include <QQmlContext>
#include <QItemSelection>
#include <private/qmetaobjectbuilder_p.h>

namespace wgt
{
ITEMROLE(modelIndex)
ITEMROLE(indexPath)

namespace WGItemViewDetails
{
const char* itemMimeKey = "application/model-item";
const char* textMimeKey = "text/plain";
const char* horizontalHeaderMimeKey = "application/model-horizontalHeader";
const char* verticalHeaderMimeKey = "application/model-verticalHeader";
}

namespace
{
class HeaderRowModel : public QtAbstractItemModel
{
public:
	HeaderRowModel() : model_(nullptr)
	{
	}

	void reset(QAbstractItemModel* model)
	{
		beginResetModel();
		model_ = model;
		connections_.reset();
		roles_.clear();

		if (model_ != nullptr)
		{
			const auto& roles = roleNames().keys();

			for (auto& role : roles)
			{
				roles_.push_back(role);
			}

			connections_ +=
			QObject::connect(model, &QAbstractItemModel::headerDataChanged, this, &HeaderRowModel::onHeaderDataChanged);
			connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeInserted, this,
			                                 &HeaderRowModel::onColumnsAboutToBeInserted);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::columnsInserted, this, &HeaderRowModel::onColumnsInserted);
			connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeRemoved, this,
			                                 &HeaderRowModel::onColumnsAboutToBeRemoved);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::columnsRemoved, this, &HeaderRowModel::onColumnsRemoved);
			connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeMoved, this,
			                                 &HeaderRowModel::onColumnsAboutToBeMoved);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::columnsMoved, this, &HeaderRowModel::onColumnsMoved);
		}

		endResetModel();
	}

private:
	QModelIndex index(int row, int column, const QModelIndex& parent) const override
	{
		if (model_ == nullptr || parent.isValid() || column != 0 || row < 0)
		{
			return QModelIndex();
		}

		return createIndex(row, column);
	}

	QModelIndex parent(const QModelIndex& child) const override
	{
		return QModelIndex();
	}

	int rowCount(const QModelIndex& parent) const override
	{
		if (model_ == nullptr || parent.isValid())
		{
			return 0;
		}

		return model_->columnCount(parent);
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return 1;
	}

	bool hasChildren(const QModelIndex& parent) const override
	{
		if (parent.isValid())
		{
			return false;
		}

		return rowCount(parent) != 0;
	}

	QVariant data(const QModelIndex& index, int role) const override
	{
		if (model_ == nullptr || index.column() != 0 || index.parent().isValid() || index.row() < 0)
		{
			return QVariant();
		}

		return model_->headerData(index.row(), Qt::Orientation::Horizontal, role);
	}

	bool setData(const QModelIndex& index, const QVariant& value, int role) override
	{
		if (model_ == nullptr || index.column() != 0 || index.parent().isValid() || index.row() < 0)
		{
			return false;
		}

		return model_->setHeaderData(index.row(), Qt::Orientation::Horizontal, value, role);
	}

	QHash<int, QByteArray> roleNames() const override
	{
		return model_->roleNames();
	}

	void onHeaderDataChanged(Qt::Orientation orientation, int first, int last)
	{
		if (model_ == nullptr || orientation != Qt::Horizontal)
		{
			return;
		}

		dataChanged(index(first, 0, QModelIndex()), index(last, 0, QModelIndex()), roles_);
	}

	void onColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
		beginInsertRows(QModelIndex(), first, last);
	}

	void onColumnsInserted(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
		endInsertRows();
	}

	void onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
		beginRemoveRows(QModelIndex(), first, last);
	}

	void onColumnsRemoved(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
		endRemoveRows();
	}

	void onColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                             const QModelIndex& destinationParent, int destinationColumn)
	{
		TF_ASSERT(!sourceParent.isValid() && !destinationParent.isValid());
		beginMoveRows(QModelIndex(), sourceFirst, sourceLast, QModelIndex(), destinationColumn);
	}

	void onColumnsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                    const QModelIndex& destinationParent, int destinationColumn)
	{
		TF_ASSERT(!sourceParent.isValid() && !destinationParent.isValid());
		endMoveRows();
	}

	QAbstractItemModel* model_;
	QtConnectionHolder connections_;
	QVector<int> roles_;
};

class ModelExtensionData : public IQtModelExtensionData
{
	typedef QMap<ItemRole::Id, QVariant> ItemData;

public:
	ModelExtensionData(QAbstractItemModel* extendedModel) 
		: extendedModel_(extendedModel)
		, internalModel_(nullptr)
		, suppressNotifications_( false )
	{
	}

	void save()
	{
		savedData_[internalModel_] = qMakePair(itemData_, qMakePair(indexCache_, indexPathCache_));
		savedHeaderData_[internalModel_] = headerData_;
		// TODO save to preference
	}

	void load()
	{
		loadItemData();
		loadHeaderData();
		// TODO load from preference
	}

	void loadItemData()
	{
		auto findIt = savedData_.find(internalModel_);

		if (findIt == savedData_.end())
		{
			itemData_.clear();
			indexCache_.clear();
			indexPathCache_.clear();
			return;
		}

		itemData_ = findIt.value().first;
		indexCache_ = findIt.value().second.first;
		indexPathCache_ = findIt.value().second.second;
	}

	void loadHeaderData()
	{
		auto headerItr = savedHeaderData_.find(internalModel_);

		if (headerItr == savedHeaderData_.end())
		{
			headerData_[0].clear();
			headerData_[1].clear();
			return;
		}

		headerData_ = headerItr.value();
	}

	void findExtensionRoles()
	{
		extensionRoleIds_.clear();
		if (internalModel_ == nullptr)
			return;
		auto internalRoles = internalModel_->roleNames().keys();
		auto allRoles = extendedModel_->roleNames();
		ItemRole::Id roleId;

		for (int role : allRoles.keys())
		{
			if (internalRoles.contains(role))
			{
				continue;
			}

			auto roleDecoder = dynamic_cast<RoleProvider*>(extendedModel_);
			TF_ASSERT(roleDecoder != nullptr);
			bool decoded = roleDecoder->decodeRole(role, roleId);
			TF_ASSERT(decoded);

			extensionRoleIds_.append(roleId);
		}
	}

	void reset(QAbstractItemModel* internalModel)
	{
		save();
		internalModel_ = internalModel;
		load();
		findExtensionRoles();
	}

	bool encodeRole(ItemRole::Id roleId, int& o_Role) const override
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

		if (extensionRoleIds_.contains(roleId))
		{
			auto roleProvider = dynamic_cast<RoleProvider*>(extendedModel_);
			TF_ASSERT(roleProvider != nullptr);
			return roleProvider->encodeRole(roleId, o_Role);
		}

		auto model = internalModel_;
		while (model != nullptr)
		{
			auto roleProvider = dynamic_cast<RoleProvider*>(model);

			if (roleProvider != nullptr && roleProvider->encodeRole(roleId, o_Role))
			{
				return true;
			}

			auto proxyModel = dynamic_cast<QAbstractProxyModel*>(model);
			model = proxyModel == nullptr ? nullptr : proxyModel->sourceModel();
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

		auto roleProvider = dynamic_cast<RoleProvider*>(extendedModel_);
		TF_ASSERT(roleProvider != nullptr);
		if (roleProvider->decodeRole(role, o_RoleId))
		{
			return true;
		}

		auto model = internalModel_;
		while (model != nullptr)
		{
			auto roleProvider = dynamic_cast<RoleProvider*>(model);

			if (roleProvider != nullptr && roleProvider->decodeRole(role, o_RoleId))
			{
				return true;
			}

			auto proxyModel = dynamic_cast<QAbstractProxyModel*>(model);
			model = proxyModel == nullptr ? nullptr : proxyModel->sourceModel();
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

	QVariant data(const QModelIndex& index, ItemRole::Id roleId) override
	{
		if (extensionRoleIds_.contains(roleId))
		{
			auto& itemData = getItemData(index);
			return itemData[roleId];
		}

		int role;

		if (!encodeRole(roleId, role))
		{
			return QVariant::Invalid;
		}

		return extendedModel_->data(index, role);
	}

	void suppressNotifications(bool enable) override
	{
		suppressNotifications_ = enable;
	}

	bool setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId) override
	{
		if (extensionRoleIds_.contains(roleId))
		{
			auto& itemData = getItemData(index);

			if (itemData[roleId] == value)
			{
				return false;
			}

			itemData[roleId] = value;
			int role;

			if (encodeRole(roleId, role))
			{
				QVector<int> roles;
				roles.append(role);
				if (suppressNotifications_)
				{
					return true;
				}
				emit extendedModel_->dataChanged(index, index, roles);
			}

			return true;
		}

		int role;

		if (!encodeRole(roleId, role))
		{
			return false;
		}

		return extendedModel_->setData(index, value, role);
	}

	void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
	                 const QVector<ItemRole::Id> roleIds) override
	{
		QVector<int> roles;

		for (auto roleId : roleIds)
		{
			int role;
			bool encoded = encodeRole(roleId, role);
			TF_ASSERT(encoded);
			roles.append(role);
		}

		emit extendedModel_->dataChanged(topLeft, bottomRight, roles);
	}

	QVariant headerData(int section, Qt::Orientation orientation, ItemRole::Id roleId) override
	{
		if (extensionRoleIds_.contains(roleId))
		{
			auto& headerItemData = getHeaderData(section, orientation);
			return headerItemData[roleId];
		}

		int role;

		if (!encodeRole(roleId, role))
		{
			return QVariant::Invalid;
		}

		return extendedModel_->headerData(section, orientation, role);
	}

	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, ItemRole::Id roleId) override
	{
		if (extensionRoleIds_.contains(roleId))
		{
			auto& headerItemData = getHeaderData(section, orientation);

			if (headerItemData[roleId] == value)
			{
				return false;
			}

			headerItemData[roleId] = value;
			headerDataChanged(orientation, section, section);
			return true;
		}

		int role;

		if (!encodeRole(roleId, role))
		{
			return false;
		}

		return extendedModel_->setHeaderData(section, orientation, role);
	}

	void headerDataChanged(Qt::Orientation orientation, int first, int last) override
	{
		emit extendedModel_->headerDataChanged(orientation, first, last);
	}

private:
	ItemData& getItemData(const QModelIndex& index)
	{
		auto indexPath = data(index, ItemRole::indexPathId);
		if (indexPath.isValid())
		{
			auto it = indexPathCache_.find(indexPath);
			if (it != indexPathCache_.end())
			{
				return itemData_[*it];
			}
			indexPathCache_[indexPath] = itemData_.count();
		}
		else
		{
			auto it = indexCache_.find(index);
			if (it != indexCache_.end())
			{
				return itemData_[*it];
			}
			indexCache_[index] = itemData_.count();
		}

		itemData_.push_back(ItemData());
		return itemData_.back();
	}

	ItemData& getHeaderData(const int section, const Qt::Orientation orientation)
	{
		int sectionNumber = orientation == Qt::Orientation::Horizontal ? 0 : 1;
		auto& headerSection = headerData_[sectionNumber];

		while (headerSection.count() < section)
		{
			headerSection.append(ItemData());
		}

		return headerSection[section];
	}

private:
	QAbstractItemModel* extendedModel_;
	QAbstractItemModel* internalModel_;

	QVector<ItemData> itemData_;
	QMap<QPersistentModelIndex, int> indexCache_;
	QMap<QVariant, int> indexPathCache_;
	QMap<QAbstractItemModel*, QPair<QVector<ItemData>, QPair<QMap<QPersistentModelIndex, int>, QMap<QVariant, int>>>> savedData_;

	std::array<QVector<ItemData>, 2> headerData_;
	QMap<QAbstractItemModel*, std::array<QVector<ItemData>, 2>> savedHeaderData_;

	QVector<ItemRole::Id> extensionRoleIds_;
	bool suppressNotifications_;
};

class ExtendedModel : public QtAbstractItemModel, public RoleProvider
{
	DECLARE_QT_MEMORY_HANDLER
public:
	ExtendedModel(QList<QtModelExtension*>& extensions) : model_(nullptr), extensions_(extensions), extensionData_(this)
	{
	}

	void reset(QAbstractItemModel* model)
	{
		beginResetModel();
		model_ = model;
		connections_.reset();
		roleNames_.clear();
		columnMappings_.clear();

		if (model_ != nullptr)
		{
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::dataChanged, this, &ExtendedModel::onDataChanged);
			connections_ +=
			QObject::connect(model, &QAbstractItemModel::headerDataChanged, this, &ExtendedModel::onHeaderDataChanged);
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
				QObject::connect(this, &QAbstractItemModel::dataChanged, extension, &QtModelExtension::onDataChanged);
				connections_ += QObject::connect(this, &QAbstractItemModel::headerDataChanged, extension,
				                                 &QtModelExtension::onHeaderDataChanged);
				connections_ += QObject::connect(this, &QAbstractItemModel::layoutAboutToBeChanged, extension,
				                                 &QtModelExtension::onLayoutAboutToBeChanged);
				connections_ += QObject::connect(this, &QAbstractItemModel::layoutChanged, extension,
				                                 &QtModelExtension::onLayoutChanged);
				connections_ += QObject::connect(this, &QAbstractItemModel::rowsAboutToBeInserted, extension,
				                                 &QtModelExtension::onRowsAboutToBeInserted);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::rowsInserted, extension, &QtModelExtension::onRowsInserted);
				connections_ += QObject::connect(this, &QAbstractItemModel::rowsAboutToBeRemoved, extension,
				                                 &QtModelExtension::onRowsAboutToBeRemoved);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::rowsRemoved, extension, &QtModelExtension::onRowsRemoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::rowsAboutToBeMoved, extension,
				                                 &QtModelExtension::onRowsAboutToBeMoved);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::rowsMoved, extension, &QtModelExtension::onRowsMoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsAboutToBeInserted, extension,
				                                 &QtModelExtension::onColumnsAboutToBeInserted);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsInserted, extension,
				                                 &QtModelExtension::onColumnsInserted);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsAboutToBeRemoved, extension,
				                                 &QtModelExtension::onColumnsAboutToBeRemoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsRemoved, extension,
				                                 &QtModelExtension::onColumnsRemoved);
				connections_ += QObject::connect(this, &QAbstractItemModel::columnsAboutToBeMoved, extension,
				                                 &QtModelExtension::onColumnsAboutToBeMoved);
				connections_ +=
				QObject::connect(this, &QAbstractItemModel::columnsMoved, extension, &QtModelExtension::onColumnsMoved);

				connections_ += QObject::connect(this, &QAbstractItemModel::modelReset, extension,
					&QtModelExtension::onModelReset);
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
				extension->setExtensionData(extensionData_);
			}

			auto columnCount = model_->columnCount();
			for (auto i = 0; i < columnCount; ++i)
			{
				columnMappings_.append(QPair<int, bool>(i, true));
			}
		}

		extensionData_.reset(model);
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

		auto destParent = sourceIndex(parent);
		if (model_->canDropMimeData(data, action, row, sourceColumn(column), destParent))
		{
			return model_->dropMimeData(data, action, row, sourceColumn(column), destParent);
		}

		bool canDrop = (data->hasFormat(WGItemViewDetails::itemMimeKey) && action == Qt::MoveAction);
		if (!canDrop)
		{
			return false;
		}

		// Move the mime data from one spot in the model to another

		QList<QPersistentModelIndex> persistentIndexes;
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
			persistentIndexes.append(sourceIndex(createIndex(row, column, internalId)));
		}

		if (persistentIndexes.empty())
		{
			return true;
		}

		std::sort(persistentIndexes.begin(), persistentIndexes.end());
		bool moveAfter = row > persistentIndexes[0].row();
		QPersistentModelIndex persistentDestination = index(row, column, parent);

		for (QModelIndex index : persistentIndexes)
		{
			int destRow = moveAfter ? persistentDestination.row() + 1 : persistentDestination.row();
			model_->moveRow(index.parent(), index.row(), destParent, destRow);
		}
		return true;
	}

	bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent,
	              int destinationChild) override
	{
		TF_ASSERT(false && "Not Implemented");
		return false;
	}

	bool moveColumns(const QModelIndex& sourceParent, int sourceColumn, int count, const QModelIndex& destinationParent,
	                 int destinationChild) override
	{
		TF_ASSERT(!sourceParent.isValid() && !destinationParent.isValid());
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
	}

	QModelIndex sourceIndex(const QModelIndex& extendedIndex, int row = -1, int column = -1) const
	{
		if (!extendedIndex.isValid())
		{
			return QModelIndex();
		}

		TF_ASSERT(extendedIndex.model() == this);
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

		TF_ASSERT(modelIndex.model() == model_);
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

	void onHeaderDataChanged(Qt::Orientation orientation, int first, int last)
	{
		headerDataChanged(orientation, first, last);
	}

	void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                              QAbstractItemModel::LayoutChangeHint hint)
	{
		TF_ASSERT(redundantIndices_.empty());
		auto persistentIndices = persistentIndexList();
		QModelIndexList redundantParents;
		for (auto& parent : parents)
		{
			if (parent.isValid())
			{
				redundantParents.append(extendedIndex(parent));
			}
		}
		while (!redundantParents.empty())
		{
			QModelIndexList redundantIndices;
			for (auto& persitentIndex : persistentIndices)
			{
				auto persistentParent = persitentIndex.parent();
				if (redundantParents.indexOf(persistentParent) != -1)
				{
					redundantIndices.append(persitentIndex);
				}
			}
			redundantParents = redundantIndices;
			redundantIndices_.append(redundantIndices);
		}

		QList<QPersistentModelIndex> extendedParents;
		for (auto& parent : parents)
		{
			extendedParents.append(extendedIndex(parent));
		}
		layoutAboutToBeChanged(extendedParents, hint);
	}

	void onLayoutChanged(const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint)
	{
		for (auto& redundantIndex : redundantIndices_)
		{
			changePersistentIndex(redundantIndex, QModelIndex());
		}
		redundantIndices_.clear();

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
	}

	void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
		beginRemoveRows(extendedIndex(parent), first, last);
	}

	void onRowsRemoved(const QModelIndex& parent, int first, int last)
	{
		endRemoveRows();
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
	}

	void onColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
	}

	void onColumnsInserted(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
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
	}

	void onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
		// operate on a copy
		auto tmp = columnMappings_;

		int count = last - first + 1;
		for (auto& columnMapping : tmp)
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
		for (int i = 0; i < tmp.size(); ++i)
		{
			if (tmp[i].first == -1)
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
				tmp.erase(tmp.begin() + first, tmp.begin() + last + 1);
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
			tmp.erase(tmp.begin() + first, tmp.end());
			endRemoveColumns();
		}

		columnMappings_ = tmp;
	}

	void onColumnsRemoved(const QModelIndex& parent, int first, int last)
	{
		TF_ASSERT(!parent.isValid());
	}

	void onColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                             const QModelIndex& destinationParent, int destinationColumn)
	{
		TF_ASSERT(!sourceParent.isValid() && !destinationParent.isValid());
		layoutAboutToBeChanged(QList<QPersistentModelIndex>(), QAbstractItemModel::HorizontalSortHint);
	}

	void onColumnsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                    const QModelIndex& destinationParent, int destinationColumn)
	{
		TF_ASSERT(!sourceParent.isValid() && !destinationParent.isValid());
		layoutChanged(QList<QPersistentModelIndex>(), QAbstractItemModel::HorizontalSortHint);
	}

	QAbstractItemModel* model_;
	QtConnectionHolder connections_;
	QList<QtModelExtension*>& extensions_;
	QHash<int, QByteArray> roleNames_;
	ModelExtensionData extensionData_;
	QList<QPair<int, bool>> columnMappings_;
	QModelIndexList redundantIndices_;
};
}

struct WGItemView::Impl
	: public Depends<ICopyPasteManager>
{
	Impl(WGItemView* view)
	    : view_(view), model_(nullptr), headerRowModel_(new HeaderRowModel()), modelExtensionManager_(nullptr)
	{
	}

	ModelExtensionManager* modelExtensionManager();

	WGItemView* view_;
	QAbstractItemModel* model_;
	QtConnectionHolder connections_;
	QList<QtModelExtension*> extensions_;
	QStringList extensionNames_;
	std::unique_ptr<ExtendedModel> extendedModel_;
	std::unique_ptr<HeaderRowModel> headerRowModel_;

private:
	ModelExtensionManager* modelExtensionManager_;
};

ModelExtensionManager* WGItemView::Impl::modelExtensionManager()
{
	if (modelExtensionManager_ == nullptr)
	{
		QQmlContext* context = QtQml::qmlContext(view_);
		QVariant modelExtensionManager = context->contextProperty("modelExtensionManager");
		TF_ASSERT(modelExtensionManager.canConvert<ModelExtensionManager*>());
		modelExtensionManager_ = modelExtensionManager.value<ModelExtensionManager*>();
	}

	return modelExtensionManager_;
}

WGItemView::WGItemView() : impl_(new Impl(this))
{
	impl_->extendedModel_.reset(new ExtendedModel(impl_->extensions_));
	impl_->headerRowModel_->reset(impl_->extendedModel_.get());

	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::modelReset, [&]() {
		rowCountChanged();
		columnCountChanged();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::rowsInserted, [&]() {
		rowCountChanged();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::rowsRemoved, [&]() {
		rowCountChanged();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::columnsInserted, [&]() {
		columnCountChanged();
	});
	QObject::connect(impl_->extendedModel_.get(), &QAbstractItemModel::columnsRemoved, [&]() {
		columnCountChanged();
	});
}

WGItemView::~WGItemView()
{
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
		auto resetFunction = [&]() {
			impl_->extendedModel_->reset(impl_->model_);
			impl_->headerRowModel_->reset(impl_->extendedModel_.get());
		};

		impl_->connections_ += QObject::connect(impl_->model_, &QAbstractItemModel::modelReset, resetFunction);
	}
}

QQmlListProperty<QtModelExtension> WGItemView::getExtensions() const
{
	return QQmlListProperty<QtModelExtension>(const_cast<WGItemView*>(this), nullptr, &appendExtension,
	                                          &countExtensions, &extensionAt, &clearExtensions);
}

void WGItemView::appendExtension(QQmlListProperty<QtModelExtension>* property, QtModelExtension* value)
{
	auto itemView = qobject_cast<WGItemView*>(property->object);
	if (itemView == nullptr)
	{
		return;
	}

	if (value == nullptr)
	{
		return;
	}

	itemView->impl_->extensions_.append(value);
	itemView->impl_->extendedModel_->reset(itemView->impl_->model_);
}

int WGItemView::countExtensions(QQmlListProperty<QtModelExtension>* property)
{
	auto itemView = qobject_cast<WGItemView*>(property->object);
	if (itemView == nullptr)
	{
		return 0;
	}

	return itemView->impl_->extensions_.count();
}

QtModelExtension* WGItemView::extensionAt(QQmlListProperty<QtModelExtension>* property, int index)
{
	auto itemView = qobject_cast<WGItemView*>(property->object);
	if (itemView == nullptr)
	{
		return nullptr;
	}

	return itemView->impl_->extensions_[index];
}

void WGItemView::clearExtensions(QQmlListProperty<QtModelExtension>* property)
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

QAbstractItemModel* WGItemView::getHeaderRowModel() const
{
	return impl_->headerRowModel_.get();
}

int WGItemView::getRowCount() const
{
	QtAbstractItemModel* extendedModel = impl_->extendedModel_.get();
	TF_ASSERT(extendedModel != nullptr);
	return extendedModel->rowCount(nullptr);
}

int WGItemView::getColumnCount() const
{
	QtAbstractItemModel* extendedModel = impl_->extendedModel_.get();
	TF_ASSERT(extendedModel != nullptr);
	return extendedModel->columnCount(nullptr);
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

QModelIndexList WGItemView::getExtendedIndexes(const QItemSelection& sourceSelection) const
{
	QModelIndexList sourceIndexes = sourceSelection.indexes();
	QItemSelection viewSelection;
	for (int i = 0; i < sourceIndexes.size(); ++i) {
		const QModelIndex srcIdx = impl_->extendedModel_->extendedIndex(sourceIndexes.at(i));
		if (!srcIdx.isValid())
			continue;
		viewSelection << QItemSelectionRange(srcIdx);
	}
	return viewSelection.indexes();
}


bool WGItemView::canCopy(const QList<QModelIndex>& indexes) const
{
	auto pCopyPasteManager = impl_->get<ICopyPasteManager>();
	if (pCopyPasteManager == nullptr)
	{
		return false;
	}

	auto& copyPasteManager = (*pCopyPasteManager);

	// Get data for each item
	const auto dataMap = this->mimeData(indexes);
	if (dataMap.empty())
	{
		return false;
	}

	// Check if text data is empty
	const auto it = dataMap.find(WGItemViewDetails::textMimeKey);
	if (it != dataMap.cend())
	{
		const auto qByteArray = it.value().toByteArray();
		return !qByteArray.isEmpty();
	}

	return false;
}

void WGItemView::copy(const QList<QModelIndex>& indexes) const
{
	auto pCopyPasteManager = impl_->get<ICopyPasteManager>();
	if (pCopyPasteManager == nullptr)
	{
		return;
	}
	auto& copyPasteManager = (*pCopyPasteManager);

	// Get data for each item
	const auto dataMap = this->mimeData(indexes);

	// Convert Qt data to clipboard format
	MimeData wgtMimeData;
	for (auto it = dataMap.constBegin(); it != dataMap.constEnd(); ++it)
	{
		const std::string key = it.key().toUtf8().data();

		const auto qByteArray = it.value().toByteArray();
		std::vector<char> value(qByteArray.begin(), qByteArray.end());

		wgtMimeData.emplace(key, value);
	}

	// Set data on clipboard
	copyPasteManager.setClipboardContents(wgtMimeData);
}

bool WGItemView::canPaste(const QList<QModelIndex>& indexes) const
{
	auto pCopyPasteManager = impl_->get<ICopyPasteManager>();
	if (pCopyPasteManager == nullptr)
	{
		return false;
	}
	auto& copyPasteManager = (*pCopyPasteManager);

	// Get clipboard data
	const auto wgtMimeData = copyPasteManager.getClipboardContents();

	// Convert clipboard data to Qt data
	QVariantMap dataMap;
	for (auto it = wgtMimeData.cbegin(); it != wgtMimeData.cend(); ++it)
	{
		const auto key = QString::fromUtf8(it->first.c_str(),
			static_cast<int>(it->first.size()));
		const QByteArray value(it->second.data(),
			static_cast<int>(it->second.size()));
		dataMap.insert(key, value);
	}

	// Drop data into each item
	for (auto index : indexes)
	{
		const auto canDrop = this->canDropMimeData(dataMap, Qt::CopyAction, index);
		if (canDrop)
		{
			return true;
		}
	}

	return false;
}

void WGItemView::paste(const QList<QModelIndex>& indexes)
{
	auto pCopyPasteManager = impl_->get<ICopyPasteManager>();
	if (pCopyPasteManager == nullptr)
	{
		return;
	}
	auto& copyPasteManager = (*pCopyPasteManager);

	// Get clipboard data
	const auto wgtMimeData = copyPasteManager.getClipboardContents();

	// Convert clipboard data to Qt data
	QVariantMap dataMap;
	for (auto it = wgtMimeData.cbegin(); it != wgtMimeData.cend(); ++it)
	{
		const auto key = QString::fromUtf8(it->first.c_str(),
			static_cast<int>(it->first.size()));
		const QByteArray value(it->second.data(),
			static_cast<int>(it->second.size()));
		dataMap.insert(key, value);
	}

	// Drop data into each item
	for (auto index : indexes)
	{
		this->dropMimeData(dataMap, Qt::CopyAction, index);
	}
}

QStringList WGItemView::mimeTypes() const
{
	return impl_->extendedModel_->mimeTypes();
}

QVariantMap WGItemView::mimeData(const QModelIndexList& indexes) const
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

wgt::QtModelExtension* WGItemView::createExtension(const QString& name)
{
	return impl_->modelExtensionManager()->createExtension(name, "2.0", this);
}

QModelIndex WGItemView::toModelIndex(const QPersistentModelIndex& persistentModelIndex) const
{
	return persistentModelIndex;
}

QPersistentModelIndex WGItemView::toPersistentModelIndex(const QModelIndex& modelIndex) const
{
	return QPersistentModelIndex(modelIndex);
}

QModelIndexList WGItemView::toModelIndexList(const QModelIndex& modelIndex) const
{
	QModelIndexList modelIndexList;
	modelIndexList.append(modelIndex);
	return modelIndexList;
}

QModelIndexList WGItemView::toModelIndexList(const QItemSelection& itemSelection) const
{
	return itemSelection.indexes();
}

QItemSelection WGItemView::toItemSelection(const QModelIndexList& modelIndexList) const
{
	QItemSelection itemSelection;
	for (auto& modelIndex : modelIndexList) 
	{
		itemSelection << QItemSelectionRange(modelIndex);
	}
	return itemSelection;
}

} // end namespace wgt
