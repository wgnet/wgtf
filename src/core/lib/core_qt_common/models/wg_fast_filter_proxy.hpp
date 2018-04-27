#ifndef WG_FAST_FILTER_PROXY_HPP
#define WG_FAST_FILTER_PROXY_HPP

#include <QAbstractProxyModel>
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>

namespace wgt
{

/**
* Proxy layer which filters items based on the text value of a given property (role).
*
* Each item in a Qt model has a set of data elements associated with it. The data elements
* have various roles, such as display text, icon, tooltip text etc. It is also possible to
* have user-defined roles, which we use for exposing the path of an asset, read-only or hidden
* flags etc. This proxy will only output those items in the source model for which the data
* associated with the role specified in the filterRole property matches the expression given
* by filterText.
*
* Matching is performed using the FilterExpression class. In its simplest form, this just
* performs a substring match, but logic and grouping operators can be used for more complex
* filters, e.g. street|road will match all strings which contain either "street" or "road".
*
* This class also exposes a tryComplete method, which can be invoked from QML to implement
* autocompletion in search fields. The call will return the first string (in alphabetical
* order) which matches the filter substring (expressions which contain operators do not
* provide auto complete suggestions).
*/
class WGFastFilterProxy : public QAbstractProxyModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

public:
	WGFastFilterProxy();
	virtual ~WGFastFilterProxy();

	Q_PROPERTY(QString filterRole READ getFilterRole WRITE setFilterRole NOTIFY filterRoleChanged)
	Q_PROPERTY(int filterRoleId READ getFilterRoleId NOTIFY filterTextChanged)
	Q_PROPERTY(QString filterText READ getFilterText WRITE setFilterText NOTIFY filterTextChanged)

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

	Q_INVOKABLE QString tryComplete(const QString& str, bool multi = false) const;

signals:
	void filterRoleChanged();
	void filterTextChanged();

private:
	QString getFilterRole() const;
	int getFilterRoleId() const;
	QString getFilterText() const;

	void setFilterRole(const QString& filterRole);
	void setFilterText(const QString& filterText);

	void onFilterChanged();

	void onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void onSourceRowsInserted(const QModelIndex& parent, int first, int last);
	void onSourceRowsRemoved(const QModelIndex& parent, int first, int last);

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
}

#endif // WG_FAST_FILTER_PROXY_HPP