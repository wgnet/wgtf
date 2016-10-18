#pragma once
#ifndef _QT_COLLECTION_MODEL_HPP
#define _QT_COLLECTION_MODEL_HPP

#include "qt_item_model.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
class CollectionModel;
class IComponentContext;


/** Provides QML interface to use a CollectionModel as a list model.
Wraps a CollectionModel in a Qt compatible model.
Can access items by key as well as by row index.
@defgroup QML QML accessible. */
class QtCollectionModel : public QtListModel
{
	Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

public:
	QtCollectionModel( IComponentContext & context,
		std::unique_ptr< CollectionModel > && source );

	QHash< int, QByteArray > roleNames() const override;	

	/** Get the wrapped CollectionModel. */
	const CollectionModel & source() const;

	/** Get the wrapped CollectionModel. */
	CollectionModel & source();

	/** Insert new rows into the list.
	@note Cannot be used with mapping types where row cannot be converted to a key.
	@note parent should never be specified.
	@param row The index for the first new item.
	@param count The amount of new items.
	@param parent This should always be the default value.
	@return True if successful. */
	virtual bool insertRows( int row,
		int count,
		const QModelIndex & parent = QModelIndex() ) override;

	/** Removes rows from the list.
	@note Cannot be used with mapping types where row cannot be converted to a key.
	@note parent should never be specified.
	@param row The index for the first item to be removed.
	@param count The amount of items to be removed.
	@param parent This should always be the default value.
	@return True if successful. */
	virtual bool removeRows( int row,
		int count,
		const QModelIndex & parent = QModelIndex() ) override;

	/** Finds an item using either key or index.
	@ingroup QML
	@param key This is used to find the item, and can either be an index or map key.
	@return The found item or nullptr.
	@note Returns nullptr when the key cannot be found in the collection, and when
	the key type does not match the source collection's key type. */
	Q_INVOKABLE QObject * item( const QVariant & key ) const;

	/** Insert a new item into the list using a key.
	@ingroup QML
	@param key The identifier for the new item, relating to the key in the collection.
	@return True if successful. */
	Q_INVOKABLE bool insertItem(const QVariant& key);

	/** Insert a new item into the list using a key.
	@ingroup QML
	@param key The identifier for the new item, relating to the key in the collection.
	@param value with which to initialize the item.
	@return True if successful. */
	Q_INVOKABLE bool insertItemValue(const QVariant& key,
	                                 const QVariant& value);

	/** Removes all items from the list matching a key.
	@ingroup QML
	@param key The identifier to find the items, relating to the key in the collection.
	@return True if successful. */
	Q_INVOKABLE bool removeItem( const QVariant & key );

	/** Checks whether the wrapped collection can be accessed using a key.
	@ingroup QML */
	Q_INVOKABLE bool isMapping() const;

private:
	std::unique_ptr<CollectionModel> model_;
};
} // end namespace wgt
#endif // _QT_COLLECTION_MODEL_HPP
