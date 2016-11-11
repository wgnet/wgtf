#ifndef SEQUENCE_LIST_ADAPTER_HPP
#define SEQUENCE_LIST_ADAPTER_HPP

#include "i_list_adapter.hpp"
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
/**
    *	Converter for re-ordering or repeating model indexes.
    *	E.g. columnSequence: [0,1,2,0,1,2]
    *	Will create a Qt model with 6 columns,
    *	but the WGTF model only has 3 columns repeated twice.
    */
class SequenceListAdapter : public IListAdapter
{
	Q_OBJECT
	Q_PROPERTY(QVariant model READ getModel WRITE setModel)
	Q_PROPERTY(QList<int> sequence READ getSequence WRITE setSequence)

	DECLARE_QT_MEMORY_HANDLER

public:
	SequenceListAdapter();
	virtual ~SequenceListAdapter();

	virtual QAbstractItemModel* model() const override;

	/**
	    *	Calculate re-ordered Qt index.
	    *	Unless the sequence is empty, then return a normal Qt index.
	    *	@param row WGTF row of the item.
	    *		Should be in the range 0<=row<rowCount.
	    *	@param column WGTF row of the item.
	    *		Should be in the range 0<=column<columnCount.
	    *	@param parent WGTF parent of the item, if applicable.
	    *	@return the Qt index or invalid if the item was not found.
	    */
	virtual QModelIndex adaptedIndex(int row, int column, const QModelIndex& parent) const override;

	/**
	    *	Get the converted number of rows.
	    *	Unless the sequence is empty, then return the normal Qt row count.
	    *	@post rowCount should not be <0.
	    *	@param parent WGTF parent of the item, if applicable.
	    *	@return the number of rows or 0.
	    */
	virtual int rowCount(const QModelIndex& parent) const override;

private:
	void onParentDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
	                         const QVector<int>& roles) override;
	void onParentRowsAboutToBeInserted(const QModelIndex& parent, int first, int last) override;
	void onParentRowsInserted(const QModelIndex& parent, int first, int last) override;
	void onParentRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) override;
	void onParentRowsRemoved(const QModelIndex& parent, int first, int last) override;
	virtual void onParentRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                        const QModelIndex& destinationParent, int destinationRow) override;
	virtual void onParentRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                               const QModelIndex& destinationParent, int destinationRow) override;

	QVariant getModel() const;
	void setModel(const QVariant& model);

	QList<int> getSequence();
	void setSequence(const QList<int>& sequence);

private:
	QAbstractItemModel* model_;
	QList<int> sequence_;
};
} // end namespace wgt
#endif // SEQUENCE_LIST_ADAPTER_HPP
