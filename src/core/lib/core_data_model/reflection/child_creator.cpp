#include "child_creator.hpp"
#include "default_property_model_extensions.hpp"
#include "reflected_collection_item.hpp"

namespace wgt
{
namespace CCDetails
{
struct PropertyNodeEqual
{
	bool operator()(const std::shared_ptr<const PropertyNode>& left,
	                const std::shared_ptr<const PropertyNode>& right) const
	{
		assert(left != nullptr);
		assert(right != nullptr);
		if (left == right)
		{
			return true;
		}

		bool isEqualProperty =
		left->propertyType == right->propertyType && left->propertyInstance == right->propertyInstance;

		if (!isEqualProperty)
			return false;

		if (left->propertyType == PropertyNode::CollectionItem)
		{
			ReflectedIteratorValue* leftIter = left->object.getBase<ReflectedIteratorValue>();
			ReflectedIteratorValue* rightIter = right->object.getBase<ReflectedIteratorValue>();
			assert(leftIter != nullptr);
			assert(rightIter != nullptr);

			return leftIter->value == rightIter->value;
		}

		return left->object == right->object;
	}
};

} // namespace CCDetails

ChildCreator::ChildCreator() : extensions(ChildCreatorExtension::createDummy()), allocator(createDefaultAllocator())
{
}

ChildCreator::~ChildCreator()
{
	assert(propertiesIndex.empty());
}

std::shared_ptr<const PropertyNode> ChildCreator::createRoot(const ObjectHandle& handle)
{
	auto result =
	propertiesIndex.emplace(MakeRootNode(handle, *allocator), std::vector<std::shared_ptr<const PropertyNode>>());
	assert(result.second == true);
	return result.first->first;
}

void ChildCreator::updateSubTree(const std::shared_ptr<const PropertyNode>& parent)
{
	IDefinitionManager* defManager = get<IDefinitionManager>();
	if (defManager == nullptr)
	{
		return;
	}

	assert(parent != nullptr);
	std::vector<std::shared_ptr<const PropertyNode>> children;
	extensions->exposeChildren(parent, children, *defManager);

	auto iter = propertiesIndex.find(parent);
	if (iter == propertiesIndex.end())
	{
		// insert new parent into index and store it's children
		std::vector<std::shared_ptr<const PropertyNode>>& newItems = propertiesIndex[parent];
		newItems = std::move(children);
		for (size_t i = 0; i < newItems.size(); ++i)
		{
			nodeCreated(parent, newItems[i], i);
		}
	}
	else
	{
		CCDetails::PropertyNodeEqual equalCmp;

		bool childrenVectorsEqual = true;
		std::vector<std::shared_ptr<const PropertyNode>>& currentChildren = iter->second;
		if (children.size() == currentChildren.size())
		{
			for (size_t i = 0; i < children.size(); ++i)
			{
				if (!equalCmp(children[i], currentChildren[i]))
				{
					childrenVectorsEqual = false;
					break;
				}
			}
		}
		else
		{
			childrenVectorsEqual = false;
		}

		if (childrenVectorsEqual)
		{
			return;
		}

		for (auto iter = currentChildren.rbegin(); iter != currentChildren.rend(); ++iter)
		{
			removeNode(*iter);
		}

		std::swap(children, currentChildren);
		for (size_t i = 0; i < currentChildren.size(); ++i)
		{
			nodeCreated(parent, currentChildren[i], i);
		}
	}
}

void ChildCreator::removeNode(const std::shared_ptr<const PropertyNode>& parent)
{
	auto iter = propertiesIndex.find(parent);
	assert(iter != propertiesIndex.end());
	const std::vector<std::shared_ptr<const PropertyNode>>& children = iter->second;
	for (const std::shared_ptr<const PropertyNode>& node : children)
	{
		removeNode(node);
	}
	propertiesIndex.erase(iter);
	nodeRemoved(parent);
	assert(parent.unique());
}

void ChildCreator::clear()
{
	propertiesIndex.clear();
}

void ChildCreator::registerExtension(const std::shared_ptr<ChildCreatorExtension>& extension)
{
	extension->setAllocator(allocator);
	extensions =
	std::static_pointer_cast<ChildCreatorExtension>(ChildCreatorExtension::addExtension(extensions, extension));
}

void ChildCreator::unregisterExtension(const std::shared_ptr<ChildCreatorExtension>& extension)
{
	extension->setAllocator(nullptr);
	extensions =
	std::static_pointer_cast<ChildCreatorExtension>(ChildCreatorExtension::removeExtension(extensions, extension));
}

} // namespace wgt
