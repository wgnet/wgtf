#ifndef SEQUENCE_LIST_ADAPTER_HPP
#define SEQUENCE_LIST_ADAPTER_HPP

#include "i_list_adapter.hpp"
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
class SequenceListAdapter : public IListAdapter
{
	Q_OBJECT
		Q_PROPERTY(QVariant model READ getModel WRITE setModel)
		Q_PROPERTY(QList< int > sequence READ getSequence WRITE setSequence)

	DECLARE_QT_MEMORY_HANDLER

public:
	SequenceListAdapter();
	virtual ~SequenceListAdapter();

	QAbstractItemModel * model() const;

	QModelIndex adaptedIndex(int row, int column, const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent) const;

private:
	void onParentDataChanged(const QModelIndex &topLeft, 
		const QModelIndex &bottomRight, const QVector<int> &roles);

	QVariant getModel() const;
	void setModel( const QVariant &model );

	QList< int > getSequence();
	void setSequence( const QList< int > &sequence );

private:
	QAbstractListModel *model_;
	QList< int > sequence_;
};
} // end namespace wgt
#endif // SEQUENCE_LIST_ADAPTER_HPP
