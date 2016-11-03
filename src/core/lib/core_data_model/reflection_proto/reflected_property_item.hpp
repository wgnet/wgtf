#ifndef PROTO_REFLECTED_PROPERTY_ITEM_HPP
#define PROTO_REFLECTED_PROPERTY_ITEM_HPP

#include "core_data_model/abstract_item.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
namespace proto
{
class ReflectedTreeModel;

class ReflectedPropertyItem : public AbstractTreeItem
{
public:
	ReflectedPropertyItem(const ReflectedTreeModel& model, const ObjectHandle& object, const std::string& path);
	virtual ~ReflectedPropertyItem();

	const ObjectHandle& getObject() const;
	const std::string& getPath() const;

	void setPath(const std::string& path);

	// AbstractItem
	virtual Variant getData(int column, ItemRole::Id roleId) const override;
	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data) override;

	virtual bool hasController() const override
	{
		return true;
	}

private:
	const ReflectedTreeModel& model_;
	ObjectHandle object_;
	std::string path_;
};
}
}

#endif //PROTO_REFLECTED_PROPERTY_ITEM_HPP