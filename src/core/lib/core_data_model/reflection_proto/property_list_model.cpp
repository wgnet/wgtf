#include "property_list_model.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"

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
		enumerateAllProperties(nullptr, *children);
	}
	return std::unique_ptr<Children>(children);
}

AbstractTreeModel::ItemIndex PropertyListModel::childHint(const ReflectedPropertyItem* item)
{
	return ItemIndex();
}

void PropertyListModel::enumerateAllProperties(const ReflectedPropertyItem* item, Children& children)
{
	auto& properties = enumerateProperties(item);
	for (auto& property : properties)
	{
		auto& object = property->getObject();
		auto& path = property->getPath();

		auto& definitionManager = *getDefinitionManager();
		auto definition = object.getDefinition(definitionManager);
		auto propertyAccessor = definition->bindProperty(path.c_str(), object);

		bool isAction = findFirstMetaData<MetaActionObj>(propertyAccessor, definitionManager) != nullptr;
		if (isAction)
		{
			continue;
		}

		bool isHidden = findFirstMetaData<MetaHiddenObj>(propertyAccessor, definitionManager) != nullptr;
		bool isInPlace = findFirstMetaData<MetaInPlaceObj>(propertyAccessor, definitionManager) != nullptr;
		if (!isHidden && !isInPlace && propertyAccessor.canGetValue())
		{
			auto value = propertyAccessor.getValue();
			ObjectHandle handle;
			if (!value.tryCast(handle))
			{
				Collection collection;
				if (!value.tryCast(collection))
				{
					children.push_back(property.get());
				}
			}
		}

		if (!isHidden || isInPlace)
		{
			enumerateAllProperties(property.get(), children);
		}
	}
}
}
}