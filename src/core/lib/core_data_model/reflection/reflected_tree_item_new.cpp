#include "reflected_tree_item_new.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"

namespace wgt
{
ReflectedTreeItemNew::ReflectedTreeItemNew(const ReflectedTreeModelNew& model)
    : parent_(nullptr), id_(0), path_(""), index_(std::numeric_limits<size_t>::max()), model_(&model)
{
}

ReflectedTreeItemNew::ReflectedTreeItemNew(ReflectedTreeItemNew* parent, size_t index, const char* path)
    : parent_(parent), index_(index), id_(HashUtilities::compute(path)), path_(path),
      model_(parent != nullptr ? parent->getModel() : nullptr)
{
}

ReflectedTreeItemNew::ReflectedTreeItemNew(ReflectedTreeItemNew* parent, size_t index, const std::string& path)
    : parent_(parent), index_(index), id_(HashUtilities::compute(path)), path_(path),
      model_(parent != nullptr ? parent->getModel() : nullptr)
{
}

ReflectedTreeItemNew::~ReflectedTreeItemNew()
{
}

const IClassDefinition* ReflectedTreeItemNew::getDefinition() const
{
	auto parent = this->getParent();
	if (parent == nullptr)
	{
		return nullptr;
	}
	return static_cast<const ReflectedTreeItemNew*>(parent)->getDefinition();
}

bool ReflectedTreeItemNew::isInPlace() const
{
	return false;
}

bool ReflectedTreeItemNew::hasController() const /* override */
{
	return true;
}

uint64_t ReflectedTreeItemNew::getId() const
{
	return id_;
}

const std::string& ReflectedTreeItemNew::getPath() const
{
	return path_;
}

const ReflectedTreeModelNew* ReflectedTreeItemNew::getModel() const
{
	TF_ASSERT((model_ != nullptr) && "Tree item is not attached to model");
	return model_;
}

const ReflectedTreeItemNew* ReflectedTreeItemNew::getParent() const
{
	return parent_;
}

ReflectedTreeItemNew* ReflectedTreeItemNew::getParent()
{
	return parent_;
}

size_t ReflectedTreeItemNew::getIndex() const
{
	return index_;
}

void ReflectedTreeItemNew::setIndex(size_t index)
{
	index_ = index;
}

bool ReflectedTreeItemNew::enumerateVisibleProperties(const PropertyCallback& callback) const
{
	auto object = this->getObject();
	if (object == nullptr)
	{
		return true;
	}

	auto pDefinitionManager = get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return true;
	}

	return enumerateVisibleProperties(object, *pDefinitionManager, "", callback);
}

bool ReflectedTreeItemNew::enumerateVisibleProperties(ObjectHandle object, const IDefinitionManager& definitionManager,
                                                      const std::string& inPlacePath, const PropertyCallback& callback)
{
	auto definition = definitionManager.getDefinition(object);
	if (definition == nullptr)
	{
		return true;
	}

	for (const auto& property : definition->allProperties())
	{
		TF_ASSERT(property != nullptr);

		// Method-only properties should be hidden from the UI
		if (property->isMethod() && !property->isValue())
		{
			continue;
		}

		auto propertyAccessor = definition->bindProperty(property->getName(), object);

		auto inPlace = findFirstMetaData<MetaInPlaceObj>(*property, definitionManager);
		if (inPlace != nullptr)
		{
			if (!propertyAccessor.canGetValue())
			{
				continue;
			}

			const Variant& value = propertyAccessor.getValue();
			const bool isCollection = value.typeIs<Collection>();
			if (isCollection)
			{
				// TODO: Support InPlace collections
				// For now just show the collection
				// Currently Despair's ContainerProperty is pushed up to the collection
				// Eventually we'll need a MetaContainer which we can check here

				if (!callback(property, inPlacePath))
				{
					return false;
				}
			}

			ObjectHandle handle;
			const bool isObjectHandle = value.tryCast(handle);
			if (isObjectHandle)
			{
				handle = reflectedRoot(handle, definitionManager);
				auto path = inPlacePath + property->getName() + ".";
				if (!enumerateVisibleProperties(handle, definitionManager, path, callback))
				{
					return false;
				}
			}
			continue;
		}

		auto hidden = false;
		auto hiddenCallback = [&](const ObjectHandleT<MetaHiddenObj>& hiddenObj)
		{
			hidden |= hiddenObj->isHidden(propertyAccessor.getObject());
		};
		forEachMetaData<MetaHiddenObj>(propertyAccessor, definitionManager, hiddenCallback);

		if(hidden)
		{
			continue;
		}

		if (!callback(property, inPlacePath))
		{
			return false;
		}
	}
	return true;
}
} // end namespace wgt
