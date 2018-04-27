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
	PropertyListModel(const ObjectHandle& object = nullptr);
	virtual ~PropertyListModel();

protected:
	virtual std::unique_ptr<Children> mapChildren(const AbstractItem* item) override;
	virtual ItemIndex childHint(const ReflectedPropertyItem* item) const override;

	void collectProperties(const ReflectedPropertyItem* item, std::vector<const ReflectedPropertyItem*>& o_Properties);

	enum FilterResult
	{
		FILTER_INCLUDE,
		FILTER_INCLUDE_CHILDREN,
		FILTER_IGNORE
	};
	virtual FilterResult filterProperty(const ReflectedPropertyItem* item) const;
};
}
}

#endif // PROTO_PROPERTY_LIST_MODEL_HPP