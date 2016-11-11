#include "wg_merge_proxy.hpp"

namespace wgt
{
WGMergeProxy::WGMergeProxy() : orientation_(Qt::Horizontal)
{
	roleNames_ = QAbstractItemModel::roleNames();
	for (auto role : roleNames_.keys())
	{
		roles_[role] = 1;
	}
}

WGMergeProxy::~WGMergeProxy()
{
}

void WGMergeProxy::addModel(QAbstractItemModel* model, int index, const QString& roleName)
{
	if (model == nullptr)
	{
		return;
	}

	for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
	{
		if ((*it)->model_ == model)
		{
			return;
		}
	}

	auto count = orientation_ == Qt::Horizontal ? model->columnCount() : model->rowCount();
	if (index >= count)
	{
		return;
	}

	auto roleNames = model->roleNames();
	auto role = -1;
	if (!roleName.isEmpty())
	{
		for (auto it = roleNames.begin(); it != roleNames.end(); ++it)
		{
			if (it.value() == roleName)
			{
				role = it.key();
			}
		}
	}
	if (role == -1)
	{
		return;
	}

	auto mapping = new Mapping();
	mapping->model_ = model;

	auto rolesChanged = false;
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
			rolesChanged = true;
			beginResetModel();
			while (true)
			{
				++mappedRole;
				if (roleNames_.find(mappedRole) == roleNames_.end())
				{
					break;
				}
			}
			roles_.insert(mappedRole, 1);
			roleNames_.insert(mappedRole, newRoleName);
		}
		else
		{
			++roles_[mappedRole];
		}
		mapping->roles_.insert(mappedRole, newIt.key());
	}

	if (rolesChanged)
	{
		endResetModel();
	}

	count = orientation_ == Qt::Horizontal ? model->rowCount() : model->columnCount();
	for (auto i = 0; i < count; ++i)
	{
		auto entry = model->data(Qt::Horizontal ? model->index(i, index) : model->index(index, i), role);
		auto it = std::find_if(entries_.begin(), entries_.end(),
		                       [&entry](QPair<QVariant, int>& item) { return item.first == entry; });
		auto entryIndex = std::distance(entries_.begin(), it);
		mapping->entries_.insert(entryIndex, i);
		if (it == entries_.end())
		{
			entries_.append(QPair<QVariant, int>(entry, 0));
			it = entries_.end() - 1;
		}
		if (it->second == 0)
		{
			auto pos = 0;
			for (auto i = 0; i < entryIndex; ++i)
			{
				if (entries_[i].second > 0)
				{
					++pos;
				}
			}
			orientation_ == Qt::Horizontal ? beginInsertRows(QModelIndex(), pos, pos) :
			                                 beginInsertColumns(QModelIndex(), pos, pos);
			++it->second;
			orientation_ == Qt::Horizontal ? endInsertRows() : endInsertColumns();
		}
		else
		{
			++it->second;
		}
	}

	mapping->connections_ +=
	QObject::connect(model, &QAbstractItemModel::dataChanged, this, &WGMergeProxy::onDataChanged);

	auto pos = orientation_ == Qt::Horizontal ? columnCount() : rowCount();
	orientation_ == Qt::Horizontal ? beginInsertColumns(QModelIndex(), pos, pos + model->columnCount() - 1) :
	                                 beginInsertRows(QModelIndex(), pos, pos + model->rowCount() - 1);
	mappings_.emplace_back(mapping);
	orientation_ == Qt::Horizontal ? endInsertColumns() : endInsertRows();
}

