#pragma once
#ifndef _REFLECTED_TREE_ITEM_NEW_HPP
#define _REFLECTED_TREE_ITEM_NEW_HPP

#include "core_data_model/abstract_item.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"

namespace wgt
{
class ReflectedTreeModelNew;

class IBaseProperty;
typedef std::shared_ptr<IBaseProperty> IBasePropertyPtr;

class IClassDefinition;
class ObjectHandle;
class PropertyAccessor;

/** Base class for adding a reflected item to a tree. */
class ReflectedTreeItemNew : public AbstractTreeItem, public Depends<IDefinitionManager>
{
public:
	ReflectedTreeItemNew(const ReflectedTreeModelNew& model);
	ReflectedTreeItemNew(ReflectedTreeItemNew* parent, size_t index, const char* path);
	ReflectedTreeItemNew(ReflectedTreeItemNew* parent, size_t index, const std::string& path);
	virtual ~ReflectedTreeItemNew();

	/** Finds the ObjectHandle of the most distant ancestor with an ObjectHandle.
	Returns null if none found. */
	virtual const ObjectHandle& getRootObject() const = 0;

	/** Finds the ObjectHandle of the closest ancestor with an ObjectHandle.
	Returns null if none found. */
	virtual const ObjectHandle& getObject() const = 0;

	/** Finds the Definition of the closest ancestor with an ObjectHandle.
	Returns null if none found. */
	virtual const IClassDefinition* getDefinition() const;

	/** Returns the child at the index position.
	The list of children include indirect children as well.
	Indirect children are children of in place children.
	Returns null if none found. */
	virtual ReflectedTreeItemNew* getChild(size_t index) const = 0;

	virtual int rowCount() const = 0;

	/** Checks whether this is an in place item.
	@see ReflectedTreeModelNew for details about in place items. */
	virtual bool isInPlace() const;

	/** Signal before a value is changed in this item.
	@param accessor The controller that is changing the value.
	@param value The new value.
	@return True if successful, false if the value should be reverted. */
	virtual bool preSetValue(const PropertyAccessor& accessor, const Variant& value) = 0;

	/** Signal after a value is changed in this item.
	@param accessor The controller that is changing the value.
	@param value The new value.
	@return True if successful, false if the value should be reverted. */
	virtual bool postSetValue(const PropertyAccessor& accessor, const Variant& value) = 0;

	/** Signal before child items are added to this item.
	@param accessor The controller that is adding the items.
	@param index The index of the first new item.
	@param count The amount of new items.
	@return True if successful, false if the operation should be reverted. */
	virtual bool preInsert(const PropertyAccessor& accessor, size_t index, size_t count) = 0;

	/** Signal after child items are added to this item.
	@param accessor The controller that is adding the items.
	@param index The index of the first new item.
	@param count The amount of new items.
	@return True if successful, false if the operation should be reverted. */
	virtual bool postInserted(const PropertyAccessor& accessor, size_t index, size_t count) = 0;

	/** Signal before child items are removed from this item.
	@param accessor The controller that is removing the items.
	@param index The index of the first removed item.
	@param count The amount of items removed.
	@return True if successful, false if the operation should be reverted. */
	virtual bool preErase(const PropertyAccessor& accessor, size_t index, size_t count) = 0;

	/** Signal after child items are removed from this item.
	@param accessor The controller that is removing the items.
	@param index The index of the first removed item.
	@param count The amount of items removed.
	@return True if successful, false if the operation should be reverted. */
	virtual bool postErased(const PropertyAccessor& accessor, size_t index, size_t count) = 0;

	virtual bool hasController() const override;

	/** Returns a unique identifier for this item. */
	uint64_t getId() const;

	/** Returns the path to access this item.
	@note This includes all (non in place) ancestors up to this item. */
	const std::string& getPath() const;

	/** Gets the model that contains this item. */
	const ReflectedTreeModelNew* getModel() const;

	/** Gets the closest parent item that is not an in place item. */
	const ReflectedTreeItemNew* getParent() const;

	/** Gets the closest parent item that is not an in place item. */
	ReflectedTreeItemNew* getParent();

	/** Returns the index of this item as a distance from its parent.
	This is used internally to keep track of the index before in place additions.*/
	size_t getIndex() const;

	/** Changes the index of this item.
	This is used internally to keep track of the index before in place additions. */
	void setIndex(size_t index);

protected:
	ReflectedTreeItemNew* parent_;
	uint64_t id_;
	std::string path_;
	size_t index_;

	typedef std::function<bool(const IBasePropertyPtr&, const std::string&)> PropertyCallback;
	bool enumerateVisibleProperties(const PropertyCallback& callback) const;

private:
	/** Iterate through properties, ignoring items with MetaHidden. */
	static bool enumerateVisibleProperties(ObjectHandle object, const IDefinitionManager& definitionManager,
	                                       const std::string& inPlacePath, const PropertyCallback& callback);

	const ReflectedTreeModelNew* model_;
};
} // end namespace wgt
#endif //_REFLECTED_TREE_ITEM_NEW_HPP
