#ifndef COLUMN_LIST_ADAPTER_HPP
#define COLUMN_LIST_ADAPTER_HPP

#include "i_list_adapter.hpp"
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
class ColumnListAdapter : public IListAdapter
{
	Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

public:
	ColumnListAdapter( const QModelIndex & row );
	virtual ~ColumnListAdapter();

	QAbstractItemModel * model() const;

	QModelIndex adaptedIndex(int row, int column, const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent) const;

private:
	void onParentDataChanged(const QModelIndex &topLeft, 
		const QModelIndex &bottomRight, const QVector<int> &roles);

private:
	const QAbstractItemModel * model_;
	QPersistentModelIndex row_;
};
} // end namespace wgt
#endif // COLUMN_LIST_ADAPTER_HPP
