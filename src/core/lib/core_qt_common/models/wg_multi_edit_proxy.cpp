#include "wg_multi_edit_proxy.hpp"

#include "models/role_provider.hpp"
#include "core_common/assert.hpp"

namespace wgt
{
ITEMROLE(multipleValues)

WGMultiEditProxy::WGMultiEditProxy()
	: multipleValuesRole_(RoleProvider::convertRole(ItemRole::multipleValuesName))
{
	roleNames_ = QAbstractItemModel::roleNames();
}

WGMultiEditProxy::~WGMultiEditProxy()
{
}

void WGMultiEditProxy::addModel(QAbstractItemModel* model)
{
	beginResetModel();

	// clear mappings
	clearMapping();

	// add new model
	sourceModels_.push_back(model);

	// map root
	auto mapping = new Mapping;
	mappings_[QModelIndex()] = mapping;
	for (auto sourceModel : sourceModels_)
	{
		mapping->entries_[sourceModel] = Entry();
		create_role_mapping(sourceModel);
		connections_ +=
		QObject::connect(sourceModel, &QAbstractItemModel::dataChanged, this, &WGMultiEditProxy::onSourceDataChanged);
	}

	endResetModel();
}

void WGMultiEditProxy::removeModel(QAbstractItemModel* model)
{
	beginResetModel();

	// clear mappings
	clearMapping();

	// remove model
	sourceModels_.erase(std::remove(sourceModels_.begin(), sourceModels_.end(), model), sourceModels_.end());

	// map root
	auto mapping = new Mapping;
	mappings_[QModelIndex()] = mapping;
	for (auto sourceModel : sourceModels_)
	{
		mapping->entries_[sourceModel] = Entry();
		create_role_mapping(sourceModel);
		connections_ +=
		QObject::connect(sourceModel, &QAbstractItemModel::dataChanged, this, &WGMultiEditProxy::onSourceDataChanged);
	}

	endResetModel();
}

QList<QString> WGMultiEditProxy::getMergeKeys() const
{
	return mergeKeys_;
}

void WGMultiEditProxy::setMergeKeys(const QList<QString>& mergeKeys)
{
	beginResetModel();
	mergeKeys_ = mergeKeys;
	mergeKeyRoleIds_.clear();
	for (auto it = mergeKeys_.begin(); it != mergeKeys_.end(); ++it)
	{
		const int role = RoleProvider::convertRole(it->toUtf8().constData());
		mergeKeyRoleIds_.push_back(role);
	}
	endResetModel();
}

void WGMultiEditProxy::onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                           const QVector<int>& roles)
{
	QModelIndex proxy_index;
	auto model = const_cast<QAbstractItemModel*>(topLeft.model());
	TF_ASSERT(model != nullptr);
	auto& mappedRole = roleMappings_[model];
	auto proxy_roles = QVector<int>();
	for (auto role : roles)
	{
		if (std::find(mergeKeyRoleIds_.begin(), mergeKeyRoleIds_.end(), role) != mergeKeyRoleIds_.end())
		{
			TF_ASSERT("not support for merge key role changes." && false);
			return;
		}
		for (auto roleIt = mappedRole.begin(); roleIt != mappedRole.end(); ++roleIt)
		{
			if (role == roleIt->second)
			{
				proxy_roles.append(roleIt->first);
				break;
			}
		}
	}

	for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
	{
		auto findIt = (*it)->entries_.find(model);
		if (findIt == (*it)->entries_.end())
		{
			continue;
		}
		if (topLeft != findIt->second.sourceParent_)
		{
			continue;
		}
		proxy_index = it.key();
		emit dataChanged(proxy_index, proxy_index, proxy_roles);
		break;
	}
}

void WGMultiEditProxy::create_role_mapping(QAbstractItemModel* sourceModel)
{
	auto roleNames = sourceModel->roleNames();
	auto& roleMapping = roleMappings_[sourceModel];
	for (auto newIt = roleNames.begin(); newIt != roleNames.end(); ++newIt)
	{
		auto mappedRole = -1;
		auto newRoleName = newIt.value();
		for (auto existingIt = roleNames_.begin(); existingIt != roleNames_.end(); ++existingIt)
		{
			auto existingRoleName = existingIt.value();
			if (existingRoleName == newRoleName)
			{
				mappedRole = existingIt.key();
			}
		}
		if (mappedRole == -1)
		{
			while (true)
			{
				++mappedRole;
				if (roleNames_.find(mappedRole) == roleNames_.end())
				{
					break;
				}
			}
			roleNames_.insert(mappedRole, newRoleName);
		}

		roleMapping.emplace(mappedRole, newIt.key());
	}
}

QHash<QModelIndex, WGMultiEditProxy::Mapping*>::iterator WGMultiEditProxy::create_index_mapping(
const QModelIndex& proxy_parent) const
{
	auto it = mappings_.find(proxy_parent);
	TF_ASSERT(it != mappings_.end());
	if ((*it)->mapped_)
	{
		return it;
	}

	std::vector<std::map<int, QVariant>> items;
	for (auto& entry : (*it)->entries_)
	{
		auto& source_model = entry.first;
		auto& source_parent = entry.second.sourceParent_;
		int rowCount = source_model->rowCount(source_parent);
		for (int i = 0; i < rowCount; ++i)
		{
			auto pair = std::map<int, QVariant>();
			for (auto roleId : mergeKeyRoleIds_)
			{
				auto entry = source_model->data(source_model->index(i, 0, source_parent), roleId);
				pair[roleId] = entry;
			}
			bool merged = false;
			int j = 0;
			for (; j < static_cast<int>(items.size()); ++j)
			{
				auto& item = items[j];
				// doing merge
				merged = true;
				for (auto roleId : mergeKeyRoleIds_)
				{
					auto left = item[roleId];
					auto right = pair[roleId];
					if (left != right)
					{
						merged = false;
						break;
					}
				}
				if (merged)
				{
					break;
				}
			}
			if (!merged)
			{
				entry.second.rows_[static_cast<int>(items.size())] = i;
				items.push_back(pair);
			}
			else
			{
				entry.second.rows_[j] = i;
			}
		}
	}
	(*it)->rowCount_ = static_cast<int>(items.size());
	(*it)->mapIter_ = it;
	(*it)->mapped_ = true;
	return it;
}

