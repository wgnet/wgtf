#include "value_extension.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"

namespace wgt
{
ITEMROLE(readOnly)
ITEMROLE(enabled)
ITEMROLE(multipleValues)

ValueExtension::ValueExtension()
{
	
}

ValueExtension::~ValueExtension()
{
}

QHash< int, QByteArray > ValueExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	registerRole( ValueRole::roleName_, roleNames );
	registerRole( ValueTypeRole::roleName_, roleNames );
	registerRole( EnumModelRole::roleName_, roleNames );
	registerRole( DefinitionRole::roleName_, roleNames );
	registerRole( DefinitionModelRole::roleName_, roleNames );
    registerRole( ObjectRole::roleName_, roleNames );
    registerRole( RootObjectRole::roleName_, roleNames );
	registerRole( KeyRole::roleName_, roleNames );
	registerRole( KeyTypeRole::roleName_, roleNames );
	registerRole( MinValueRole::roleName_, roleNames );
	registerRole( MaxValueRole::roleName_, roleNames );
	registerRole( StepSizeRole::roleName_, roleNames);
	registerRole( DecimalsRole::roleName_, roleNames);
	registerRole( IndexPathRole::roleName_, roleNames );
	registerRole( UrlIsAssetBrowserRole::roleName_, roleNames );
	registerRole( UrlDialogTitleRole::roleName_, roleNames );
	registerRole( UrlDialogDefaultFolderRole::roleName_, roleNames );
	registerRole( UrlDialogNameFiltersRole::roleName_, roleNames );
	registerRole( UrlDialogSelectedNameFilterRole::roleName_, roleNames );
	registerRole( UrlDialogModalityRole::roleName_, roleNames );
	registerRole( IsReadOnlyRole::roleName_, roleNames );
	registerRole( ItemRole::readOnlyName, roleNames );
	registerRole( ItemRole::enabledName, roleNames );
	registerRole( ItemRole::multipleValuesName, roleNames );
	for (auto i = 0; i < roles_.count(); ++i)
	{
		registerRole( roles_[i].toUtf8().constData(), roleNames );
	}
	return roleNames;
}

QVariant ValueExtension::data( const QModelIndex &index, int role ) const
{
	ItemRole::Id roleId;
	if (!decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	assert( index.isValid() );
	auto item = reinterpret_cast< IItem * >( index.internalPointer() );
	if (item == nullptr)
	{
		return false;
	}
	auto column = index.column();

	if (roleId == ValueRole::roleId_ ||
		roleId == ValueTypeRole::roleId_ ||
        roleId == ObjectRole::roleId_ ||
        roleId == RootObjectRole::roleId_ ||
		roleId == IndexPathRole::roleId_ ||
		roleId == KeyRole::roleId_ ||
		roleId == KeyTypeRole::roleId_ ||
		roleId == MinValueRole::roleId_ ||
		roleId == MaxValueRole::roleId_ ||
		roleId == StepSizeRole::roleId_ ||
		roleId == DecimalsRole::roleId_ ||
		roleId == EnumModelRole::roleId_ ||
		roleId == DefinitionRole::roleId_ ||
		roleId == DefinitionModelRole::roleId_ ||
		roleId == UrlIsAssetBrowserRole::roleId_ ||
		roleId == UrlDialogTitleRole::roleId_ ||
		roleId == UrlDialogDefaultFolderRole::roleId_ ||
		roleId == UrlDialogNameFiltersRole::roleId_ ||
		roleId == UrlDialogSelectedNameFilterRole::roleId_ ||
		roleId == UrlDialogModalityRole::roleId_ ||
		roleId == IsReadOnlyRole::roleId_ ||
		roleId == ItemRole::readOnlyId ||
		roleId == ItemRole::enabledId ||
		roleId == ItemRole::multipleValuesId)
	{
		return QtHelpers::toQVariant( item->getData( column, roleId ), const_cast<QAbstractItemModel*>(index.model()));
	}
	return QVariant( QVariant::Invalid );
}

bool ValueExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	ItemRole::Id roleId;
	if (!decodeRole( role, roleId ))
	{
		return false;
	}

	assert( index.isValid() );
	auto item = reinterpret_cast< IItem * >( index.internalPointer() );
	if (item == nullptr)
	{
		return false;
	}
	auto column = index.column();

	if (roleId == ValueRole::roleId_ ||
		roleId == DefinitionRole::roleId_)	{
		auto oldValue = QtHelpers::toQVariant(
			item->getData( column, roleId ), const_cast<QAbstractItemModel*>(index.model()));
		if (value == oldValue)
		{
			return true;
		}

		auto data = QtHelpers::toVariant( value );
		return item->setData( column, roleId, data );
	}

	return false;
}

void ValueExtension::onDataAboutToBeChanged( const QModelIndex &index, int role, const QVariant &value )
{
	auto model = index.model();
	assert( model != nullptr );

	ItemRole::Id roleId;
	if (!decodeRole( role, roleId ))
	{
		return;
	}

	if (roleId == DefinitionRole::roleId_)
	{
		QList<QPersistentModelIndex> parents;
		parents.append( index );
		emit const_cast< QAbstractItemModel * >( model )->layoutAboutToBeChanged( parents, QAbstractItemModel::VerticalSortHint );
	}
}

void ValueExtension::onDataChanged( const QModelIndex &index, int role, const QVariant &value )
{
	auto model = index.model();
	assert( model != nullptr );

	ItemRole::Id roleId;
	if (!decodeRole( role, roleId ))
	{
		return;
	}

	if (roleId == ValueRole::roleId_ ||
		roleId == DefinitionRole::roleId_)
	{
		QVector<int> roles;
		roles.append( role );
		emit const_cast< QAbstractItemModel * >( model )->dataChanged( index, index, roles );
	}

	if (roleId == DefinitionRole::roleId_)
	{
		QList<QPersistentModelIndex> parents;
		parents.append( index );
		emit const_cast< QAbstractItemModel * >( model )->layoutChanged( parents, QAbstractItemModel::VerticalSortHint );
	}
}

QQmlListProperty< QString > ValueExtension::getRoles() const
{
	return QQmlListProperty< QString >(
		const_cast< ValueExtension * >( this ),
		nullptr,
		&appendRole, 
		&countRoles,
		&roleAt, 
		&clearRoles );
}

void ValueExtension::appendRole( 
	QQmlListProperty< QString > * property, 
	QString * value )
{
	auto valueExtension = qobject_cast< ValueExtension * >( property->object );
	if (valueExtension == nullptr)
	{
		return;
	}

	// beginResetModel
	valueExtension->roles_.append( *value );
	// endResetModel
}

QString * ValueExtension::roleAt( 
	QQmlListProperty< QString > * property, 
	int index )
{
	auto valueExtension = qobject_cast< ValueExtension * >( property->object );
	if (valueExtension == nullptr)
	{
		return nullptr;
	}

	return &valueExtension->roles_[index];
}

void ValueExtension::clearRoles( 
	QQmlListProperty< QString > * property )
{
	auto valueExtension = qobject_cast< ValueExtension * >( property->object );
	if (valueExtension == nullptr)
	{
		return;
	}

	// beginResetModel
	valueExtension->roles_.clear();
	// endResetModel
}

int ValueExtension::countRoles( 
	QQmlListProperty< QString > * property )
{
	auto valueExtension = qobject_cast< ValueExtension * >( property->object );
	if (valueExtension == nullptr)
	{
		return 0;
	}

	return valueExtension->roles_.count();
}
} // end namespace wgt
