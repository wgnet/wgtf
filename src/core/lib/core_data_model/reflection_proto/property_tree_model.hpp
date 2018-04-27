#ifndef PROTO_PROPERTY_TREE_MODEL_HPP
#define PROTO_PROPERTY_TREE_MODEL_HPP

#include "reflected_tree_model.hpp"

namespace wgt
{
namespace proto
{
class PropertyGroupItem;
typedef std::unique_ptr<PropertyGroupItem> PropertyGroupItemPtr;

class PropertyTreeModel : public ReflectedTreeModel
{
public:
	PropertyTreeModel(const ObjectHandle& object = nullptr);
	virtual ~PropertyTreeModel();

protected:
	virtual std::unique_ptr<Children> mapChildren(const AbstractItem* item) override;
	virtual void clearChildren(const AbstractItem* item) override;

	virtual const AbstractItem* mappedItem(const ReflectedPropertyItem* item) const;

	typedef std::pair<const ReflectedPropertyItem*, uint64_t> PropertyItem;
	typedef std::vector<PropertyGroupItemPtr>                 Groups;

	Groups getGroups(const PropertyItem propertyItem);
	void clearGroups(const ReflectedPropertyItem* item);

	PropertyItem propertyItem(const AbstractItem* item) const;
	void collectProperties(const ReflectedPropertyItem* item, std::vector<PropertyItem>& o_Properties, Groups* groups = nullptr, uint64_t groupHash = 0);

	enum FilterResult
	{
		FILTER_INCLUDE,
		FILTER_INCLUDE_CHILDREN,
		FILTER_IGNORE
	};
	virtual FilterResult filterProperty(const ReflectedPropertyItem* item) const;

private:
	std::unordered_map<const ReflectedPropertyItem*, Groups> groups_;
};
}
}

#endif // PROTO_PROPERTY_TREE_MODEL_HPP