QHash<QModelIndex, WGMultiEditProxy::Mapping*>::const_iterator WGMultiEditProxy::index_to_iterator(
const QModelIndex& proxy_index) const
{
	auto* p = proxy_index.internalPointer();
	TF_ASSERT(p);
	auto it = static_cast<const WGMultiEditProxy::Mapping*>(p)->mapIter_;
	TF_ASSERT(it != mappings_.end());
	TF_ASSERT(it.value());
	return it;
}

void WGMultiEditProxy::clearMapping()
{
	qDeleteAll(mappings_);
	mappings_.clear();
	connections_.reset();
	roleMappings_.clear();
	roleNames_.clear();
	roleNames_ = QAbstractItemModel::roleNames();
}

QModelIndex WGMultiEditProxy::index(int row, int column, const QModelIndex& parent) const
{
	auto it = create_index_mapping(parent);
	TF_ASSERT(it != mappings_.end());
	auto current_index = createIndex(row, column, *it);

	// create mapping for current index
	auto findIt = mappings_.find(current_index);
	if (findIt == mappings_.end())
	{
		auto mapping = new Mapping;
		mappings_[current_index] = mapping;

		for (auto& entry : (*it)->entries_)
		{
			auto& source_model = entry.first;
			auto& source_parent = entry.second.sourceParent_;
			auto findSourceRow = entry.second.rows_.find(row);
			if (findSourceRow != entry.second.rows_.end())
			{
				auto source_row = findSourceRow->second;
				auto source_index = source_model->index(source_row, column, source_parent);
				mapping->entries_[source_model].sourceParent_ = source_index;
			}
		}
	}

	return current_index;
}

QModelIndex WGMultiEditProxy::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}
	return index_to_iterator(child).key();
}

int WGMultiEditProxy::rowCount(const QModelIndex& parent) const
{
	if (sourceModels_.empty())
	{
		return 0;
	}
	auto it = create_index_mapping(parent);
	TF_ASSERT(it != mappings_.end());
	return (*it)->rowCount_;
}

int WGMultiEditProxy::columnCount(const QModelIndex& parent) const
{
	auto count = 0;
	if (sourceModels_.empty())
	{
		return count;
	}
	return 1;
}

QVariant WGMultiEditProxy::data(const QModelIndex& index, int role) const
{
	if (sourceModels_.empty())
	{
		return QVariant();
	}
	auto row = index.row();
	auto column = index.column();
	auto parent = this->parent(index);
	auto it = create_index_mapping(parent);
	QVariant retValue;
	int i = 0;
	bool multipleValues = false;
	for (auto& entry : (*it)->entries_)
	{
		auto& source_model = entry.first;
		auto& source_parent = entry.second.sourceParent_;
		QVariant sourceData;
		auto findIt = entry.second.rows_.find(row);
		auto roleMapping = roleMappings_.find(source_model);
		TF_ASSERT(roleMapping != roleMappings_.end());
		auto roleIt = roleMapping->second.find(role);
		if (findIt == entry.second.rows_.end() || roleIt == roleMapping->second.end())
		{
			continue;
		}
		auto sourceRole = roleIt->second;
		if (sourceRole == multipleValuesRole_)
		{
			retValue = false;
			// for merged rows, set multipleValue to true
			if (i > 0)
			{
				retValue = true;
				break;
			}
		}
		else
		{
			auto source_row = findIt->second;
			auto source_index = source_model->index(source_row, column, source_parent);
			sourceData = source_model->data(source_index, sourceRole);

			if (i == 0)
			{
				retValue = sourceData;
			}
			else
			{
				if (retValue != sourceData)
				{
					retValue = QVariant();
					break;
				}
			}
		}
		i++;
	}

	return retValue;
}

bool WGMultiEditProxy::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (sourceModels_.empty())
	{
		return false;
	}
	if (!index.isValid())
	{
		return false;
	}
	auto parent = this->parent(index);
	auto findIt = mappings_.find(parent);
	TF_ASSERT(findIt != mappings_.end());
	auto row = index.row();
	auto column = index.column();
	bool ret = false;
	for (auto& entry : (*findIt)->entries_)
	{
		auto& source_model = entry.first;
		auto& source_parent = entry.second.sourceParent_;
		auto findSourceRow = entry.second.rows_.find(row);
		auto roleIt = roleMappings_[source_model].find(role);
		if (findSourceRow == entry.second.rows_.end() || roleIt == roleMappings_[source_model].end())
		{
			continue;
		}
		auto source_row = findSourceRow->second;
		auto source_index = source_model->index(source_row, column, source_parent);
		ret = source_model->setData(source_index, value, roleIt->second) || ret;
	}

	return ret;
}

QHash<int, QByteArray> WGMultiEditProxy::roleNames() const
{
	return roleNames_;
}
}