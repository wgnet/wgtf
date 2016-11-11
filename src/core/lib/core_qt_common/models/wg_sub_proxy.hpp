#ifndef WG_SUB_PROXY_HPP
#define WG_SUB_PROXY_HPP

#include <QAbstractProxyModel>
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>

namespace wgt
{
class WGSubProxy : public QAbstractProxyModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

	Q_PROPERTY(QModelIndex sourceParent READ getSourceParent WRITE setSourceParent NOTIFY sourceParentChanged)

public:
	WGSubProxy();
	virtual ~WGSubProxy();

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

signals:
	void sourceParentChanged();

private:
	QModelIndex getSourceParent() const;
	void setSourceParent(const QModelIndex& sourceParent);

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
}

#endif // WG_TRANSPOSE_PROXY_HPP