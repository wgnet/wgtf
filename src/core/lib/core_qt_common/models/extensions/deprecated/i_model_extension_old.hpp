#ifndef I_MODEL_EXTENSION_OLD_HPP
#define I_MODEL_EXTENSION_OLD_HPP

#include "core_data_model/i_item_role.hpp"
#include "core_qt_common/models/role_provider.hpp"
#include "core_qt_common/qt_new_handler.hpp"

#include <cassert>

#include <QHash>
#include <QPersistentModelIndex>
#include <QVariant>

class QAbstractItemModel;

namespace wgt
{
class IQtFramework;

class IModelExtensionOld : public QObject
					  , public RoleProvider
{
	Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

public:
	IModelExtensionOld() {};
	virtual ~IModelExtensionOld() {};

	virtual QHash< int, QByteArray > roleNames() const = 0;

	virtual QVariant data( const QModelIndex &index, int role ) const { return QVariant::Invalid; }
	virtual bool setData( const QModelIndex &index, const QVariant &value, int role ) { return false; }

	virtual QVariant headerData( int section, Qt::Orientation orientation, int role ) const { return QVariant::Invalid; }
	virtual bool setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role ) { return false; }

	void init( IQtFramework* qtFramework )
	{
		qtFramework_ = qtFramework;
	}

	virtual void saveStates( const char * modelUniqueName ) {}
	virtual void loadStates( const char * modelUniqueName ) {}

public slots:
	virtual void onDataAboutToBeChanged( 
		const QModelIndex &index, int role, const QVariant &value ) {}
	virtual void onDataChanged( 
		const QModelIndex &index, int role, const QVariant &value ) {}
	virtual void onLayoutAboutToBeChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) {}
	virtual void onLayoutChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) {}
	virtual void onRowsAboutToBeInserted( 
		const QModelIndex& parent, int first, int last ) {}
	virtual void onRowsInserted(
		const QModelIndex & parent, int first, int last ) {}
	virtual void onRowsAboutToBeRemoved(
		const QModelIndex& parent, int first, int last ) {}
	virtual void onRowsRemoved( 
		const QModelIndex & parent, int first, int last ) {}

protected:
	IQtFramework * qtFramework_;
};
} // end namespace wgt
#endif // I_MODEL_EXTENSION_OLD_HPP
