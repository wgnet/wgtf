#ifndef WG_FLATTEN_PROXY_HPP
#define WG_FLATTEN_PROXY_HPP

#include <QAbstractProxyModel>
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/helpers/wgt_interface_provider.hpp"

#include <memory>

namespace wgt
{

/**
* Proxy layer which flattens a hierarchical model to a list.
*
* This is used when the data in a tree model must be presented or processed as a flat
* list. The asset browser places it as an adapter between its hierarchical data model
* and a filter proxy, to produce flattened data for the search results view.
*/
class WGFlattenProxy 
	: public QAbstractProxyModel
	, public WGTInterfaceProvider
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	WGFlattenProxy();
	virtual ~WGFlattenProxy();

	virtual void setSourceModel(QAbstractItemModel* model) override;

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
	void onSourceRowsInserted(const QModelIndex& parent, int first, int last);
	void onSourceRowsRemoved(const QModelIndex& parent, int first, int last);

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
}

#endif // WG_FLATTEN_PROXY_HPP