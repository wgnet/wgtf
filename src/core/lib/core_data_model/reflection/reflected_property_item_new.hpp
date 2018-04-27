#pragma once
#ifndef _REFLECTED_PROPERTY_ITEM_NEW_HPP
#define _REFLECTED_PROPERTY_ITEM_NEW_HPP

#include "reflected_tree_item_new.hpp"

#include <memory>

namespace wgt
{
/** Represents a reflected property in a ReflectedTreeModelNew.
Must be a child item.
Create an item in a ReflectedTreeModel from an IBaseProperty on an ObjectHandle
via a parent+string or via a parent+child property pair.
E.g. strings "parent.child" or "parent.child[0]" or pair (*parent, *child) */
class ReflectedPropertyItemNew : public ReflectedTreeItemNew
{
public:
	/** Creates a reflected tree item to represent a reflected property.
	@note parent must not be null.
	@param property the property to use for the data for this item.
	@param parent the parent of this property. Cannot be null.
	@warning the parent *must* correspond to the property. */
	ReflectedPropertyItemNew(const IBasePropertyPtr& property, ReflectedTreeItemNew* parent, size_t index,
	                         const std::string& inPlacePath);

	/** Creates a reflected tree item to represent a reflected property.
	@note parent must not be null.
	@param propertyName the name of the property on the parent.
	@param displayName the name used for display in the UI.
	@param parent the parent of this property. Cannot be null. */
	ReflectedPropertyItemNew(const std::string& propertyName, std::string displayName, ReflectedTreeItemNew* parent,
	                         size_t index);
	virtual ~ReflectedPropertyItemNew();

	// AbstractItem
	virtual Variant getData(int column, ItemRole::Id roleId) const override;
	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data) override;

	// ReflectedTreeItemNew
	virtual const ObjectHandle& getRootObject() const override;
	virtual const ObjectHandle& getObject() const override;

	virtual ReflectedTreeItemNew* getChild(size_t index) const override;
	virtual int rowCount() const override;

	virtual bool preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	virtual bool postSetValue(const PropertyAccessor& accessor, const Variant& value) override;

	virtual bool preInsert(const PropertyAccessor& accessor, size_t index, size_t count) override;
	virtual bool postInserted(const PropertyAccessor& accessor, size_t index, size_t count) override;

	virtual bool preErase(const PropertyAccessor& accessor, size_t index, size_t count) override;
	virtual bool postErased(const PropertyAccessor& accessor, size_t index, size_t count) override;

private:
	class Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // _REFLECTED_PROPERTY_ITEM_NEW_HPP
