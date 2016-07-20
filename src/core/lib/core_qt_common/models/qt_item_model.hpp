#ifndef QT_ITEM_MODEL_HPP
#define QT_ITEM_MODEL_HPP

#include "qt_abstract_item_model.hpp"
#include <memory>
#include "role_provider.hpp"

namespace wgt
{
class AbstractItemModel;
class AbstractListModel;
class AbstractTreeModel;
class AbstractTableModel;
class IComponentContext;


/**
 *	Adapter layer to allow any AbstractItemModel to be used by Qt and QML views.
 */
class QtItemModel : public QtAbstractItemModel, public RoleProvider
{
	Q_OBJECT

public:
	QtItemModel( IComponentContext & context, AbstractItemModel & source );
	virtual ~QtItemModel();

	/**
	 *	Get AbstractItemModel that is being adapted to be used by Qt.
	 */
	const AbstractItemModel & source() const;
	AbstractItemModel & source();

	// QAbstractItemModel
	QModelIndex index( int row, int column, const QModelIndex &parent ) const override;
	QModelIndex parent( const QModelIndex &child ) const override;

	int rowCount( const QModelIndex &parent ) const override;
	int columnCount( const QModelIndex &parent ) const override;
	bool hasChildren( const QModelIndex &parent ) const override;

	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index, const QVariant &value, int role ) override;

	QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
	bool setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role ) override;

	bool insertRows( int row, int count, const QModelIndex &parent ) override;
	bool insertColumns( int column, int count, const QModelIndex &parent ) override;
	bool removeRows( int row, int count, const QModelIndex &parent ) override;
	bool removeColumns( int column, int count, const QModelIndex &parent ) override;

	QHash< int, QByteArray > roleNames() const override;

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};


/**
 *	Provides QML invokable functions to iterate, insert and remove items by row.
 */
class QtListModel : public QtItemModel
{
	Q_OBJECT

public:
	QtListModel( IComponentContext & context,
		AbstractListModel & source );

	/**
	 *	Get AbstractListModel that is being adapted to be used by Qt.
	 */
	const AbstractListModel & source() const;
	AbstractListModel & source();

	/**
	 *	Get item from row in list.
	 */
	Q_INVOKABLE QObject * item( int row ) const;

	/**
	 *	Get number of rows in list.
	 */
	Q_INVOKABLE int count() const;

	/**
	 *	Add a new row.
	 *	@param row the position to insert the new item.
	 *		Inserts the item *before* the given row.
	 *		If row is 0, the row is added to the start.
	 *		If row is count(), the row is added to the end.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool insertItem( int row );

	/**
	 *	Remove a row.
	 *	@param row to be removed.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool removeItem( int row );
};


/**
 *	Provides QML invokable functions to iterate, insert and remove items by row
 *	and parent.
 */
class QtTreeModel : public QtItemModel
{
	Q_OBJECT

public:
	QtTreeModel( IComponentContext & context, AbstractTreeModel & source );

	/**
	 *	Get AbstractTreeModel that is being adapted to be used by Qt.
	 */
	const AbstractTreeModel & source() const;
	AbstractTreeModel & source();

	/**
	 *	Get item from row and parent.
	 */
	Q_INVOKABLE QObject * item( int row, QObject * parent ) const;

	/**
	 *	Get number of rows under parent.
	 */
	Q_INVOKABLE int count( QObject * parent ) const;

	/**
	 *	Add a new row under parent.
	 *	@param row the position under the parent to insert the new item.
	 *		Inserts the item *before* the given row.
	 *		If row is 0, the row is added to the start.
	 *		If row is count(), the row is added to the end.
	 *	@param parent item in tree.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool insertItem( int row, QObject * parent );

	/**
	 *	Remove a row under parent.
	 *	@param row to be removed.
	 *	@param parent item in tree.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool removeItem( int row, QObject * parent );
};


/**
 *	Provides QML invokable functions to iterate, insert and remove items by row
 *	and column.
 */
class QtTableModel : public QtItemModel
{
	Q_OBJECT

public:
	QtTableModel( IComponentContext & context, AbstractTableModel & source );

	/**
	 *	Get AbstractTableModel that is being adapted to be used by Qt.
	 */
	const AbstractTableModel & source() const;
	AbstractTableModel & source();

	/**
	 *	Get item from row and column.
	 */
	Q_INVOKABLE QObject * item( int row, int column ) const;

	/**
	 *	Get number of rows in table.
	 */
	Q_INVOKABLE int rowCount() const;

	/**
	 *	Get number of columns in table.
	 */
	Q_INVOKABLE int columnCount() const;

	/**
	 *	Add a new row.
	 *	@param row of the new item.
	 *		Inserts the item *before* the given row.
	 *		If row is 0, the row is added to the start.
	 *		If row is rowCount(), the row is added to the end.
	 *	@param column of the new item.
	 *		Inserts the item *before* the given column.
	 *		If column is 0, the column is added to the start.
	 *		If column is columnCount(), the column is added to the end.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool insertRow( int row );

	/**
	 *	Add a new column.
	 *	@param column of the new item.
	 *		Inserts the item *before* the given column.
	 *		If column is 0, the column is added to the start.
	 *		If column is columnCount(), the column is added to the end.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool insertColumn( int column );

	/**
	 *	Remove a row.
	 *	@param row of item to be removed.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool removeRow( int row );

	/**
	 *	Remove a column.
	 *	@param column of item to be removed.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool removeColumn( int column );

private:
	using QtItemModel::rowCount;
	using QtItemModel::columnCount;
};
} // end namespace wgt
#endif//QT_ITEM_MODEL_HPP
