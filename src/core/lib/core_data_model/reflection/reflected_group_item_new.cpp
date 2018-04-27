#include "reflected_group_item_new.hpp"

#include "reflected_property_item_new.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/object/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_base.hpp"

#include "core_reflection/interfaces/i_reflection_controller.hpp"

#include "core_string_utils/string_utils.hpp"
#include "core_dependency_system/depends.hpp"

#include <codecvt>

namespace wgt
{
namespace
{
bool isSameGroup(ObjectHandleT<MetaGroupObj> pItemGroup, ObjectHandleT<MetaGroupObj> pFoundGroup, const ObjectHandle& object)
{
	return (pItemGroup != nullptr) && (pFoundGroup != nullptr) &&
	(pFoundGroup == pItemGroup || (pFoundGroup->getGroupNameHash(object) == pItemGroup->getGroupNameHash(object)));
}

} // namespace

class ReflectedGroupItemNew::Implementation : public Depends<IReflectionController, IDefinitionManager>
{
public:
	Implementation(const MetaData & metaData, ObjectHandleT<MetaGroupObj> groupObj);

	ObjectHandleT<MetaGroupObj> groupObj_;
	std::string displayName_;
	std::vector<std::unique_ptr<ReflectedTreeItemNew>> children_;
	const MetaData * metaData_;
};

ReflectedGroupItemNew::Implementation::Implementation(
	const MetaData & metaData, ObjectHandleT<MetaGroupObj> groupObj)
	: groupObj_(groupObj)
	, metaData_( &metaData )
{
}

ReflectedGroupItemNew::ReflectedGroupItemNew(
	const MetaData & metaData, ObjectHandleT<MetaGroupObj> groupObj, 
	ReflectedTreeItemNew* parent, size_t index, const std::string& inplacePath)
    : ReflectedTreeItemNew(parent, index, inplacePath)
	, impl_(new Implementation( metaData, groupObj))
{
	TF_ASSERT(impl_->groupObj_ != nullptr);
	if (impl_->groupObj_ == nullptr)
	{
		impl_->displayName_.clear();
	}
	else
	{
		impl_->displayName_ = StringUtils::to_string(impl_->groupObj_->getGroupName(getObject()));
	}
	HashUtilities::combine(id_, impl_->displayName_);
}

ReflectedGroupItemNew::~ReflectedGroupItemNew()
{
}

Variant ReflectedGroupItemNew::getData(int column, ItemRole::Id roleId) const /* override */
{
	auto obj = this->getRootObject();
	if (obj == nullptr)
	{
		return Variant();
	}
	auto definition = getDefinition();
	if (definition == nullptr)
	{
		return Variant();
	}

	if (roleId == ItemRole::displayId)
	{
		switch (column)
		{
		case 0:
			return impl_->displayName_.c_str();

		default:
			return "Reflected Group";
		}
	}
	else if (roleId == ItemRole::itemIdId)
	{
		return getId();
	}

	if (roleId == ObjectRole::roleId_)
	{
		return this->getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return this->getRootObject();
	}
	else if (roleId == ValueRole::roleId_)
	{
		auto collectionHolder = std::make_shared<CollectionHolder<Variants>>();
		Variants& childValues = collectionHolder->storage();

		this->getChildValues(childValues);

		return std::move(Collection(collectionHolder));
	}
	else if (roleId == ValueTypeRole::roleId_)
	{
		return TypeId::getType<Collection>().getName();
	}
	else if (roleId == ItemRole::tooltipId)
	{
		auto definitionManager = impl_->get<IDefinitionManager>();
		auto tooltipObj = findFirstMetaData<MetaTooltipObj>(*impl_->metaData_, *definitionManager);
		if (tooltipObj != nullptr)
		{
			return tooltipObj->getTooltip(nullptr);
		}
	}
	return Variant();
}

bool ReflectedGroupItemNew::setData(int column, ItemRole::Id roleId, const Variant& data) /* override */
{
	auto object = this->getObject();
	if (object == nullptr)
	{
		return false;
	}

	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto definition = this->getDefinition();
	if (definition == nullptr)
	{
		return false;
	}

	Collection collection;
	const bool isOk = data.tryCast(collection);
	if (!isOk)
	{
		return false;
	}

	auto iter = collection.begin();
	this->enumerateVisibleProperties([&](const IBasePropertyPtr& property, const std::string& inPlacePath) {
		if (iter == collection.end())
		{
			return false;
		}

		auto pFoundGroupObj = findFirstMetaData<MetaGroupObj>(*property, *pDefinitionManager);
		if (isSameGroup(impl_->groupObj_, pFoundGroupObj, object))
		{
			const Variant& value = *iter++;
			auto path = inPlacePath + property->getName();
			auto propertyAccessor = definition->bindProperty(path.c_str(), object);
			auto controller = impl_->get<IReflectionController>();
			if (controller)
			{
				controller->setValue(propertyAccessor, value);
			}
			else
			{
				propertyAccessor.setValue(value);
			}
		}
		return true;
	});

	return true;
}

const ObjectHandle& ReflectedGroupItemNew::getRootObject() const /* override */
{
	return parent_->getRootObject();
}

const ObjectHandle& ReflectedGroupItemNew::getObject() const /* override */
{
	return parent_->getObject();
}

ReflectedTreeItemNew* ReflectedGroupItemNew::getChild(size_t index) const /* override */
{
	ReflectedTreeItemNew* child = nullptr;
	if (impl_->children_.size() > index)
	{
		child = impl_->children_[index].get();
	}

	if (child != nullptr)
	{
		return child;
	}

	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return nullptr;
	}

