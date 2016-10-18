#include "qt_collection_model.hpp"

#include "core_data_model/common_data_roles.hpp"
#include "helpers/qt_helpers.hpp"
#include "core_qt_script/qt_script_object.hpp"

#include <iterator>

namespace wgt
{
ITEMROLE( key )
ITEMROLE( keyType )


QtCollectionModel::QtCollectionModel( IComponentContext & context,
	std::unique_ptr< CollectionModel > && source )
	: QtListModel( context, *source.get() ), model_( std::move( source ) )
{
}

QHash< int, QByteArray > QtCollectionModel::roleNames() const
{
	auto roleNames = QtListModel::roleNames();
	registerRole( ItemRole::valueName, roleNames );
	registerRole( ItemRole::keyName, roleNames );
	registerRole( ItemRole::valueTypeName, roleNames );
	registerRole( ItemRole::keyTypeName, roleNames );
	return roleNames;
}

const CollectionModel & QtCollectionModel::source() const
{
	return static_cast< const CollectionModel & >( QtItemModel::source() );
}


CollectionModel & QtCollectionModel::source()
{
	return static_cast< CollectionModel & >( QtItemModel::source() );
}


bool QtCollectionModel::insertRows( int row,
	int count,
	const QModelIndex & parent )
{
	auto & collectionModel = this->source();

	// Item already uses the Command System or the Command System is not available
	const auto useController =
		(!collectionModel.hasController() && (itemModelController_ != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.insertRows( row, count );
	}

	// Queue with Command System, to register undo/redo data
	auto pParent = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr; 
	return itemModelController_->insertRows( collectionModel,
		row,
		count,
		pParent );
}


bool QtCollectionModel::removeRows( int row,
	int count,
	const QModelIndex & parent )
{
	auto & collectionModel = this->source();

	// Item already uses the Command System or the Command System is not available
	const auto useController =
		(!collectionModel.hasController() && (itemModelController_ != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.removeRows( row, count );
	}

	// Queue with Command System, to register undo/redo data
	auto pParent = parent.isValid() ?
		reinterpret_cast< AbstractItem * >( parent.internalId() ) : nullptr; 
	return itemModelController_->removeRows( collectionModel,
		row,
		count,
		pParent );
}


QObject * QtCollectionModel::item( const QVariant & key ) const
{
	const auto & collectionModel = this->source();
	const auto variantKey = QtHelpers::toVariant( key );

	const auto pItem = collectionModel.find(variantKey);
	const auto value = pItem->getData(0 /* row */, 0 /* column */, ValueRole::roleId_);

	auto qValue = QtHelpers::toQVariant(value, const_cast<QtCollectionModel*>(this));
	return qValue.value<QObject*>();
}

bool QtCollectionModel::insertItem(const QVariant& key)
{
	auto& collectionModel = this->source();
	const auto variantKey = QtHelpers::toVariant(key);

	// Item already uses the Command System or the Command System is not available
	const auto useController =
	(!collectionModel.hasController() && (itemModelController_ != nullptr));

	if (!useController)
	{
		// Set property directly
		return collectionModel.insertItem(variantKey);
	}

	// Queue with Command System, to register undo/redo data
	const int count = 1;
	const AbstractItem* pParent = nullptr;
	return itemModelController_->insertItem(collectionModel, variantKey);
}

bool QtCollectionModel::insertItemValue(const QVariant& key, const QVariant& value)
{
	auto & collectionModel = this->source();
	const auto variantKey = QtHelpers::toVariant( key );
	const auto variantValue = QtHelpers::toVariant(value);

	// Item already uses the Command System or the Command System is not available
	const auto useController =
	(!collectionModel.hasController() && (itemModelController_ != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.insertItem(variantKey, variantValue);
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->insertItem(collectionModel,
	                                        variantKey,
	                                        variantValue);
}


bool QtCollectionModel::removeItem( const QVariant & key )
{
	auto & collectionModel = this->source();
	const auto variantKey = QtHelpers::toVariant( key );

	// Item already uses the Command System or the Command System is not available
	const auto useController =
	(!collectionModel.hasController() && (itemModelController_ != nullptr));
	if (!useController)
	{
		// Set property directly
		return collectionModel.removeItem( variantKey );
	}

	// Queue with Command System, to register undo/redo data
	return itemModelController_->removeItem(collectionModel, variantKey);
}

bool QtCollectionModel::isMapping() const
{
	auto & collectionModel = this->source();
	return collectionModel.isMapping();
}
} // end namespace wgt
