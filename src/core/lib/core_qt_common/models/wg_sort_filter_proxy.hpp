#ifndef WG_SORT_FILTER_PROXY_HPP
#define WG_SORT_FILTER_PROXY_HPP

#include <QSortFilterProxyModel>
#include <memory>

namespace wgt
{

class WGSortFilterProxy : public QSortFilterProxyModel
{
	Q_OBJECT

	Q_PROPERTY( QObject * sortFilter READ getSortFilter WRITE setSortFilter NOTIFY sortFilterChanged )
	Q_PROPERTY( int sortColumn READ sortColumn )
	Q_PROPERTY( Qt::SortOrder sortOrder READ sortOrder )

signals:
	void sortFilterChanged();

public:
	WGSortFilterProxy();
	virtual ~WGSortFilterProxy();

	Q_INVOKABLE void sort( int column, Qt::SortOrder order = Qt::AscendingOrder ) override;

private:
	QObject * getSortFilter() const;
	void setSortFilter( QObject * sortFilter );

	void onSourceModelChanged();
	void onSortFilterChanged();

	bool lessThan( const QModelIndex & source_left, const QModelIndex & source_right ) const override;

	struct Impl;
	std::unique_ptr< Impl > impl_;
};

}

#endif // WG_SORT_FILTER_PROXY_HPP