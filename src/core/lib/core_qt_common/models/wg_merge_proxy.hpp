#ifndef WG_MERGE_PROXY_HPP
#define WG_MERGE_PROXY_HPP

#include "qt_connection_holder.hpp"
#include <QAbstractItemModel>
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>

namespace wgt
{
class WGMergeProxy : public QAbstractItemModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	WGMergeProxy();
	virtual ~WGMergeProxy();

	Q_INVOKABLE void addModel(QAbstractItemModel* model, int index = 0, const QString& roleName = "display");
	Q_INVOKABLE void removeModel(QAbstractItemModel* model);

	Q_PROPERTY(Qt::Orientation orientation READ getOrientation WRITE setOrientation);

	Q_INVOKABLE virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const;

	Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	Q_INVOKABLE virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	Q_INVOKABLE virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	Q_INVOKABLE virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual QHash<int, QByteArray> roleNames() const override;

private:
	Qt::Orientation getOrientation() const;
	void setOrientation(Qt::Orientation orientation);

	void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

	struct Mapping
	{
		QAbstractItemModel* model_;
		QMap<int, int> entries_;
		QMap<int, int> roles_;
		QtConnectionHolder connections_;
	};
	std::vector<std::unique_ptr<Mapping>> mappings_;

	Qt::Orientation orientation_;
	QList<QPair<QVariant, int>> entries_;
	QHash<int, int> roles_;
	QHash<int, QByteArray> roleNames_;
};
}

#endif //WG_MERGE_PROXY_HPP