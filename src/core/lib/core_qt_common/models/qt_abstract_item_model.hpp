#ifndef QT_ABSTRACT_ITEM_MODEL_HPP
#define QT_ABSTRACT_ITEM_MODEL_HPP

#include <QAbstractItemModel>

#include <memory>

namespace wgt
{

/**
 *	Adapter layer to allow AbstractItemModels or ExtendedModels to be used by
 *	Qt and QML views.
 */
class QtAbstractItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	QtAbstractItemModel();
	virtual ~QtAbstractItemModel();

	Q_INVOKABLE QModelIndex itemToIndex( QObject* item ) const;
	Q_INVOKABLE QObject * indexToItem( const QModelIndex &index ) const;

	Q_INVOKABLE QObject * item( int row, int column, QObject * parent ) const;
	Q_INVOKABLE QObject * parent( QObject * child ) const;

	Q_INVOKABLE int rowIndex( QObject * item ) const;
	Q_INVOKABLE int columnIndex( QObject * item ) const;

	Q_INVOKABLE int rowCount( QObject * parent ) const;
	Q_INVOKABLE int columnCount( QObject * parent ) const;
	Q_INVOKABLE bool hasChildren( QObject * parent ) const;

	Q_INVOKABLE bool insertRow( int row, QObject * parent );
	Q_INVOKABLE bool insertColumn( int column, QObject * parent );
	Q_INVOKABLE bool removeRow( int row, QObject * parent );
	Q_INVOKABLE bool removeColumn( int column, QObject * parent );

	// Explicitly declare virtual functions from QAbstractItemModel to prevent them from being hidden
	virtual QModelIndex parent(const QModelIndex &child) const = 0;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const = 0;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const = 0;
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};
} // end namespace wgt
#endif//QT_ABSTRACT_ITEM_MODEL_HPP
