#pragma once
#ifndef _REFLECTED_OBJECT_ITEM_NEW_HPP
#define _REFLECTED_OBJECT_ITEM_NEW_HPP

#include "reflected_tree_item_new.hpp"

#include <memory>

namespace wgt
{
/** Represents an ObjectHandle as an item in a ReflectedTreeModel.
This can result in either a root or child item. */
class ReflectedObjectItemNew : public ReflectedTreeItemNew
{
public:
	/** Creates a reflected tree item to represent a reflected object.
	The created item may contain child items as well.
	This constructor is used for top level items.
	@warning the parent *must* correspond to the object.
	@param object The object relating to this item.
	@param model the parent model of this property. */
	ReflectedObjectItemNew(const ObjectHandle& object, const ReflectedTreeModelNew& model);

	/** Creates a reflected tree item to represent a reflected object.
	The created item may contain child items as well.
	This constructor is used for non top level items.
	@warning the parent *must* correspond to the object.
	@param object The object relating to this item.
	@param parent the parent item of this property. */
	ReflectedObjectItemNew(const ObjectHandle& object, ReflectedTreeItemNew* parent, size_t index);

	virtual ~ReflectedObjectItemNew();

	// AbstractItem
	virtual Variant getData(int column, ItemRole::Id roleId) const override;
	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data) override;

	// ReflectedTreeItemNew
	virtual const ObjectHandle& getRootObject() const override;
	virtual const ObjectHandle& getObject() const override;
	virtual const IClassDefinition* getDefinition() const override;

	virtual ReflectedTreeItemNew* getChild(size_t index) const override;
	virtual int rowCount() const override;

	virtual bool isInPlace() const override;

	virtual bool preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	virtual bool postSetValue(const PropertyAccessor& accessor, const Variant& value) override;

	virtual bool preInsert(const PropertyAccessor& accessor, size_t index, size_t count) override;
	virtual bool postInserted(const PropertyAccessor& accessor, size_t index, size_t count) override;

	virtual bool preErase(const PropertyAccessor& accessor, size_t index, size_t count) override;
	virtual bool postErased(const PropertyAccessor& accessor, size_t index, size_t count) override;

private:
	class Implementation;
	std::unique_ptr<Implementation> impl_;

	typedef std::function<bool(ReflectedTreeItemNew&)> ReflectedItemCallback;
	void enumerateChildren(const ReflectedItemCallback& callback) const;
};
} // end namespace wgt
#endif // _REFLECTED_OBJECT_ITEM_NEW_HPP
