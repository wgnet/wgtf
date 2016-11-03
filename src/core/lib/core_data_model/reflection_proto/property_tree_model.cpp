#include "property_tree_model.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"

namespace wgt
{
namespace proto
{
class ReflectedGroupItem : public AbstractTreeItem
{
public:
	ReflectedGroupItem(const ObjectHandle& object, const MetaGroupObj& groupObj)
	    : object_(object)
	    , groupName_(groupObj.getGroupName())
	    , hash_(groupObj.getGroupNameHash())
	{
	}

	virtual ~ReflectedGroupItem()
	{
	}

	const ObjectHandle& getObject() const
	{
		return object_;
	}

	uint64_t getHash() const
	{
		return hash_;
	}

	// AbstractItem
	virtual Variant getData(int column, ItemRole::Id roleId) const override
	{
		if (roleId == ItemRole::displayId)
		{
			return groupName_.c_str();
		}

		return Variant();
	}

	virtual bool hasController() const override
	{
		return true;
	}

private:
	ObjectHandle object_;
	std::wstring groupName_;
	uint64_t hash_;
};

PropertyTreeModel::PropertyTreeModel(IComponentContext& context, const ObjectHandle& object)
    : ReflectedTreeModel(context, object)
{
}

PropertyTreeModel::~PropertyTreeModel()
{
}

std::unique_ptr<ReflectedTreeModel::Children> PropertyTreeModel::getChildren(const AbstractItem* item)
{
	auto children = new Children();

	auto groupItem = dynamic_cast<const ReflectedGroupItem*>(item);
	if (groupItem)
	{
		std::vector<const ReflectedPropertyItem*> properties;
		auto index = this->index(groupItem);
		collectProperties(static_cast<const ReflectedPropertyItem*>(index.parent_), properties);
		for (auto& property : properties)
		{
			auto& object = property->getObject();
			auto& path = property->getPath();

			auto definition = object.getDefinition(*getDefinitionManager());
			auto propertyAccessor = definition->bindProperty(path.c_str(), object);

			auto hiddenObj = findFirstMetaData<MetaHiddenObj>(propertyAccessor, *getDefinitionManager());
			if (hiddenObj != nullptr)
			{
				continue;
			}

			auto groupObj = findFirstMetaData<MetaGroupObj>(propertyAccessor, *getDefinitionManager());
			if (groupObj == nullptr || groupObj->getGroupNameHash() != groupItem->getHash())
			{
				continue;
			}

			children->push_back(property);
		}
	}
	else
	{
		auto propertyItem = static_cast<const ReflectedPropertyItem*>(item);

		auto& groups = enumerateGroups(propertyItem);
		for (auto& group : groups)
		{
			children->push_back(group.get());
		}

		std::vector<const ReflectedPropertyItem*> properties;
		collectProperties(propertyItem, properties);
		for (auto& property : properties)
		{
			auto& object = property->getObject();
			auto& path = property->getPath();

			auto definition = object.getDefinition(*getDefinitionManager());
			auto propertyAccessor = definition->bindProperty(path.c_str(), object);

			auto hiddenObj = findFirstMetaData<MetaHiddenObj>(propertyAccessor, *getDefinitionManager());
			if (hiddenObj != nullptr)
			{
				continue;
			}

			auto groupObj = findFirstMetaData<MetaGroupObj>(propertyAccessor, *getDefinitionManager());
			if (groupObj != nullptr)
			{
				continue;
			}

			children->push_back(property);
		}
	}

	return std::unique_ptr<Children>(children);
}

void PropertyTreeModel::removeChildren(const AbstractItem* item, std::unique_ptr<Children>& children)
{
	auto groupItem = dynamic_cast<const ReflectedGroupItem*>(item);
	if (groupItem)
	{
		children.reset();
		return;
	}

	auto propertyItem = static_cast<const ReflectedPropertyItem*>(item);
	clearGroups(propertyItem);
	clearProperties(propertyItem);
	children.reset();
}

const PropertyTreeModel::Groups& PropertyTreeModel::enumerateGroups(const ReflectedPropertyItem* item)
{
	auto groupsIt = groups_.find(item);
	if (groupsIt != groups_.end())
	{
		auto groups = groupsIt->second;
		assert(groups != nullptr);
		return *groups;
	}

	auto groups = new Groups();
	groups_.insert(std::make_pair(item, groups));

	auto& properties = enumerateProperties(item);
	for (auto& property : properties)
	{
		auto& object = property->getObject();
		auto& path = property->getPath();

		auto definition = object.getDefinition(*getDefinitionManager());
		auto propertyAccessor = definition->bindProperty(path.c_str(), object);

		auto groupObj = findFirstMetaData<MetaGroupObj>(propertyAccessor, *getDefinitionManager());
		if (groupObj != nullptr)
		{
			auto it = std::find_if(groups->begin(), groups->end(),
			                       [&](const std::unique_ptr<ReflectedGroupItem>& groupItem) { return groupItem->getHash() == groupObj->getGroupNameHash(); });
			if (it == groups->end())
			{
				groups->emplace_back(new ReflectedGroupItem(item != nullptr ? item->getObject() : getObject(), *groupObj));
			}
		}
	}

	return *groups;
}

void PropertyTreeModel::clearGroups(const ReflectedPropertyItem* item)
{
	auto groupsIt = groups_.find(item);
	if (groupsIt == groups_.end())
	{
		return;
	}

	auto groups = groupsIt->second;
	assert(groups != nullptr);
	groups_.erase(groupsIt);
	delete groups;
}

void PropertyTreeModel::collectProperties(const ReflectedPropertyItem* item, std::vector<const ReflectedPropertyItem*>& o_Properties)
{
	auto& properties = enumerateProperties(item);
	for (auto& property : properties)
	{
		auto& object = property->getObject();
		auto& path = property->getPath();

		auto definition = object.getDefinition(*getDefinitionManager());
		auto propertyAccessor = definition->bindProperty(path.c_str(), object);

		if (!propertyAccessor.isValid())
		{
			continue;
		}

		if (propertyAccessor.getProperty()->isMethod())
		{
			continue;
		}

		auto metaInPlaceObj = findFirstMetaData<MetaInPlaceObj>(propertyAccessor, *getDefinitionManager());
		if (metaInPlaceObj != nullptr)
		{
			collectProperties(property.get(), o_Properties);
			continue;
		}

		o_Properties.push_back(property.get());
	}
}
}
}