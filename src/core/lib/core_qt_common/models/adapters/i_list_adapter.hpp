#ifndef I_LIST_ADAPTER_HPP
#define I_LIST_ADAPTER_HPP

#include <QAbstractItemModel>

#include "core_qt_common/qt_connection_holder.hpp"

namespace wgt
{
class IListAdapter : public QAbstractListModel
{
	Q_OBJECT

public:
	virtual ~IListAdapter();
	void reset();
	virtual void connect();
	virtual void disconnect();

	virtual QAbstractItemModel * model() const = 0;

	virtual QHash< int, QByteArray > roleNames() const;
	virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex adaptedIndex(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const = 0;
	virtual int rowCount(const QModelIndex &parent) const = 0;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
	
protected:
	virtual void onParentDataChanged(const QModelIndex &topLeft, 
		const QModelIndex &bottomRight, const QVector<int> &roles) {};
	virtual void onParentLayoutAboutToBeChanged(const QList<QPersistentModelIndex> & parents, 
		QAbstractItemModel::LayoutChangeHint hint) {};
	virtual void onParentLayoutChanged(const QList<QPersistentModelIndex> & parents, 
		QAbstractItemModel::LayoutChangeHint hint) {};
	virtual void onParentRowsAboutToBeInserted(const QModelIndex & parent, int first, int last) {};
	virtual void onParentRowsInserted(const QModelIndex & parent, int first, int last) {};
	virtual void onParentRowsAboutToBeRemoved(const QModelIndex & parent, int first, int last) {};
	virtual void onParentRowsRemoved(const QModelIndex & parent, int first, int last) {};

private:
	QModelIndex cachedAdaptedIndex(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;

	mutable std::map<int, QPersistentModelIndex> cachedAdaptedIndices_;
	QtConnectionHolder connections_;
};
} // end namespace wgt
#endif // I_LIST_ADAPTER_HPP
