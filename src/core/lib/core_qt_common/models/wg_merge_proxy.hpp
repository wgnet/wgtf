#ifndef WG_MERGE_PROXY_HPP
#define WG_MERGE_PROXY_HPP

#include "qt_connection_holder.hpp"
#include <QAbstractItemModel>
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>

namespace wgt
{

/**
* Proxy layer which produces a property value table for multiple input items (models).
*
* This proxy assumes that each input model is a vertical list of properties for an item, such
* as that produced by ReflectedTreeModel. The name and value of each property are accessed
* by calling the data() method of the model with different role IDs. The role which represents
* the property name must be passed to the addModel() call.
*
* The proxy performs a union operation on the sets of properties from all the source models. The
* output model is a table with a row for each (unique) property and a column for each source item.
* The data() method of this proxy will return an invalid QVariant if the item indicated by the
* column does not have the property indicated by the row (so that the view can display an
* empty cell), or forward the call to the source model if the property exists. The setData()
* method similarly forwards the value to the source model which corresponds to the item.
*
* The class also supports source models which are laid out horizontally, i.e. a single row with
* a column for each property. The orientation field should be set to Qt::Vertical in that
* case, which will also transpose the output: items will be rows, properties will be columns.
*
* This proxy is used by the MultiEdit control. The control needs to arrange properties in columns
* and items in rows, but note that it cannot set the orientation to vertical to achieve this due
* to the layout of the source models. A WGTransposeProxy is used on the output of this proxy instead.
*/
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

	Q_INVOKABLE virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const override;

	Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	Q_INVOKABLE virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	Q_INVOKABLE virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	
	Q_INVOKABLE virtual int roleId(const QString& roleName) const;

	virtual QHash<int, QByteArray> roleNames() const override;

	virtual QStringList mimeTypes() const override;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

private:
	struct Mapping
	{
		QAbstractItemModel* model_;
		QMap<int, int> entries_;
		QMap<int, int> roles_;
		QtConnectionHolder connections_;
	};

	Qt::Orientation getOrientation() const;
	void setOrientation(Qt::Orientation orientation);

	void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	std::pair<Mapping*, QModelIndex> mappingFromIndex(int row, int column) const;
	std::pair<Mapping*, QModelIndex> mappingFromIndex(const QModelIndex& index) const;

	std::vector<std::unique_ptr<Mapping>> mappings_;
	Qt::Orientation orientation_;
	QList<QPair<QVariant, int>> entries_;
	QHash<int, int> roles_;
	QHash<int, QByteArray> roleNames_;
};
}

#endif // WG_MERGE_PROXY_HPP