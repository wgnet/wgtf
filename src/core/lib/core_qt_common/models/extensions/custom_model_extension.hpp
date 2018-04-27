#pragma once

#include "qt_model_extension.hpp"
#include "core_object\i_object_handle_storage.hpp"
#include "core_object\i_managed_object.hpp"

#include <cassert>
#include <memory>
#include <functional>

#include <QHash>
#include <QPersistentModelIndex>
#include <QVariant>

class QAbstractItemModel;

namespace wgt
{
class QtFrameworkCommon;

/** A user extension to a view.
An extension provides additional functionality to a model, by adding additional roles.
Any roles defined in this extension are exposed as additional properties in QML.*/
class CustomModelExtension : public QtModelExtension
{
	Q_OBJECT
	Q_PROPERTY(QVariant extension READ getExtension)

public:
	typedef std::function<QModelIndex(int, int, void*, const QAbstractItemModel* model)> ModelIndexCreationFunction;

	CustomModelExtension();
	virtual ~CustomModelExtension();

	void initialise(QtFrameworkCommon& frameworkCommon, QObject* view, ManagedObjectPtr&& object);

	virtual QVariant data(const QModelIndex& index, ItemRole::Id roleId) const override;

	virtual bool setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId) override;

	virtual QVariant headerData(int section, Qt::Orientation orientation, ItemRole::Id roleId) const override;

	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
	                           ItemRole::Id roleId) override;

	void setCreateIndexFunction(const ModelIndexCreationFunction& function);

public slots:
	virtual void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
	                           const QVector<int>& roles) override;
	virtual void onHeaderDataChanged(Qt::Orientation orientation, int first, int last) override;
	virtual void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                                      QAbstractItemModel::LayoutChangeHint hint) override;
	virtual void onLayoutChanged(const QList<QPersistentModelIndex>& parents,
	                             QAbstractItemModel::LayoutChangeHint hint) override;
	virtual void onRowsAboutToBeInserted(const QModelIndex& parent, int first, int last) override;
	virtual void onRowsInserted(const QModelIndex& parent, int first, int last) override;
	virtual void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) override;
	virtual void onRowsRemoved(const QModelIndex& parent, int first, int last) override;
	virtual void onRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                  const QModelIndex& destinationParent, int destinationRow) override;
	virtual void onRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                         const QModelIndex& destinationParent, int destinationRow) override;
	virtual void onColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last) override;
	virtual void onColumnsInserted(const QModelIndex& parent, int first, int last) override;
	virtual void onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last) override;
	virtual void onColumnsRemoved(const QModelIndex& parent, int first, int last) override;
	virtual void onColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                     const QModelIndex& destinationParent, int destinationColumn) override;
	virtual void onColumnsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                            const QModelIndex& destinationParent, int destinationColumn) override;

private:
	QVariant getExtension() const;

	struct Implementation;
	friend Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
