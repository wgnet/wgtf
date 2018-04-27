#ifndef WG_SORT_FILTER_PROXY_HPP
#define WG_SORT_FILTER_PROXY_HPP

#include "core_qt_common/qt_private/qsortfilterproxymodel.h"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/helpers/wgt_interface_provider.hpp"
#include "core_qt_common/qt_filter_object.hpp"
#include <memory>

namespace wgt
{

/**
* Proxy layer which filters and sorts a source model using predicates provided from QML scripts.
*
* If a sort object is provided, it must have a lessThan method which receives two model indices
* and returns true if the first index should come before the second.
*
* If a filter object is provided, it should have one or more of the following methods:
*	- filterAcceptsRow(item)
*	- filterAcceptsColumn(item)
*	- filterAcceptsItem(item)
*
* The first two methods are similar to those present in QSortFilterProxyModel. The third is called
* for both rows and columns, if the row- or column-specific method is not defined.
*/
class WGSortFilterProxy
	: public QSortFilterProxyModel
	, public WGTInterfaceProvider
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

	Q_PROPERTY(QObject* sortObject READ getSortObject WRITE setSortObject NOTIFY sortObjectChanged)
	Q_PROPERTY(QObject* filterObject READ getFilterObject WRITE setFilterObject NOTIFY filterObjectChanged)

signals:
	void beforeSort();
	void afterSort();
	void sortObjectChanged();
	void filterObjectChanged();

public:
	WGSortFilterProxy();
	virtual ~WGSortFilterProxy();

	Q_INVOKABLE bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
	Q_INVOKABLE QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
	Q_INVOKABLE QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

	Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
	Q_INVOKABLE void invalidateFilter();

	enum SortFilterResult
	{
		REJECTED = 0,
		ACCEPTED = 1
	};
	Q_ENUMS(SortFilterResult)

private:
	QObject* getSortObject() const;
	void setSortObject(QObject* sortObject);

	QObject* getFilterObject() const;
	void setFilterObject(QObject* filterObject);

	void onSourceModelChanged();
	void onSortObjectChanged();
	void onFilterObjectChanged();

	SortFilterResult filterAcceptsRowNonRecursive(int source_row, const QModelIndex& source_parent) const;
	SortFilterResult filterAcceptsColumnNonRecursive(int source_column, const QModelIndex& source_parent) const;

	bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
	bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

	Variant indexToItemId(const QModelIndex& index) const;

	struct Impl;
	std::unique_ptr<Impl> impl_;
};
}

#endif // WG_SORT_FILTER_PROXY_HPP