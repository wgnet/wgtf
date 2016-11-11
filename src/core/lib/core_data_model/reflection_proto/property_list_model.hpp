#ifndef PROTO_PROPERTY_LIST_MODEL_HPP
#define PROTO_PROPERTY_LIST_MODEL_HPP

#include "reflected_tree_model.hpp"

namespace wgt
{
namespace proto
{
class PropertyListModel : public ReflectedTreeModel
{
public:
	PropertyListModel(IComponentContext& context, const ObjectHandle& object = nullptr);
	virtual ~PropertyListModel();

protected:
	virtual std::unique_ptr<Children> getChildren(const AbstractItem* item) override;
	virtual ItemIndex childHint(const AbstractItem* item) override;

	void collectProperties(const ReflectedPropertyItem* item, std::vector<const ReflectedPropertyItem*>& o_Properties);

	enum FilterResult
	{
		INCLUDE,
		INCLUDE_CHILDREN,
		IGNORE
	};
	virtual FilterResult filterProperty(const ReflectedPropertyItem* item) const;
};
}
}

#endif // PROTO_PROPERTY_LIST_MODEL_HPP