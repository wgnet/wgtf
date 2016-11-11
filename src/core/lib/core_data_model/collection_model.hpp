#ifndef COLLECTION_MODEL_HPP
#define COLLECTION_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "core_variant/collection.hpp"
#include <memory>
#include <vector>

namespace wgt
{
/** Data model for wrapping a Collection in a list model. */
class CollectionModel : public AbstractListModel
{
public:
	CollectionModel();
	virtual ~CollectionModel();

	/** Changes the model to source data from a new collection.
	@param collection The new collection to link to. */
	void setSource(Collection& collection);

	/** Changes the model to source data from a new collection.
	@param collection The new collection to link to. */
	void setSource(const Collection& collection);

	/** Returns the linked collection this model sources data from. */
	const Collection& getSource() const;

	/** Returns the linked collection this model sources data from. */
	Collection& getSource();

	// AbstractListModel
	virtual AbstractItem* item(int index) const override;
	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

	virtual bool insertRows(int row, int count) override;
	virtual bool removeRows(int row, int count) override;

	virtual std::vector<std::string> roles() const override;

	// Collection interface using keys
	/**
	 *	@param key key can be used with linear collections or maps.
	 *	If the key is an int, then it behaves the same as an index.
	 *	@return item at key or nullptr.
	 *	@note item pointers may be deleted by removeRows(), removeColumns(),
	 *	or removeItem().
	 */
	AbstractItem* find(const Variant& key);
	const AbstractItem* find(const Variant& key) const;
	/**
	 *	Inserts an empty/blank value.
	 *	@param key key can be used with linear collections or maps.
	 *	If the key is an int, then it behaves the same as an index.
	 *	@return success.
	 */
	bool insertItem(const Variant& key);
	/**
	 *	Inserts a value.
	 *	@param key key can be used with linear collections or maps.
	 *	If the key is an int, then it behaves the same as an index.
	 *	If the (key > rowCount) on a linear collection, then fail.
	 *	@param value with which to initialize the new item.
	 *	@return success.
	 */
	bool insertItem(const Variant& key, const Variant& value);
	/**
	 *	Removes an entry.
	 *	@param key key can be used with linear collections or maps.
	 *	If the key is an int, then it behaves the same as an index.
	 *	@return success.
	 */
	bool removeItem(const Variant& key);
	/**
	 *	@return if the underlying type is a map that only accepts keys.
	 *	Index-based functions, item/insertRows/removeRows(), will not work.
	 */
	bool isMapping() const;

	virtual bool hasController() const override;

	bool readonly() const;

	// AbstractListModel connections
	virtual Connection connectPreItemDataChanged(AbstractListModel::DataCallback callback) override;
	virtual Connection connectPostItemDataChanged(AbstractListModel::DataCallback callback) override;

	virtual Connection connectPreRowsInserted(AbstractListModel::RangeCallback callback) override;
	virtual Connection connectPostRowsInserted(AbstractListModel::RangeCallback callback) override;

	virtual Connection connectPreRowsRemoved(AbstractListModel::RangeCallback callback) override;
	virtual Connection connectPostRowsRemoved(AbstractListModel::RangeCallback callback) override;

	virtual Connection connectPreModelReset(AbstractListModel::VoidCallback callback) override;
	virtual Connection connectPostModelReset(AbstractListModel::VoidCallback callback) override;

protected:
	/** The linked collection that contains the actual data. */
	Collection collection_;

	bool readonly_;

	/** The list items referencing the elements in collection_. */
	mutable std::vector<std::unique_ptr<AbstractItem>> items_;

	Signal<AbstractListModel::DataSignature> preItemDataChanged_;
	Signal<AbstractListModel::DataSignature> postItemDataChanged_;

	Signal<AbstractListModel::RangeSignature> preRowsInserted_;
	Signal<AbstractListModel::RangeSignature> postRowsInserted_;

	Signal<AbstractListModel::RangeSignature> preRowsRemoved_;
	Signal<AbstractListModel::RangeSignature> postRowsRemoved_;

	Signal<AbstractListModel::VoidSignature> preModelReset_;
	Signal<AbstractListModel::VoidSignature> postModelReset_;

	std::vector<Connection> connections_;

	private:
		void setSourceInternal(const Collection& collection);
};
} // end namespace wgt
#endif // COLLECTION_LIST_MODEL_HPP