	auto parent = const_cast<ReflectedGroupItemNew*>(this);
	int skipChildren = static_cast<int>(impl_->children_.size());
	this->enumerateVisibleProperties([this, parent, &child, &skipChildren, pDefinitionManager](
	const IBasePropertyPtr& property, const std::string& inPlacePath) {
		auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *pDefinitionManager);
		if ((property != nullptr) && isSameGroup(impl_->groupObj_, groupObj, parent->getObject()))
		{
			// Skip already iterated children
			if (--skipChildren < 0)
			{
				impl_->children_.emplace_back(
				new ReflectedPropertyItemNew(property, parent, impl_->children_.size(), inPlacePath));
				child = impl_->children_.back().get();
				return false;
			}
		}
		return true;
	});

	return child;
}

int ReflectedGroupItemNew::rowCount() const /* override */
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return 0;
	}

	int count = 0;
	this->enumerateVisibleProperties(
	[this, &count, pDefinitionManager](const IBasePropertyPtr& property, const std::string&) {
		auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *pDefinitionManager);
		count += isSameGroup(impl_->groupObj_, groupObj, getObject());
		return true;
	});

	return count;
}

bool ReflectedGroupItemNew::preSetValue(const PropertyAccessor& accessor, const Variant& value) /* override */
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
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

bool ReflectedGroupItemNew::postSetValue(const PropertyAccessor& accessor, const Variant& value) /* override */
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
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

bool ReflectedGroupItemNew::preInsert(const PropertyAccessor& accessor, size_t index, size_t count)
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preInsert(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedGroupItemNew::postInserted(const PropertyAccessor& accessor, size_t index, size_t count)
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postInserted(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedGroupItemNew::preErase(const PropertyAccessor& accessor, size_t index, size_t count)
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preErase(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}

bool ReflectedGroupItemNew::postErased(const PropertyAccessor& accessor, size_t index, size_t count)
{
	for (auto it = impl_->children_.begin(); it != impl_->children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postErased(accessor, index, count))
		{
			return true;
		}
	}
	return false;
}

void ReflectedGroupItemNew::getChildValues(Variants& outChildValues) const
{
	if (impl_->groupObj_ == nullptr)
	{
		return;
	}

	auto object = this->getObject();
	if (object == nullptr)
	{
		return;
	}

	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return;
	}

	auto definition = pDefinitionManager->getDefinition(object);
	if (definition == nullptr)
	{
		return;
	}

	this->enumerateVisibleProperties([&](const IBasePropertyPtr& property, const std::string& inplacePath) {
		// Check if this property is a part of this group
		const auto pFoundGroupObj = findFirstMetaData<MetaGroupObj>(*property, *pDefinitionManager);
		if (isSameGroup(impl_->groupObj_, pFoundGroupObj, object))
		{
			auto path = inplacePath + property->getName();
			auto propertyAccessor = definition->bindProperty(path.c_str(), object);
			auto controller = impl_->get<IReflectionController>();
			Variant value = controller ? controller->getValue(propertyAccessor) : propertyAccessor.getValue();
			outChildValues.emplace_back(value);
		}
		return true;
	});
}
} // end namespace wgt
