#ifndef WG_COLUMN_LAYOUT_PROXY_HPP
#define WG_COLUMN_LAYOUT_PROXY_HPP

#include <QAbstractProxyModel>
#include <memory>
#include "qt_connection_holder.hpp"
#include "role_provider.hpp"
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
/**
* Proxy layer to adapt a model by changing its model indices to redirect to different columns.
*
* The columnSequence property controls the remapping. It is possible to duplicate columns using
* this proxy, which is useful for showing different properties of an item on separate columns
* within a view. For example, WGAssetListView uses this proxy with columnSequence set to
* [0, 0, 0, 0] to create 4 columns for each row, all pointing to the same item in the source
* model (which has no concept of columns). The columnDelegates property of WGListView is then used
* to extract 4 different properties of the asset (contents, date, type and size) and populate the
* columns of the list view with them.
*/
class WGColumnLayoutProxy : public QAbstractProxyModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER
	Q_PROPERTY(QList<int> columnSequence READ getColumnSequence WRITE setColumnSequence NOTIFY columnSequenceChanged)

public:
	WGColumnLayoutProxy();
	virtual ~WGColumnLayoutProxy();

	// QAbstractProxyModel
	virtual void setSourceModel(QAbstractItemModel* sourceModel) override;

	virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
	virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
	QModelIndex mapToSourceParent(const QModelIndex& proxyIndex) const;
	QModelIndex mapFromSourceParent(const QModelIndex& sourceIndex) const;
	QModelIndexList mapAllFromSource(const QModelIndex& sourceIndex) const;

	// QAbstractItemModel
	Q_INVOKABLE virtual QModelIndex index(int row, int column,
	                                      const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const override;

	Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

	Q_INVOKABLE virtual QVariant data(const QModelIndex& index, int role) const override;
	Q_INVOKABLE virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation,
	                                        int role = Qt::DisplayRole) const override;
	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
	                           int role = Qt::EditRole) override;

	virtual QHash<int, QByteArray> roleNames() const override;

signals:
	void columnSequenceChanged();

private:
	QList<int> getColumnSequence() const;
	void setColumnSequence(const QList<int>& columnSequence);

	void onSourceModelAboutToBeReset();
	void onSourceModelReset();
	void onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void onSourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);
	void onSourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                                    QAbstractItemModel::LayoutChangeHint hint);
	void onSourceLayoutChanged(const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint);
	void onSourceRowsAboutToBeInserted(const QModelIndex& parent, int first, int last);
	void onSourceRowsInserted();
	void onSourceRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
	void onSourceRowsRemoved();
	void onSourceRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
	                                const QModelIndex& destinationParent, int destinationRow);
	void onSourceRowsMoved(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row);
	void onSourceColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last);
	void onSourceColumnsInserted();
	void onSourceColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
	void onSourceColumnsRemoved();
	void onSourceColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
	                                   const QModelIndex& destinationParent, int destinationColumn);
	void onSourceColumnsMoved(const QModelIndex& parent, int start, int end, const QModelIndex& destination,
	                          int column);

	struct Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif // WG_COLUMN_LAYOUT_PROXY_HPP