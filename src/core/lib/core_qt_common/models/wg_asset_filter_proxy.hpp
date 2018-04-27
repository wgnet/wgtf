#ifndef WG_ASSET_FILTER_PROXY_HPP
#define WG_ASSET_FILTER_PROXY_HPP

#include <QAbstractProxyModel>
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/helpers/wgt_interface_provider.hpp"
#include <memory>

namespace wgt
{

/**
* Proxy layer which filters assets depending on their type and hidden flag.
*
* This is used by WGAssetBrowser on top of its base data model to control the display
* of hidden objects based on the showHidden setting, and to filter assets by their
* extension.
*/
class WGAssetFilterProxy
	: public QAbstractProxyModel
	, public WGTInterfaceProvider
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	WGAssetFilterProxy();
	virtual ~WGAssetFilterProxy();

	Q_PROPERTY(QString extension READ getExtension WRITE setExtension NOTIFY extensionChanged)
	Q_PROPERTY(bool showHidden READ getShowHidden WRITE setShowHidden NOTIFY showHiddenChanged)

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

signals:
	void extensionChanged();
	void showHiddenChanged();
	void filterObjectChanged();

private:
	QString getExtension() const;
	bool getShowHidden() const;

	void setExtension(const QString& extension);
	void setShowHidden(bool showHidden);

	void onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void onSourceRowsAboutToBeInserted(const QModelIndex& parent, int first, int last);
	void onSourceRowsInserted(const QModelIndex& parent, int first, int last);
	void onSourceRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
	void onSourceRowsRemoved(const QModelIndex& parent, int first, int last);

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
}

#endif // WG_ASSET_FILTER_PROXY_HPP