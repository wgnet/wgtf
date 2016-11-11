#ifndef REFLECTED_OBJECT_ITEM_HPP
#define REFLECTED_OBJECT_ITEM_HPP

#include "reflected_item.hpp"
#include "core_reflection/object_handle.hpp"

#include <set>

namespace wgt
{
/**
 *	Create an item in a ReflectedTreeModel from an ObjectHandle.
 *	Can be a root or child item.
 */
class ReflectedObjectItem : public ReflectedItem
{
public:
	/**
	 *	Construct a tree data model by reflecting over the given object.
	 *	@param object the root of the tree.
	 *		@warning the parent *must* correspond to the object.
	 *	@param parent the parent of this property.
	 */
	ReflectedObjectItem(const ObjectHandle& object, ReflectedItem* parent = nullptr);
	virtual ~ReflectedObjectItem()
	{
	}

	// ReflectedItem
	const ObjectHandle& getRootObject() const override
	{
		return parent_ ? parent_->getRootObject() : object_;
	}
	const ObjectHandle& getObject() const override
	{
		return object_;
	}
	const IClassDefinition* getDefinition() const override;

	// IItem
	const char* getDisplayText(int column) const override;
	Variant getData(int column, ItemRole::Id roleId) const override;
	bool setData(int column, ItemRole::Id roleId, const Variant& data) override;

	// GenericTreeItem
	GenericTreeItem* getChild(size_t index) const override;

	bool empty() const override;
	size_t size() const override;

	bool preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
	bool postSetValue(const PropertyAccessor& accessor, const Variant& value) override;

private:
	typedef std::set<const wchar_t*, bool (*)(const wchar_t*, const wchar_t*)> Groups;
	typedef std::function<bool(ReflectedItem&)> ReflectedItemCallback;

	void EnumerateChildren(const ReflectedItemCallback& callback) const;
	Groups& GetGroups() const;

	ObjectHandle object_;
	mutable std::string displayName_;
	mutable std::vector<std::unique_ptr<ReflectedItem>> children_;
	mutable Groups groups_;
};
} // end namespace wgt
#endif // REFLECTED_OBJECT_ITEM_HPP
