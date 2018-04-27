#include "property_tree_model.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
	ITEMROLE(indexPath)
namespace proto
{

namespace
{
	ObjectHandleT<MetaGroupObj> getGroupForProperty(const PropertyAccessor& accessor, const IDefinitionManager& defMgr)
	{
		auto groupObj = findFirstMetaData<MetaGroupObj>(accessor, defMgr);
		if (groupObj == nullptr)
		{
			return groupObj;
		}
		// Ignore empty label groups, collapse down to parent group
		const auto& object = accessor.getObject();
		const auto groupName = groupObj->getGroupName(object);
		if (!groupName || *groupName == '\0')
		{
			return nullptr;
		}
		return groupObj;
	}
}
class PropertyGroupItem : public AbstractTreeItem
{
public:
	PropertyGroupItem(const MetaGroupObj& groupObj, const ObjectHandle& object, int currGroupLevel = MetaGroupObj::FULL_GROUP_LEVEL) 
		: groupObj_(groupObj) 
		, object_(object)
		, currGroupLevel_(currGroupLevel)
	{
	}

	virtual ~PropertyGroupItem()
	{
	}

	int getCurrLevelNormalized() const 
	{
		int result = currGroupLevel_;
		if (result == (groupObj_.getNumSubGroups() - 1))
			result = MetaGroupObj::FULL_GROUP_LEVEL;

		return result;
	}

	uint64_t getHash() const
	{
		int level       = getCurrLevelNormalized();
		uint64_t result = groupObj_.getGroupNameHash(object_, level);
		return result;
	}

	const MetaGroupObj& getGroupObj() const { return groupObj_; } 
	const ObjectHandle& getObject()   const { return object_; }

	// AbstractItem
	virtual Variant getData(int column, ItemRole::Id roleId) const override
	{
		if (roleId == ItemRole::displayId)
		{
			return groupObj_.getGroupName(object_, currGroupLevel_);
		}
		else if (roleId == ItemRole::indexPathId)
		{
			auto path = object_.path();
			int level = getCurrLevelNormalized();

			std::wstring groupNameW = groupObj_.getGroupName(object_, level);
			std::string  groupName(groupNameW.begin(), groupNameW.end());
			path += "{" + groupName + "}";
			return path;
		}

		return Variant();
	}

