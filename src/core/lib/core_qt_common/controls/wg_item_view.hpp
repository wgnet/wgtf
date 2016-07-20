#ifndef WG_ITEM_VIEW_HPP
#define WG_ITEM_VIEW_HPP

#include "core_qt_common/qt_new_handler.hpp"

#include <memory>
#include <QQuickItem>
#include <QQmlListProperty>

class QAbstractItemModel;
class QString;

namespace wgt
{
class IModelExtension;

class WGItemView : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY( QAbstractItemModel * model READ getModel WRITE setModel NOTIFY modelChanged )

	Q_PROPERTY( QStringList roles READ getRoles WRITE setRoles )
	Q_PROPERTY( QQmlListProperty< wgt::IModelExtension > extensions READ getExtensions )

	Q_PROPERTY( QAbstractItemModel * extendedModel READ getExtendedModel NOTIFY extendedModelChanged )
	//Enable for headers once body works.
	Q_PROPERTY( QList< QObject* > headerData READ getHeaderData NOTIFY headerDataChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGItemView();
	~WGItemView();

	Q_INVOKABLE int getRow( const QModelIndex & index ) const;
	Q_INVOKABLE int getColumn( const QModelIndex & index ) const;
	Q_INVOKABLE QModelIndex getParent( const QModelIndex & index ) const;

signals:
	void modelChanged();
	void extendedModelChanged();
	void headerDataChanged();

private:
	QAbstractItemModel * getModel() const;
	void setModel( QAbstractItemModel * model );

	QStringList getRoles() const;
	void setRoles( const QStringList & roles );

	QQmlListProperty< IModelExtension > getExtensions() const;
	static void appendExtension( QQmlListProperty< IModelExtension > * property, IModelExtension * value );
	static int countExtensions( QQmlListProperty< IModelExtension > * property );
	static IModelExtension * extensionAt( QQmlListProperty< IModelExtension > * property, int index );
	static void clearExtensions( QQmlListProperty< IModelExtension > * property );

	QAbstractItemModel * getExtendedModel() const;
    QList< QObject* > getHeaderData() const;

	void refresh();

	struct Impl;
	std::unique_ptr< Impl > impl_;
};
} // end namespace wgt
#endif//WG_ITEM_VIEW_HPP
