#include "reflected_object_item_new.hpp"

#include "reflected_group_item_new.hpp"
#include "reflected_property_item_new.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"

#include "core_string_utils/string_utils.hpp"

#include <codecvt>
#include <set>

namespace wgt
{
namespace
{
bool compareWStrings(const wchar_t* a, const wchar_t* b)
{
	return wcscmp(a, b) < 0;
}

} // namespace

class ReflectedObjectItemNew::Implementation : public Depends<IDefinitionManager>
{
public:
	Implementation(const ObjectHandle& object);

	typedef std::set<const wchar_t*, bool (*)(const wchar_t*, const wchar_t*)> Groups;

	const Groups& getGroups(const ReflectedObjectItemNew& self);

	ObjectHandle object_;
	std::string displayName_;
	std::vector<std::unique_ptr<ReflectedTreeItemNew>> children_;
	Groups groups_;
};

ReflectedObjectItemNew::Implementation::Implementation(const ObjectHandle& object)
    : object_(object), groups_(compareWStrings)
{
}

const ReflectedObjectItemNew::Implementation::Groups& ReflectedObjectItemNew::Implementation::getGroups(
const ReflectedObjectItemNew& self)
{
	auto definition = self.getDefinition();
	if (!groups_.empty() || (definition == nullptr))
	{
		return groups_;
	}

	auto pDefinitionManager = self.impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return groups_;
	}

	auto& parent = const_cast<ReflectedObjectItemNew&>(self);
	self.enumerateVisibleProperties(
	[this, &self, &parent, pDefinitionManager](const IBasePropertyPtr& property, const std::string& inPlacePath) {
		auto groupObj = findFirstMetaData<MetaGroupObj>(*property, *pDefinitionManager);
		if (groupObj != nullptr && groups_.insert(groupObj->getGroupName(parent.getObject())).second)
		{
			children_.emplace_back(
				new ReflectedGroupItemNew(
					property->getMetaData(), groupObj, &parent, children_.size(), inPlacePath));
		}
		return true;
	});
	return groups_;
}

ReflectedObjectItemNew::ReflectedObjectItemNew(const ObjectHandle& object, const ReflectedTreeModelNew& model)
    : ReflectedTreeItemNew(model), impl_(new Implementation(object))
{
}

ReflectedObjectItemNew::ReflectedObjectItemNew(const ObjectHandle& object, ReflectedTreeItemNew* parent, size_t index)
    : ReflectedTreeItemNew(parent, index, parent ? parent->getPath() + "." : ""), impl_(new Implementation(object))
{
}

ReflectedObjectItemNew::~ReflectedObjectItemNew()
{
}

Variant ReflectedObjectItemNew::getData(int column, ItemRole::Id roleId) const /* override */
{
	if (roleId == ItemRole::displayId)
	{
		switch (column)
		{
		case 0:
			if (impl_->displayName_.empty())
			{
				auto definition = this->getDefinition();
				if (definition == nullptr)
				{
					return "";
				}
				auto pDefinitionManager = impl_->get<IDefinitionManager>();
				if (pDefinitionManager == nullptr)
				{
					return "";
				}
				auto displayName = findFirstMetaData<MetaDisplayNameObj>(*definition, *pDefinitionManager);
				if (displayName == nullptr)
				{
					impl_->displayName_ = definition->getName();
				}
				else
				{
					impl_->displayName_ = StringUtils::to_string(displayName->getDisplayName(impl_->object_));
				}
			}
			return impl_->displayName_.c_str();

		default:
			auto definition = getDefinition();
			if (definition == nullptr)
			{
				return "";
			}
			return definition->getName();
		}
	}
	else if (roleId == ItemRole::itemIdId)
	{
		return getId();
	}

	if (roleId == ValueRole::roleId_)
	{
		return impl_->object_;
	}
	if (roleId == ValueTypeRole::roleId_)
	{
		return TypeId::getType<ObjectHandle>().getName();
	}
	else if (roleId == ObjectRole::roleId_)
	{
		return this->getObject();
	}
	else if (roleId == RootObjectRole::roleId_)
	{
		return this->getRootObject();
	}

	return Variant();
}

