#pragma once
#ifndef _QT_COLLECTION_MODEL_HPP
#define _QT_COLLECTION_MODEL_HPP

#include "qt_item_model.hpp"
#include "core_data_model/collection_model.hpp"


namespace wgt
{
class CollectionModel;


/**
 *	Provides QML interface to use a CollectionModel as a list model.
 *	Can access items by key as well as by row index.
 */
class QtCollectionModel : public QtListModel
{
	Q_OBJECT

public:
	QtCollectionModel( std::unique_ptr<CollectionModel>&& source );

	QHash< int, QByteArray > roleNames() const override;	

	/**
	 *	Get CollectionModel that is being adapted to be used by Qt.
	 */
	const CollectionModel & source() const;
	CollectionModel & source();

	/**
	 *	Note: cannot be used with mapping types where row cannot be converted
	 *	to a key.
	 */
	virtual bool insertRows( int row,
		int count,
		const QModelIndex & parent = QModelIndex() ) override;

	/**
	 *	Note: cannot be used with mapping types where row cannot be converted
	 *	to a key.
	 */
	virtual bool removeRows( int row,
		int count,
		const QModelIndex & parent = QModelIndex() ) override;

	/**
	 *	Get item from collection.
	 *	@param key may be index or map key.
	 *	@return found item or nullptr if:
	 *		Key type does not match the source collection's key type.
	 *		Key cannot be found in collection.
	 */
	Q_INVOKABLE QObject * item( const QVariant & key ) const;

	/**
	 *	Add a new item using the given key.
	 *	
	 *	@param key into the collection to insert the new item.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool insertItem( const QVariant & key );

	/**
	 *	Remove all items matching the given key.
	 *	
	 *	@param key erase all elements matching the given key.
	 *	@return true if at least one item was erased (may be more than one).
	 */
	Q_INVOKABLE bool removeItem( const QVariant & key );

	Q_INVOKABLE bool isMapping() const;

private:
	std::unique_ptr<CollectionModel> model_;
};
} // end namespace wgt
#endif // _QT_COLLECTION_MODEL_HPP
