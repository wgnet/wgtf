#ifndef I_MODEL_EXTENSION_HPP
#define I_MODEL_EXTENSION_HPP

#include "core_data_model/i_item_role.hpp"
#include "core_qt_common/models/role_provider.hpp"
#include "core_qt_common/qt_new_handler.hpp"

#include <cassert>

#include <QHash>
#include <QPersistentModelIndex>
#include <QVariant>

class QAbstractItemModel;

namespace wgt
{
class IQtFramework;

/** Data storage for extensions.*/
class IExtensionData
{
public:
	virtual bool encodeRole(ItemRole::Id roleId, int& o_Role) const = 0;
	virtual bool decodeRole(int role, ItemRole::Id& o_RoleId) const = 0;

	virtual bool encodeRoleExt(ItemRole::Id roleId, int& o_Role) const = 0;
	virtual bool decodeRoleExt(int role, ItemRole::Id& o_RoleId) const = 0;

	virtual QVariant data(const QModelIndex& index, ItemRole::Id roleId) = 0;
	virtual bool setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId) = 0;

	virtual QVariant dataExt(const QModelIndex& index, ItemRole::Id roleId) = 0;
	virtual bool setDataExt(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId) = 0;

	virtual void dataExtChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
	                            const QVector<ItemRole::Id> roleIds) = 0;
};

/** An extension to a model or view.
An extension provides additional functionality to a model, by adding additional roles.
Any roles defined in this extension are exposed as additional properties in QML.*/
class IModelExtension : public QObject
{
	Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

public:
	IModelExtension();
	virtual ~IModelExtension();

	/** Initialise the extension with initial data.
	@param extensionData_ Initial data.*/
	void init(IExtensionData& extensionData_);
	/** Returns all roles used by the extension.
	@return A vector of role names.*/
	const std::vector<std::string>& roles() const;

	/** Get role data at an index position.
	@param index The position the data applies to.
	@param role The decoded role identifier.
	@return The role data.*/
	virtual QVariant data(const QModelIndex& index, ItemRole::Id roleId) const;

	/** Set role data at an index position.
	@param index The position the data applies to.
	@param value The role data.
	@param role The decoded role identifier.
	@return True if successful.*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId);

	/** Get role data for a header row or column.
	@param section The row or column number.
	@param orientation Specifies whether section refers to a row or column.
	@param role The decoded role identifier.
	@return The role data.*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, ItemRole::Id roleId) const;

	/** Set role data for a header row or column.
	@param section The row or column number.
	@param orientation Specifies whether section refers to a row or column.
	@param value The role data.
	@param role The decoded role identifier.
	@return True if successful.*/
	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, ItemRole::Id roleId);

public slots:
	virtual void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
	{
	}
	virtual void onHeaderDataChanged(Qt::Orientation orientation, int first, int last)
	{
	}
	virtual void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                                      QAbstractItemModel::LayoutChangeHint hint)
	{
	}
	virtual void onLayoutChanged(const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint)
	{
	}
	virtual void onRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onRowsInserted(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onRowsRemoved(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                  const QModelIndex& destinationParent, int destinationRow)
	{
	}
	virtual void onRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                         const QModelIndex& destinationParent, int destinationRow)
	{
	}
	virtual void onColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onColumnsInserted(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onColumnsRemoved(const QModelIndex& parent, int first, int last)
	{
	}
	virtual void onColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                     const QModelIndex& destinationParent, int destinationRow)
	{
	}
	virtual void onColumnsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                            const QModelIndex& destinationParent, int destinationColumn)
	{
	}

protected:
	IExtensionData* extensionData_;
	std::vector<std::string> roles_;
};
} // end namespace wgt
#endif // I_MODEL_EXTENSION_HPP
