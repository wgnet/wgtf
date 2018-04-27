#include "core_data_model/i_item_role.hpp"
#include "role_provider.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "qt_abstract_item_model.hpp"
#include "qml_model_data.hpp"
#include "qml_model_row.hpp"

#include <QObject>
#include <QVector>
#include <QHash>

namespace wgt
{
struct QmlItemModelHelper: public RoleProvider
{
	QmlItemModelHelper(QtAbstractItemModel& self, const QHash<int, QByteArray>& roleNames)
		: self_(self), header_(nullptr), columnCount_(0), roleNames_(roleNames)
	{
		resetPendingChanges();
	}

	virtual void addRowConnections(QmlModelRow* row, QtConnectionHolder& connections)
	{
		auto connectionsPointer = &connections;

		auto columnChangedCallback = [this](int index, QmlModelData* column) {
			this->addRoles(column);
			this->updateColumnCount(index + 1);
			this->flushPendingChanges();
		};

		auto columnsRemovedCallback = [connectionsPointer]()
		{
			connectionsPointer->reset();
		};

		connections += self_.connect(row, &QmlModelRow::columnAdded, columnChangedCallback);
		connections += self_.connect(row, &QmlModelRow::columnChanged, columnChangedCallback);
		connections += self_.connect(row, &QmlModelRow::columnsCleared, columnsRemovedCallback);
	}

	void setHeader(QmlModelRow* row)
	{
		if (header_ != nullptr)
		{
			headerColumnConnections_.reset();
		}

		header_ = row;

		if (row != nullptr)
		{
			addRoles(row);
			addRowConnections(row, headerColumnConnections_);
			flushPendingChanges();
		}
	}

	void addRoles(QmlModelRow* row)
	{
		auto& columns = row->columns();
		updateColumnCount(columns.count());

		for (auto column : columns)
		{
			addRoles(column);
		}
	}

	void addRoles(QmlModelData* column)
	{
		const auto& roles = column->roles();
		const auto& keys = roles.keys();

		for (const auto& roleName : keys)
		{
			QByteArray bytes = roleName.toLocal8Bit();
			
			if (registerRole(bytes.constData(), pendingRoleNames_))
			{
				rolesPending_ = true;
			}
		}
	}

	void updateColumnCount(int newCount)
	{
		pendingColumnCount_ = std::max(pendingColumnCount_, newCount);
	}

	QVariant data(QmlModelRow* row, int index, int role)
	{
		auto& columns = row->columns();

		const bool invalidColumn = index < 0 || index >= columns.count();

		if (invalidColumn)
		{
			return QVariant::Invalid;
		}

		auto& roles = columns[index]->roles();
		auto roleName = roleNames_[role];
		auto itr = roles.find(roleName);

		if (itr == roles.end())
		{
			return QVariant::Invalid;
		}

		return itr.value();
	}

	bool setData(QmlModelRow* row, int index, int role, QVariant value)
	{
		auto& columns = row->columns();

		const bool invalidColumn = index < 0 || index >= columns.count();

		if (invalidColumn)
		{
			return false;
		}

		auto& roles = columns[index]->roles();
		auto roleName = roleNames_[role];
		auto itr = roles.find(roleName);

		if (itr == roles.end() || itr.value() == value)
		{
			return false;
		}

		roles[roleName] = value;
		return true;
	}

	void resetPendingChanges()
	{
		pendingRoleNames_ = roleNames_;
		pendingColumnCount_ = columnCount_;
		rolesPending_ = false;
	}

	bool pendingChanges()
	{
		return rolesPending_ || columnCount_ != pendingColumnCount_;
	}

	virtual void flushPendingChanges()
	{
		columnCount_ = pendingColumnCount_;
		roleNames_ = pendingRoleNames_;
		rolesPending_ = false;
	}

	QtAbstractItemModel& self_;
	int columnCount_;
	int pendingColumnCount_;
	bool rolesPending_;
	QHash<int, QByteArray> roleNames_;
	QHash<int, QByteArray> pendingRoleNames_;
	QtConnectionHolder headerColumnConnections_;
	QmlModelRow* header_;
};
} // end namespace wgt