bool ReflectedObjectItemNew::setData(int column, ItemRole::Id roleId, const Variant& data) /* override */
{
	if (roleId != ValueRole::roleId_)
	{
		return false;
	}

	ObjectHandle other;
	if (!data.tryCast(other))
	{
		return false;
	}

	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return false;
	}

	auto obj = this->getRootObject();
	auto definition = pDefinitionManager->getDefinition(obj);
	auto otherDef = pDefinitionManager->getDefinition(other);
	if (definition != otherDef)
	{
		return false;
	}

	for (auto prop : definition->allProperties())
	{
		auto accessor = definition->bindProperty(prop->getName(), obj);
		auto otherAccessor = definition->bindProperty(prop->getName(), other);
		if (accessor.canSetValue())
		{
			TF_ASSERT(otherAccessor.canGetValue());
			accessor.setValue(otherAccessor.getValue());
		}
	}

	const auto pParent = this->getParent();
	if (pParent)
	{
		return pParent->setData(column, roleId, obj);
	}

	return true;
}

const ObjectHandle& ReflectedObjectItemNew::getRootObject() const /* override */
{
	return parent_ ? parent_->getRootObject() : impl_->object_;
}

const ObjectHandle& ReflectedObjectItemNew::getObject() const /* override */
{
	return impl_->object_;
}

const IClassDefinition* ReflectedObjectItemNew::getDefinition() const
{
	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return nullptr;
	}
	return pDefinitionManager->getDefinition(impl_->object_);
}

ReflectedTreeItemNew* ReflectedObjectItemNew::getChild(size_t index) const
{
	if (impl_->children_.size() > index)
	{
		return impl_->children_[index].get();
	}

	size_t currentIndex = 0;
	this->enumerateChildren([&currentIndex, index](ReflectedTreeItemNew& item) { return (currentIndex++ == index); });

	if (currentIndex > 0)
	{
		return impl_->children_[--currentIndex].get();
	}
	return nullptr;
}

int ReflectedObjectItemNew::rowCount() const
{
	int count = 0;

	this->enumerateChildren([&count](ReflectedTreeItemNew&) {
		++count;
		return true;
	});

	return count;
}

bool ReflectedObjectItemNew::isInPlace() const
{
	return parent_ != nullptr;
}

bool ReflectedObjectItemNew::preSetValue(const PropertyAccessor& accessor, const Variant& value)
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

bool ReflectedObjectItemNew::postSetValue(const PropertyAccessor& accessor, const Variant& value)
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

bool ReflectedObjectItemNew::preInsert(const PropertyAccessor& accessor, size_t index, size_t count)
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

bool ReflectedObjectItemNew::postInserted(const PropertyAccessor& accessor, size_t index, size_t count)
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

bool ReflectedObjectItemNew::preErase(const PropertyAccessor& accessor, size_t index, size_t count)
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

bool ReflectedObjectItemNew::postErased(const PropertyAccessor& accessor, size_t index, size_t count)
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

void ReflectedObjectItemNew::enumerateChildren(const ReflectedItemCallback& callback) const
{
	// Get the groups and iterate them first
	const auto& groups = impl_->getGroups(*this);

	// This will iterate all the groups and any cached children
	for (auto& child : impl_->children_)
	{
		if (!callback(*child.get()))
		{
			return;
		}
	}

	auto pDefinitionManager = impl_->get<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return;
	}

	// ReflectedGroupItem children handle grouped items
	int skipChildCount = static_cast<int>(impl_->children_.size() - groups.size());
	auto parent = const_cast<ReflectedObjectItemNew*>(this);
	this->enumerateVisibleProperties([&](const IBasePropertyPtr& property, const std::string& inPlacePath) {
		bool isGrouped = findFirstMetaData<MetaGroupObj>(*property, *pDefinitionManager) != nullptr;
		if (!isGrouped)
		{
			// Skip already iterated children
			if (--skipChildCount < 0)
			{
				impl_->children_.emplace_back(
				new ReflectedPropertyItemNew(property, parent, impl_->children_.size(), inPlacePath));
				return callback(*impl_->children_.back().get());
			}
		}
		return true;
	});
}
} // end namespace wgt
