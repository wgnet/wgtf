#ifndef WG_RANGE_PROXY_HPP
#define WG_RANGE_PROXY_HPP

#include <QAbstractProxyModel>
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>

namespace wgt
{

/**
* Proxy layer which exposes a sub-range of the source model.
*/
class WGRangeProxy : public QAbstractProxyModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	WGRangeProxy();
	virtual ~WGRangeProxy();

	virtual void setSourceModel(QAbstractItemModel* model) override;

	Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
	Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

	Q_INVOKABLE virtual QModelIndex index(int row, int column,
	                                      const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const override;

	Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
	                           int role = Qt::EditRole) override;

	virtual QHash<int, QByteArray> roleNames() const override;

	Q_INVOKABLE void setRange(int top, int left, int bottom, int right);

signals:
	void rowsChanged(int first, int last);
	void columnsChanged(int first, int last);

private:
	QModelIndex mapLowerBound(const QModelIndex& sourceIndex) const;
	QModelIndex mapUpperBound(const QModelIndex& sourceIndex) const;

	bool isValid() const;
	void reset();

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
}

#endif // WG_RANGE_PROXY_HPP
