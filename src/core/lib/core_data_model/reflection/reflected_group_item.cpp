#include "reflected_group_item.hpp"

#include "reflected_property_item.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_string_utils/string_utils.hpp"
#include <codecvt>

namespace wgt
{
ReflectedGroupItem::ReflectedGroupItem(
	const MetaData & metaData, 
	ObjectHandleT<MetaGroupObj> groupObj, ReflectedItem* parent,
                                       const std::string& inplacePath)
    : ReflectedItem(parent, inplacePath)
	, metaData_( &metaData )
	, groupObj_(groupObj)
{
	TF_ASSERT(groupObj_ != nullptr);
	if (groupObj_ == nullptr)
	{
		displayName_.clear();
	}
	else
	{
		displayName_ = StringUtils::to_string(groupObj_->getGroupName(getObject()));
	}
}

const char* ReflectedGroupItem::getDisplayText(int column) const
{
	return displayName_.c_str();
}

Variant ReflectedGroupItem::getData(int column, ItemRole::Id roleId) const
{
	auto obj = getRootObject();
	if (obj == nullptr)
	{
		return Variant();
	}
	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return Variant();
	}

	if (roleId == IndexPathRole::roleId_)
	{
		if (parent_ == nullptr)
		{
			return displayName_;
		}
		std::string parentIndexPath = parent_->getPath();
		return parentIndexPath + displayName_;
	}
	else if (roleId == ObjectRole::roleId_)
	{
		return getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return getRootObject();
	}
	else if (roleId == ItemRole::tooltipId)
	{
		auto definitionManager = getDefinitionManager();
		auto tooltipObj = findFirstMetaData<MetaTooltipObj>(*metaData_, *definitionManager);
		if (tooltipObj != nullptr)
		{
			return tooltipObj->getTooltip(nullptr);
		}
		return Variant();
	}
	else if (roleId == ValueRole::roleId_)
	{
		auto collectionHolder = std::make_shared<CollectionHolder<Variants>>();
		Variants& childValues_ = collectionHolder->storage();

		getChildValues(childValues_);

		return std::move(Collection(collectionHolder));
	}
	return Variant();
}

void ReflectedGroupItem::getChildValues(Variants& childValues) const
{
	if (groupObj_ == nullptr)
		return;

	auto object = getObject();
	if (object == nullptr)
		return;

	auto definitionManager = getDefinitionManager();
	if (definitionManager == nullptr)
		return;

	auto definition = definitionManager->getDefinition(object);
	if (definition == nullptr)
		return;

	EnumerateVisibleProperties([&](IBasePropertyPtr property, const std::string& inplacePath) {
		// Check if this property is a part of this group
		const auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *definitionManager);
		if (isSameGroup(groupObj))
		{
			auto path = inplacePath + property->getName();
			auto propertyAccessor = definition->bindProperty(path.c_str(), object);
			Variant value = getController()->getValue(propertyAccessor);
			childValues.emplace_back(value);
		}
		return true;
	});
}

bool ReflectedGroupItem::isSameGroup(ObjectHandleT<MetaGroupObj> group) const
{
	const auto& object = getObject();
	return groupObj_ != nullptr && group != nullptr &&
	(group == groupObj_ || group->getGroupNameHash(object) == groupObj_->getGroupNameHash(object));
}

bool ReflectedGroupItem::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	auto controller = getController();
	if (controller == nullptr)
	{
		return false;
	}

	auto object = getObject();
	if (object == nullptr)
	{
		return false;
	}

	auto definitionManager = getDefinitionManager();
	if (definitionManager == nullptr)
	{
		return false;
	}

	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return false;
	}

	Collection collection;
	bool isOk = data.tryCast(collection);
	if (!isOk)
	{
		return false;
	}
	size_t value_size = collection.size();

	auto iter = collection.begin();

	EnumerateVisibleProperties([&](IBasePropertyPtr property, const std::string& inplacePath) {
		if (iter == collection.end())
			return false;

		auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *getDefinitionManager());
		if (isSameGroup(groupObj))
		{
			const Variant& value = *iter++;
			auto path = inplacePath + property->getName();
			auto propertyAccessor = definition->bindProperty(path.c_str(), object);
			controller->setValue(propertyAccessor, value);
		}
		return true;
	});

	return true;
}

GenericTreeItem* ReflectedGroupItem::getChild(size_t index) const
{
	GenericTreeItem* child = nullptr;
	if (children_.size() > index)
	{
		child = children_[index].get();
	}

	if (child != nullptr)
		return child;

	auto parent = const_cast<ReflectedGroupItem*>(this);
	int skipChildren = static_cast<int>(children_.size());
	EnumerateVisibleProperties(
	[this, parent, &child, &skipChildren](IBasePropertyPtr property, const std::string& inplacePath) {
		auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *getDefinitionManager());
		if (isSameGroup(groupObj) && property != nullptr)
		{
			// Skip already iterated children
			if (--skipChildren < 0)
			{
				children_.emplace_back(new ReflectedPropertyItem(property, parent, inplacePath));
				child = children_.back().get();
				return false;
			}
		}
		return true;
	});

	return child;
}

bool ReflectedGroupItem::empty() const
{
	bool isEmpty = true;
	EnumerateVisibleProperties([this, &isEmpty](IBasePropertyPtr property, const std::string&) {
		auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *getDefinitionManager());
		if (isSameGroup(groupObj))
		{
			isEmpty = false;
		}
		return isEmpty;
	});
	return isEmpty;
}

size_t ReflectedGroupItem::size() const
{
	size_t count = 0;
	EnumerateVisibleProperties([this, &count](IBasePropertyPtr property, const std::string&) {
		auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *getDefinitionManager());
		count += isSameGroup(groupObj);
		return true;
	});

	return count;
}

//==============================================================================
bool ReflectedGroupItem::preSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preSetValue(accessor, value))
		{
			return true;
		}
	}
	return false;
}

//==============================================================================
bool ReflectedGroupItem::postSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postSetValue(accessor, value))
		{
			return true;
		}
	}
	return false;
}
} // end namespace wgt
