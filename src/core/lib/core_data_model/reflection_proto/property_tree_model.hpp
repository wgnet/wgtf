#ifndef PROTO_PROPERTY_TREE_MODEL_HPP
#define PROTO_PROPERTY_TREE_MODEL_HPP

#include "reflected_tree_model.hpp"

namespace wgt
{
namespace proto
{
class ReflectedGroupItem;

class PropertyTreeModel : public ReflectedTreeModel
{
public:
	PropertyTreeModel(IComponentContext& context, const ObjectHandle& object = nullptr);
	virtual ~PropertyTreeModel();

protected:
	virtual std::unique_ptr<Children> getChildren(const AbstractItem* item) override;
	virtual void clearChildren(const AbstractItem* item) override;

	typedef std::vector<std::unique_ptr<ReflectedGroupItem>> Groups;
	const Groups& getGroups(const ReflectedPropertyItem* item);
	void clearGroups(const ReflectedPropertyItem* item);

	void collectProperties(const ReflectedPropertyItem* item, std::vector<const ReflectedPropertyItem*>& o_Properties);

	enum FilterResult
	{
		INCLUDE,
		INCLUDE_CHILDREN,
		IGNORE
	};
	virtual FilterResult filterProperty(const ReflectedPropertyItem* item) const;

private:
	std::map<const ReflectedPropertyItem*, Groups*> groups_;
};
}
}

#endif // PROTO_PROPERTY_TREE_MODEL_HPP