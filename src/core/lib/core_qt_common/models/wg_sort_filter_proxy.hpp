#ifndef WG_SORT_FILTER_PROXY_HPP
#define WG_SORT_FILTER_PROXY_HPP

#include "qt_private/qsortfilterproxymodel.h"
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>

namespace wgt
{

class WGSortFilterProxy : public QSortFilterProxyModel
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

	Q_PROPERTY( QObject * sortObject READ getSortObject WRITE setSortObject NOTIFY sortObjectChanged )
	Q_PROPERTY( QObject * filterObject READ getFilterObject WRITE setFilterObject NOTIFY filterObjectChanged )

signals:
	void sortObjectChanged();
	void filterObjectChanged();

public:
	WGSortFilterProxy();
	virtual ~WGSortFilterProxy();

	Q_INVOKABLE void sort( int column, Qt::SortOrder order = Qt::AscendingOrder ) override;
	Q_INVOKABLE void invalidateFilter();

private:
	QObject * getSortObject() const;
	void setSortObject( QObject * sortObject );

	QObject * getFilterObject() const;
	void setFilterObject( QObject * filterObject );

	void onSourceModelChanged();
	void onSortObjectChanged();
	void onFilterObjectChanged();

	bool filterAcceptsRowNonRecursive(int source_row, const QModelIndex &source_parent) const;
	bool filterAcceptsColumnNonRecursive(int source_column, const QModelIndex &source_parent) const;

	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
	bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
	bool lessThan( const QModelIndex & source_left, const QModelIndex & source_right ) const override;

	struct Impl;
	std::unique_ptr< Impl > impl_;
};

}

#endif // WG_SORT_FILTER_PROXY_HPP