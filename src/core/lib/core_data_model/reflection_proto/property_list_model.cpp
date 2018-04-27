#include "property_list_model.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
namespace proto
{
PropertyListModel::PropertyListModel(const ObjectHandle& object) : ReflectedTreeModel(object)
{
}

PropertyListModel::~PropertyListModel()
{
}

std::unique_ptr<ReflectedTreeModel::Children> PropertyListModel::mapChildren(const AbstractItem* item)
{
	auto children = new Children();
	if (item == nullptr)
	{
		std::vector<const ReflectedPropertyItem*> properties;
		collectProperties(nullptr, properties);
		for (auto& property : properties)
		{
			children->push_back(property);
		}
	}
	return std::unique_ptr<Children>(children);
}

AbstractTreeModel::ItemIndex PropertyListModel::childHint(const ReflectedPropertyItem* item) const
{
	return ItemIndex();
}

void PropertyListModel::collectProperties(const ReflectedPropertyItem* item,
                                          std::vector<const ReflectedPropertyItem*>& o_Properties)
{
	auto& properties = getProperties(item);
	for (auto& property : properties)
	{
		auto filterResult = filterProperty(property.get());
		switch (filterResult)
		{
		case FILTER_INCLUDE:
			o_Properties.push_back(property.get());
			break;

		case FILTER_INCLUDE_CHILDREN:
			collectProperties(property.get(), o_Properties);
			break;

		case FILTER_IGNORE:
		default:
			break;
		}
	}
}

PropertyListModel::FilterResult PropertyListModel::filterProperty(const ReflectedPropertyItem* item) const
{
	auto& object = getObject();
	auto && path = item->getPath();

	auto& definitionManager = *get<IDefinitionManager>();
	auto definition = definitionManager.getDefinition(object);
	if (definition == nullptr)
	{
		return FILTER_IGNORE;
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
		auto currentHidden = hiddenObj->isHidden(propertyAccessor.getObject());
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