	virtual bool hasController() const override
	{
		return true;
	}

private:
	const int currGroupLevel_;
	const MetaGroupObj& groupObj_;
	ObjectHandle object_;
};

PropertyTreeModel::PropertyTreeModel(const ObjectHandle& object) : ReflectedTreeModel(object)
{
}

PropertyTreeModel::~PropertyTreeModel()
{
}

std::unique_ptr<ReflectedTreeModel::Children> PropertyTreeModel::mapChildren(const AbstractItem* item)
{
	auto children = new Children();
	std::pair<const ReflectedPropertyItem *, uint64_t> propertyItem = this->propertyItem(item);
	auto& newGroups = getGroups(propertyItem);

	const ReflectedPropertyItem *reflectedItem = propertyItem.first;
	auto& currGroupList                        = groups_[reflectedItem];
	for (auto& group : newGroups)
	{
		children->push_back(group.get());
		currGroupList.push_back(std::move(group));
	}

	std::vector<PropertyItem> properties;
	collectProperties(propertyItem.first, properties);
	for (auto& property : properties)
	{
		uint64_t containerPropertyHash = propertyItem.second;
		uint64_t propertyHash          = property.second;

		if (containerPropertyHash == propertyHash)
		{
			children->push_back(property.first);
		}
	}

	return std::unique_ptr<Children>(children);
}

void PropertyTreeModel::clearChildren(const AbstractItem* item)
{
	const ReflectedPropertyItem* propertyItem = nullptr;
	for (auto& groups : groups_)
	{
		for (auto& groupItem : groups.second)
		{
			if (groupItem.get() == item)
			{
				propertyItem = groups.first;
			}
		}
	}
	if (propertyItem == nullptr)
	{
		propertyItem = static_cast<const ReflectedPropertyItem*>(item);
	}

	clearGroups(propertyItem);
	clearProperties(propertyItem);
}

const AbstractItem* PropertyTreeModel::mappedItem(const ReflectedPropertyItem* item) const
{
	auto& object = getObject();
	if (!object.isValid())
	{
		return item;
	}

	auto definition = get<IDefinitionManager>()->getDefinition(object);
	while (item != nullptr && !isMapped(item))
	{
		auto && path = item->getPath();
		auto propertyAccessor = definition->bindProperty(path, object);

		auto parentItem = parentProperty(item);
		auto groupObj = getGroupForProperty(propertyAccessor, *get<IDefinitionManager>());
		if (groupObj != nullptr)
		{
			auto groupsIt = groups_.find(parentItem);
			if(groupsIt != groups_.end())
			{
				auto& groups = groupsIt->second;
				auto& propertyObject = propertyAccessor.getObject();
				auto it = std::find_if(groups.begin(), groups.end(), [&](const PropertyGroupItemPtr& groupItem) {
					return groupItem->getHash() == groupObj->getGroupNameHash(propertyObject);
				});
				TF_ASSERT(it != groups.end());
				return it->get();
			}
		}

		item = parentItem;
	}
	return item;
}

PropertyTreeModel::Groups PropertyTreeModel::getGroups(const PropertyItem propertyItem)
{
	const ReflectedPropertyItem *item = propertyItem.first;
	std::vector<PropertyItem> properties;
	Groups newGroups;
	uint64_t groupHash = propertyItem.second;
	collectProperties(item, properties, &newGroups, groupHash);
	
	auto& currGroupList = groups_[propertyItem.first];
	auto it = std::remove_if(newGroups.begin(), newGroups.end(), [&](const PropertyGroupItemPtr& groupItem) 
	{
		// Remove group if no property references that group
		{
			auto propIt =
				std::find_if(properties.begin(), properties.end(), [&](const PropertyItem& property)
			{
				uint64_t propertyUsesGroupWithHash = property.second;
				const MetaGroupObj &groupObj = groupItem->getGroupObj();
				const ObjectHandle &object   = groupItem->getObject();
				if (groupObj.getGroupNameHash(object, MetaGroupObj::FULL_GROUP_LEVEL) == propertyUsesGroupWithHash) 
					return true;

				return false;
			});

			if (propIt == properties.end()) return true;
		}

		// Remove group from list if it is already in the group list.
		{
			auto groupIt =
				std::find_if(currGroupList.begin(), currGroupList.end(),
					[&](const PropertyGroupItemPtr &existingGroup)
			{
				return (groupItem->getHash() == existingGroup->getHash());
			});

			return !(groupIt == currGroupList.end());
		}

	});
	newGroups.erase(it, newGroups.end());
	return newGroups;
}

void PropertyTreeModel::clearGroups(const ReflectedPropertyItem* item)
{
	auto groupsIt = groups_.find(item);
	if (groupsIt == groups_.end())
	{
		return;
	}

	groups_.erase(groupsIt);
}

PropertyTreeModel::PropertyItem PropertyTreeModel::propertyItem(const AbstractItem* item) const
{
	for (auto& groups : groups_)
	{
		for (auto& groupItem : groups.second)
		{
			if (groupItem.get() == item)
			{
				return PropertyItem(groups.first, groupItem->getHash());
			}
		}
	}
	return PropertyItem(static_cast<const ReflectedPropertyItem*>(item), 0);
}

void PropertyTreeModel::collectProperties(const ReflectedPropertyItem* item, std::vector<PropertyItem>& o_Properties, Groups* groups, uint64_t groupHash)
{
	auto makePropertyGroupIfUnique = [this](Groups *groups, ObjectHandleT<MetaGroupObj> groupObj, 
		                                    ObjectHandle propertyObject, uint64_t checkHash,
		                                    int subGroupIndex) 
	{
		auto found = std::find_if(groups->begin(), groups->end(), [&](const PropertyGroupItemPtr& group)
		{
			return group->getHash() == checkHash;
		});

		if (found == groups->end())
		{
			groups->emplace_back(std::make_unique<PropertyGroupItem>(*groupObj, 
																	 propertyObject,
																	 subGroupIndex));
		}
	};

	auto& object = getObject();
	if (!object.isValid())
	{
		return;
	}

	auto definition = get<IDefinitionManager>()->getDefinition(object);
	auto& properties = getProperties(item);
	for (auto& property : properties)
	{
		auto propertyGroupHash = groupHash;
		auto && path = property->getPath();
		auto propertyAccessor = definition->bindProperty(path, object);
		ObjectHandleT<MetaGroupObj> groupObj = getGroupForProperty(propertyAccessor, *get<IDefinitionManager>());
		if (groupObj != nullptr)
		{
			auto& propertyObject = propertyAccessor.getObject();
			bool subGroupDetected = (propertyGroupHash != 0);

			if (!subGroupDetected) 
			{
				propertyGroupHash = groupObj->getGroupNameHash(propertyObject, MetaGroupObj::FULL_GROUP_LEVEL);
			}

			if (groups)
			{
				// NOTE: Level 0 contains the full group name if there are multiple sub groups. In the case
				// there is, we don't want to print the full name, but the sub-names, which are from index
				// 1 onwards in that case.
				int numSubGroups = groupObj->getNumSubGroups();
				int subGroupStartIndex = (numSubGroups > 1) ? 1 : 0;

				if (subGroupDetected)
				{
					int lastSubGroupIndex = numSubGroups - 1;
					for (int subGroupIndex = subGroupStartIndex; subGroupIndex < numSubGroups; subGroupIndex++)
					{
						uint64_t checkHash = groupObj->getGroupNameHash(propertyObject, subGroupIndex);
						if (checkHash == propertyGroupHash)
						{
							// NOTE: If last subgroup, the group needs to use the full hash because the properties are
							// bound to the full name of the group hash.
							uint64_t subGroupHash;
							int nextIndex = subGroupIndex + 1;

							if (nextIndex == lastSubGroupIndex)
							{
								subGroupHash = groupObj->getGroupNameHash(propertyObject, MetaGroupObj::FULL_GROUP_LEVEL);
								makePropertyGroupIfUnique(groups, groupObj, propertyObject, subGroupHash, nextIndex);
							}
							else if (nextIndex > lastSubGroupIndex)
							{
								// do nothing
							}
							else
							{
								subGroupHash = groupObj->getGroupNameHash(propertyObject, nextIndex);
								makePropertyGroupIfUnique(groups, groupObj, propertyObject, subGroupHash, nextIndex);
							}

							propertyGroupHash = groupObj->getGroupNameHash(propertyObject, MetaGroupObj::FULL_GROUP_LEVEL);
							break;
						}
					}
				}
				else
				{
					uint64_t checkHash = groupObj->getGroupNameHash(propertyObject, subGroupStartIndex);
					makePropertyGroupIfUnique(groups, groupObj, propertyObject, checkHash, subGroupStartIndex);
				}
			}

		}

		auto filterResult = filterProperty(property.get());
		switch (filterResult)
		{
		case FILTER_INCLUDE:
			o_Properties.push_back(PropertyItem(property.get(), propertyGroupHash));
			break;

		case FILTER_INCLUDE_CHILDREN:
			collectProperties(property.get(), o_Properties, groups, propertyGroupHash);
			break;

		case FILTER_IGNORE:
		default:
			break;
		}
	}
}

PropertyTreeModel::FilterResult PropertyTreeModel::filterProperty(const ReflectedPropertyItem* item) const
{
	auto& object = getObject();
	auto& path = item->getPath();

	auto& definitionManager = *get<IDefinitionManager>();
	auto definition = definitionManager.getDefinition(object);
	if (definition == nullptr)
	{
		return FILTER_INCLUDE;
	}

	auto propertyAccessor = definition->bindProperty(path, object);
	if (!propertyAccessor.isValid() || !propertyAccessor.getProperty()->isValue())
	{
		return FILTER_IGNORE;
	}

	// InPlace visibility can be dynamic, check for dynamic hidden properties (SpawnComponent - "Spawn Template")
	auto hidden = false;
	auto dynamicHidden = false; 
	auto hiddenCallback = [&](const ObjectHandleT<MetaHiddenObj>& hiddenObj)
	{
		auto currentHidden = hiddenObj->isHidden(propertyAccessor.getObject());;
		hidden |= currentHidden;
		dynamicHidden |= currentHidden && hiddenObj->isDynamic();
	};
	forEachMetaData<MetaHiddenObj>(propertyAccessor, *get<IDefinitionManager>(), hiddenCallback);

	auto metaInPlaceObj = findFirstMetaData<MetaInPlaceObj>(propertyAccessor, *get<IDefinitionManager>());
	if (metaInPlaceObj != nullptr)
	{
		return dynamicHidden ? FILTER_IGNORE : FILTER_INCLUDE_CHILDREN;
	}

	return hidden ? FILTER_IGNORE : FILTER_INCLUDE;
}
}
}