#pragma once
#ifndef _REFLECTED_TREE_MODEL_NEW_HPP
#define _REFLECTED_TREE_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{
class IComponentContext;
class ObjectHandle;

/** Construct a tree data model by reflecting over the given objects.
Tree may have zero or multiple "root" objects.
This tree uses the notion of in place items to hide certain group levels from view.
An item's children include direct (not in place) children as well as
(not in place) children of the in place children.
For example:
> group1
>> group2 (in place)
>>> item1
>>> item2
>> item4
Is transformed into:
> group1
>> item1
>> item2
>> item4
*/
class ReflectedTreeModelNew : public AbstractTreeModel
{
public:
	ReflectedTreeModelNew(const ObjectHandle& object);
	virtual ~ReflectedTreeModelNew();

	/** Gets the item at an index position.
	@note The children for the index's parent includes children of in place items.
	For example, the index (2, group1) will return item4 for this tree:
	> group1
	>> group2 (in place)
	>>> item1
	>>> item2
	>>> item3 (hidden)
	>> item4
	@param item The index used to look for the item.
	@return The item found. */
	virtual AbstractItem* item(const ItemIndex& index) const override;

	/** Returns the index to locate an item in the tree.
	@note The item's parent might not be its direct parent, as in place parents are skipped over.
	For example, the following will yield the index (1, group1) for item2:
	> group1
	>> group2 (in place)
	>>> item1
	>>> item2
	>>> item3 (hidden)
	>> item4
	@param item The item to locate in the tree.
	@return The index to locate the item. */
	virtual ItemIndex index(const AbstractItem* item) const override;

	/** Gets the number of leaf nodes.
	@note This excludes in place items, but includes descendants of in place items.
	@note This excludes hidden items.
	For example, the following results in a count of 3 (at group1):
	> group1
	>> group2 (in place)
	>>> item1
	>>> item2
	>>> item3 (hidden)
	>> item4
	@param item The parent item under which to look for descendants.
	@return The number of items found. */
	virtual int rowCount(const AbstractItem* item) const override;

	virtual int columnCount() const override;

	/** Determines whether an item has any child items under it.
	@note If a child is in place, that child is excluded, but its children are considered.
	@note Hidden children are excluded.
	For example, the following results in true (at group1):
	> group1
	>> group2 (in place)
	>>> item1
	@param item The parent item under which to look for children.
	@return True if at least one child found. */
	virtual bool hasChildren(const AbstractItem* item) const override;

	void iterateRoles(const std::function<void(const char*)>& iterFunc) const override;
	virtual std::vector<std::string> roles() const override;

	virtual bool hasController() const override;

	virtual Connection connectPreItemDataChanged(AbstractTreeModel::DataCallback callback) override;
	virtual Connection connectPostItemDataChanged(AbstractTreeModel::DataCallback callback) override;
	virtual Connection connectPreRowsInserted(AbstractTreeModel::RangeCallback callback) override;
	virtual Connection connectPostRowsInserted(AbstractTreeModel::RangeCallback callback) override;
	virtual Connection connectPreRowsRemoved(AbstractTreeModel::RangeCallback callback) override;
	virtual Connection connectPostRowsRemoved(AbstractTreeModel::RangeCallback callback) override;

private:
	friend class ReflectedPropertyItemNew;

	Signal<AbstractTreeModel::DataSignature> preItemDataChanged_;
	Signal<AbstractTreeModel::DataSignature> postItemDataChanged_;

	Signal<AbstractTreeModel::RangeSignature> preRowsInserted_;
	Signal<AbstractTreeModel::RangeSignature> postRowsInserted_;

	Signal<AbstractTreeModel::RangeSignature> preRowsRemoved_;
	Signal<AbstractTreeModel::RangeSignature> postRowsRemoved_;

	class Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // _REFLECTED_TREE_MODEL_NEW_HPP
