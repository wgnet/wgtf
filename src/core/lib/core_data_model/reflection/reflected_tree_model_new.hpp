#pragma once
#ifndef _REFLECTED_TREE_MODEL_NEW_HPP
#define _REFLECTED_TREE_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{
class IComponentContext;
class ObjectHandle;

/**
 *	Construct a tree data model by reflecting over the given objects.
 *	Tree may have zero or multiple "root" objects.
 */
class ReflectedTreeModelNew
	: public AbstractTreeModel
{
public:

	ReflectedTreeModelNew( IComponentContext & contextManager, const ObjectHandle & object );
	virtual ~ReflectedTreeModelNew();
	

	virtual AbstractItem * item( const ItemIndex & index ) const override;
	virtual ItemIndex index( const AbstractItem * item ) const override;

	/**
	 *	Get the number of child items, excluding hidden items, including null items
	 *	E.g.
	 *	> group1
	 *	>> group2 <- hidden
	 *	>>> item1 - count
	 *	>>> item2 - count
	 *	>>> item3 - count
	 *	>> item4 - count
	 *	getChildCount( group1 ) == 4
	 *	E.g.
	 *	> group1
	 *	>> group2 <- hidden
	 *	>> group3 <- hidden
	 *	getChildCount( group1 ) == 0
	 */
	virtual int rowCount( const AbstractItem * item ) const override;
	virtual int columnCount() const override;
	virtual bool hasChildren( const AbstractItem * item ) const override;

	virtual Connection connectPreItemDataChanged(
		AbstractTreeModel::DataCallback callback ) override;
	virtual Connection connectPostItemDataChanged(
		AbstractTreeModel::DataCallback callback ) override;
	virtual Connection connectPreRowsInserted(
		AbstractTreeModel::RangeCallback callback ) override;
	virtual Connection connectPostRowsInserted(
		AbstractTreeModel::RangeCallback callback ) override;
	virtual Connection connectPreRowsRemoved(
		AbstractTreeModel::RangeCallback callback ) override;
	virtual Connection connectPostRowsRemoved(
		AbstractTreeModel::RangeCallback callback ) override;

private:
	friend class ReflectedPropertyItemNew;

	Signal< AbstractTreeModel::DataSignature > preItemDataChanged_;
	Signal< AbstractTreeModel::DataSignature > postItemDataChanged_;

	Signal< AbstractTreeModel::RangeSignature > preRowsInserted_;
	Signal< AbstractTreeModel::RangeSignature > postRowsInserted_;

	Signal< AbstractTreeModel::RangeSignature > preRowsRemoved_;
	Signal< AbstractTreeModel::RangeSignature > postRowsRemoved_;

	class Implementation;
	std::unique_ptr< Implementation > impl_;
};
} // end namespace wgt
#endif // _REFLECTED_TREE_MODEL_NEW_HPP
