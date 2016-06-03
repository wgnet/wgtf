#ifndef COLLECTION_MODEL_HPP
#define COLLECTION_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "core_variant/collection.hpp"
#include <memory>
#include <vector>

namespace wgt
{
/**
 *	Data model for displaying a Collection as a list.
 */
class CollectionModel
	: public AbstractListModel
{
public:
	CollectionModel();
	virtual ~CollectionModel();

	void setSource(Collection & collection);
	const Collection & getSource() const;
	Collection & getSource();

	// AbstractListModel
	virtual AbstractItem * item(int index) const override;
	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

	virtual Connection connectPreItemDataChanged(AbstractListModel::DataCallback callback) override;
	virtual Connection connectPostItemDataChanged(AbstractListModel::DataCallback callback) override;

	virtual Connection connectPreRowsInserted(AbstractListModel::RangeCallback callback) override;
	virtual Connection connectPostRowsInserted(AbstractListModel::RangeCallback callback) override;

	virtual Connection connectPreRowsRemoved(AbstractListModel::RangeCallback callback) override;
	virtual Connection connectPostRowsRemoved(AbstractListModel::RangeCallback callback) override;

protected:
	Collection collection_;
	mutable std::vector< std::unique_ptr< AbstractItem > > items_;

private:

	Signal<AbstractListModel::DataSignature> preItemDataChanged_;
	Signal<AbstractListModel::DataSignature> postItemDataChanged_;

	Signal<AbstractListModel::RangeSignature> preRowsInserted_; 
	Signal<AbstractListModel::RangeSignature> postRowsInserted_;

	Signal<AbstractListModel::RangeSignature> preRowsRemoved_;
	Signal<AbstractListModel::RangeSignature> postRowsRemoved_; 


	Connection connectPreChange_;
	Connection connectPostChanged_;

	Connection connectPreInsert_;
	Connection connectPostInserted_;

	Connection connectPreErase_;
	Connection connectPostErase_;
};
} // end namespace wgt
#endif // COLLECTION_LIST_MODEL_HPP
