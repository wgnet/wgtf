#include "property_list_model.hpp"

namespace wgt
{
namespace proto
{
PropertyListModel::PropertyListModel(IComponentContext& context, const ObjectHandle& object)
    : ReflectedTreeModel(context, object)
{
}

PropertyListModel::~PropertyListModel()
{
}

std::unique_ptr<ReflectedTreeModel::Children> PropertyListModel::getChildren(const AbstractItem* item)
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

AbstractTreeModel::ItemIndex PropertyListModel::childHint(const AbstractItem* item)
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
		case INCLUDE:
			o_Properties.push_back(property.get());

		case INCLUDE_CHILDREN:
			collectProperties(property.get(), o_Properties);

		case IGNORE:
		default:
			break;
		}
	}
}

PropertyListModel::FilterResult PropertyListModel::filterProperty(const ReflectedPropertyItem* item) const
{
	auto& object = item->getObject();
	auto& path = item->getPath();

	auto& definitionManager = *getDefinitionManager();
	auto definition = object.getDefinition(definitionManager);
	if (definition == nullptr)
	{
		return IGNORE;
	}
	auto propertyAccessor = definition->bindProperty(path.c_str(), object);

	if (propertyAccessor.canGetValue())
	{
		auto value = propertyAccessor.getValue();
		if (!value.typeIs<ObjectHandle>())
		{
			if (!value.typeIs<Collection>())
			{
				return INCLUDE;
			}
		}

		return INCLUDE_CHILDREN;
	}

	return IGNORE;
}
}
}