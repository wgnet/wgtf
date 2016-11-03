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
	PropertyListModel(IComponentContext& context, const ObjectHandle& object);
	virtual ~PropertyListModel();

protected:
	virtual std::unique_ptr<Children> getChildren(const AbstractItem* item) override;

	virtual ItemIndex childHint(const ReflectedPropertyItem* item) override;

	void enumerateAllProperties(const ReflectedPropertyItem* item, Children& children);
};
}
}

#endif //PROTO_PROPERTY_LIST_MODEL_HPP