void WGMergeProxy::removeModel(QAbstractItemModel* model)
{
	if (model == nullptr)
	{
		return;
	}

	auto pos = 0;
	auto it = mappings_.begin();
	for (; it != mappings_.end(); ++it)
	{
		if ((*it)->model_ == model)
		{
			break;
		}
		orientation_ == Qt::Horizontal ? pos += (*it)->model_->columnCount() : (*it)->model_->rowCount();
	}
	if (it == mappings_.end())
	{
		return;
	}

	orientation_ == Qt::Horizontal ? beginRemoveColumns(QModelIndex(), pos, pos + model->columnCount() - 1) :
	                                 beginRemoveRows(QModelIndex(), pos, pos + model->rowCount() - 1);
	auto mapping = (*it).release();
	mappings_.erase(it);
	orientation_ == Qt::Horizontal ? endRemoveColumns() : endRemoveRows();

	for (auto mappingEntryIt = mapping->entries_.begin(); mappingEntryIt != mapping->entries_.end(); ++mappingEntryIt)
	{
		auto entryIndex = mappingEntryIt.key();
		auto entryIt = entries_.begin() + entryIndex;
		if (entryIt->second == 1)
		{
			auto pos = 0;
			for (auto i = 0; i < entryIndex; ++i)
			{
				if (entries_[i].second > 0)
				{
					++pos;
				}
			}
			orientation_ == Qt::Horizontal ? beginRemoveRows(QModelIndex(), pos, pos) :
			                                 beginRemoveColumns(QModelIndex(), pos, pos);
			--entryIt->second;
			orientation_ == Qt::Horizontal ? endRemoveRows() : endRemoveColumns();
		}
		else
		{
			--entryIt->second;
		}
	}

	auto rolesChanged = false;
	for (auto mappingRolesIt = mapping->roles_.begin(); mappingRolesIt != mapping->roles_.end(); ++mappingRolesIt)
	{
		auto role = mappingRolesIt.key();
		if (--roles_[role] == 0)
		{
			rolesChanged = true;
			beginResetModel();
			roleNames_.remove(role);
		}
	}

	if (rolesChanged)
	{
		endResetModel();
	}

	delete mapping;
}

Qt::Orientation WGMergeProxy::getOrientation() const
{
	return orientation_;
}

void WGMergeProxy::setOrientation(Qt::Orientation orientation)
{
	beginResetModel();
	orientation_ = orientation;
	endResetModel();
}

void WGMergeProxy::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	auto model = topLeft.model();

	auto proxy_row = topLeft.row();
	auto proxy_column = topLeft.column();
	auto proxy_roles = QVector<int>();

	for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
	{
		auto mapping = it->get();
		if (mapping->model_ != model)
		{
			if (orientation_ == Qt::Horizontal)
			{
				proxy_column += mapping->model_->columnCount();
			}
			else
			{
				proxy_row += mapping->model_->rowCount();
			}
			continue;
		}

		for (auto entryIt = mapping->entries_.begin(); entryIt != mapping->entries_.end(); ++entryIt)
		{
			if (orientation_ == Qt::Horizontal)
			{
				if (proxy_row == entryIt.value())
				{
					proxy_row = entryIt.key();
					break;
				}
			}
			else
			{
				if (proxy_column == entryIt.value())
				{
					proxy_column = entryIt.key();
					break;
				}
			}
		}

		auto proxy_index = index(proxy_row, proxy_column);

		for (auto role : roles)
		{
			for (auto roleIt = mapping->roles_.begin(); roleIt != mapping->roles_.end(); ++roleIt)
			{
				if (role == roleIt.value())
				{
					proxy_roles.append(roleIt.key());
					break;
				}
			}
		}

		dataChanged(proxy_index, proxy_index, proxy_roles);
		break;
	}
}

QModelIndex WGMergeProxy::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return QModelIndex();
	}

	if (orientation_ == Qt::Horizontal)
	{
		if (row >= entries_.count())
		{
			return QModelIndex();
		}
	}
	else
	{
		if (column >= entries_.count())
		{
			return QModelIndex();
		}
	}

	return createIndex(row, column, nullptr);
}

QModelIndex WGMergeProxy::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int WGMergeProxy::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
	{
		return 0;
	}

	auto count = 0;
	if (orientation_ == Qt::Horizontal)
	{
		for (auto& entry : entries_)
		{
			if (entry.second > 0)
			{
				++count;
			}
		}
		return count;
	}

	for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
	{
		count += (*it)->model_->rowCount();
	}
	return count;
}

int WGMergeProxy::columnCount(const QModelIndex& parent) const
{
	auto count = 0;
	if (orientation_ != Qt::Horizontal)
	{
		for (auto& entry : entries_)
		{
			if (entry.second > 0)
			{
				++count;
			}
		}
		return count;
	}

	for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
	{
		count += (*it)->model_->columnCount();
	}
	return count;
}

