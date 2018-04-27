#ifndef WG_TRANSPOSE_PROXY_HPP
#define WG_TRANSPOSE_PROXY_HPP

#include <QAbstractProxyModel>
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{

/**
* Proxy layer which transposes the source model, i.e. swaps rows and columns.
*
* Used by the MultiEdit control to transpose the output of WGMergeProxy, which has
* items as columns and properties as rows, into a layout where items are rows, which
* is more suitable for presentation.
*/
class WGTransposeProxy : public QAbstractProxyModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	virtual void setSourceModel(QAbstractItemModel* sourceModel) override;

	Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
	Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

	Q_INVOKABLE virtual QModelIndex index(int row, int column,
	                                      const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const override;

	Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
	                           int role = Qt::EditRole) override;

	virtual QHash<int, QByteArray> roleNames() const override;

private:
	QtConnectionHolder connections_;
};
}

#endif // WG_TRANSPOSE_PROXY_HPP