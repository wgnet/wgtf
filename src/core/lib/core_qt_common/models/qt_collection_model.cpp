#include "qt_collection_model.hpp"

#include "helpers/qt_helpers.hpp"
#include "core_qt_script/qt_script_object.hpp"


namespace wgt
{
ITEMROLE( value )
ITEMROLE( key )
ITEMROLE( valueType )
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
	auto & collection = collectionModel.getSource();

	// Insert/remove by row disabled for mapping types
	if (collection.isMapping())
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert index directly to key
	Variant key( row );
	bool success = true;
	for (int i = 0; i < count; ++i)
	{
		// Repeatedly inserting items at the same key
		// should add count items above the first
		const auto insertItr = collection.insert( key );
		success &= (insertItr != collection.end());
	}
	return success;
}


bool QtCollectionModel::removeRows( int row,
	int count,
	const QModelIndex & parent )
{
	auto & collectionModel = this->source();
	auto & collection = collectionModel.getSource();

	// Insert/remove by row disabled for mapping types
	if (collection.isMapping())
	{
		return false;
	}

	// Trying to remove too many rows
	if ((row + count) >= this->rowCount( parent ))
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert index directly to key
	Variant key( row );
	bool success = true;
	for (int i = 0; i < count; ++i)
	{
		// Repeatedly removing items at the same key
		// should remove count items after the first
		const auto erasedCount = collection.eraseKey( key );
		success &= (erasedCount > 0);
	}
	return success;
}


QObject * QtCollectionModel::item( const QVariant & key ) const
{
	const auto & collectionModel = this->source();
	const auto & collection = collectionModel.getSource();

	// Check key types match
	const auto variantKey = QtHelpers::toVariant( key );

	int row = 0;
	if (collection.isMapping())
	{
		// Key type may not be an index.
		// E.g. map[ "string key" ]
		// Iterate to convert key to an index.
		auto itr = collection.cbegin();
		for (; itr != collection.cend(); ++itr, ++row)
		{
			if (itr.key() == variantKey)
			{
				break;
			}
		}
		if (itr == collection.cend())
		{
			return nullptr;
		}
	}
	else
	{
		// Since this is an index-able collection
		// Convert key directly to index
		const auto isRow = variantKey.tryCast< int >( row );
		if (!isRow)
		{
			return nullptr;
		}
	}

	assert( QtItemModel::hasIndex( row, 0 ) );
	auto value = collection.find(variantKey).value();
	auto qValue = QtHelpers::toQVariant(value, const_cast<QtCollectionModel*>(this));
	return qValue.value<QObject*>();
}


bool QtCollectionModel::insertItem( const QVariant & key )
{
	auto & collectionModel = this->source();
	auto & collection = collectionModel.getSource();

	const auto variantKey = QtHelpers::toVariant( key );
	const auto insertItr = collection.insert( variantKey );
	return (insertItr != collection.end());
}


bool QtCollectionModel::removeItem( const QVariant & key )
{
	auto & collectionModel = this->source();
	auto & collection = collectionModel.getSource();

	const auto variantKey = QtHelpers::toVariant( key );
	const auto erasedCount = collection.eraseKey( variantKey );
	return (erasedCount > 0);
}

bool QtCollectionModel::isMapping() const
{
	auto & collectionModel = this->source();
	auto & collection = collectionModel.getSource();
	
	return collection.isMapping();
}
} // end namespace wgt