QVariant WGMergeProxy::data(const QModelIndex& index, int role) const
{
	if (orientation_ == Qt::Horizontal)
	{
		auto sourceColumn = index.column();
		for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
		{
			auto mapping = it->get();

			auto sourceColumnCount = mapping->model_->columnCount();
			if (sourceColumn >= sourceColumnCount)
			{
				sourceColumn -= sourceColumnCount;
				continue;
			}

			auto entryIt = mapping->entries_.find(index.row());
			if (entryIt == mapping->entries_.end())
			{
				return QVariant();
			}
			auto sourceRow = entryIt.value();

			auto roleIt = mapping->roles_.find(role);
			if (roleIt == mapping->roles_.end())
			{
				return QVariant();
			}
			auto sourceRole = roleIt.value();

			return mapping->model_->data(mapping->model_->index(sourceRow, sourceColumn), sourceRole);
		}
	}
	else
	{
		auto sourceRow = index.row();
		for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
		{
			auto mapping = it->get();

			auto sourceRowCount = mapping->model_->rowCount();
			if (sourceRow >= sourceRowCount)
			{
				sourceRow -= sourceRowCount;
				continue;
			}

			auto entryIt = mapping->entries_.find(index.column());
			if (entryIt == mapping->entries_.end())
			{
				return QVariant();
			}
			auto sourceColumn = entryIt.value();

			auto roleIt = mapping->roles_.find(role);
			if (roleIt == mapping->roles_.end())
			{
				return QVariant();
			}
			auto sourceRole = roleIt.value();

			return mapping->model_->data(mapping->model_->index(sourceRow, sourceColumn), sourceRole);
		}
	}

	return QVariant();
}

bool WGMergeProxy::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (orientation_ == Qt::Horizontal)
	{
		auto sourceColumn = index.column();
		for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
		{
			auto mapping = it->get();

			auto sourceColumnCount = mapping->model_->columnCount();
			if (sourceColumn >= sourceColumnCount)
			{
				sourceColumn -= sourceColumnCount;
				continue;
			}

			auto entryIt = mapping->entries_.find(index.row());
			if (entryIt == mapping->entries_.end())
			{
				return false;
			}
			auto sourceRow = entryIt.value();

			auto roleIt = mapping->roles_.find(role);
			if (roleIt == mapping->roles_.end())
			{
				return false;
			}
			auto sourceRole = roleIt.value();

			return mapping->model_->setData(mapping->model_->index(sourceRow, sourceColumn), value, sourceRole);
		}
	}
	else
	{
		auto sourceRow = index.row();
		for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
		{
			auto mapping = it->get();

			auto sourceRowCount = mapping->model_->rowCount();
			if (sourceRow >= sourceRowCount)
			{
				sourceRow -= sourceRowCount;
				continue;
			}

			auto entryIt = mapping->entries_.find(index.column());
			if (entryIt == mapping->entries_.end())
			{
				return false;
			}
			auto sourceColumn = entryIt.value();

			auto roleIt = mapping->roles_.find(role);
			if (roleIt == mapping->roles_.end())
			{
				return false;
			}
			auto sourceRole = roleIt.value();

			return mapping->model_->setData(mapping->model_->index(sourceRow, sourceColumn), value, sourceRole);
		}
	}

	return false;
}

QVariant WGMergeProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == orientation_)
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		if (section < 0 || section >= entries_.count())
		{
			return QVariant();
		}

		return entries_.at(section).first;
	}

	for (auto it = mappings_.begin(); it != mappings_.end(); ++it)
	{
		auto mapping = it->get();

		auto entryIt = mapping->entries_.find(section);
		if (entryIt == mapping->entries_.end())
		{
			continue;
		}
		auto sourceSection = entryIt.value();

		auto roleIt = mapping->roles_.find(role);
		if (roleIt == mapping->roles_.end())
		{
			continue;
		}
		auto sourceRole = roleIt.value();

		auto data = mapping->model_->headerData(sourceSection, orientation, sourceRole);
		if (data.isValid())
		{
			return data;
		}
	}

	return QVariant();
}

QHash<int, QByteArray> WGMergeProxy::roleNames() const
{
	return roleNames_;
}
}