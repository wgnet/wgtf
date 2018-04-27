#include "class_definition_model_new.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_string_utils/string_utils.hpp"

#include <iterator>

namespace wgt
{
ITEMROLE(display);
ITEMROLE(decoration);
ITEMROLE(value);
ITEMROLE(indexPath);

namespace ClassDefinitionModelDetails
{
static const std::string s_RolesArr[] = {
	ItemRole::valueName, ItemRole::indexPathName,
};
static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
                                                 &s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);
}

namespace
{
class ClassDefinitionItem : public AbstractListItem
{
public:
	ClassDefinitionItem(const IClassDefinition* definition) : definition_(definition)
	{
		TF_ASSERT(definition_);
		auto metaDisplayName = findFirstMetaData<MetaDisplayNameObj>(*definition_, *definition_->getDefinitionManager());
		auto metaIcon = findFirstMetaData<MetaIconObj>(*definition_, *definition_->getDefinitionManager());
		displayName_ = metaDisplayName != nullptr ? StringUtils::to_string(metaDisplayName->getDisplayName()) : definition_->getName();
		iconPath_ = metaIcon != nullptr ? metaIcon->getIcon() : "";
	}

	Variant getData(int column, ItemRole::Id roleId) const override
	{
		if (roleId == ItemRole::displayId)
		{
			return displayName_;
		}
		if (roleId == ItemRole::decorationId)
		{
			if (!iconPath_.empty())
			{
				return iconPath_;
			}
		}
		else if (roleId == ItemRole::valueId)
		{
			return const_cast<IClassDefinition*>(definition_);
		}
		else if (roleId == ItemRole::indexPathId)
		{
			return TypeId::getType<ObjectHandle>().getName();
		}
		return Variant();
	}

	bool setData(int column, ItemRole::Id roleId, const Variant& data) override
	{
		return false;
	}

private:
	const IClassDefinition* definition_;
	std::string displayName_;
	std::string iconPath_;
};
}

ClassDefinitionModelNew::ClassDefinitionModelNew(const IClassDefinition* definition,
                                                 const IDefinitionManager& definitionManager)
	: definition_(definition)
{
	std::vector<IClassDefinition*> definitions;
	definitionManager.getDefinitionsOfType(definition, definitions);

	for (auto it = definitions.begin(); it != definitions.end(); ++it)
	{
		items_.emplace_back(new ClassDefinitionItem(*it));
	}
}

ClassDefinitionModelNew::~ClassDefinitionModelNew()
{
}

//------------------------------------------------------------------------------
void ClassDefinitionModelNew::iterateRoles(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : ClassDefinitionModelDetails::s_RolesVec)
	{
		iterFunc(role.c_str());
	}
}

//------------------------------------------------------------------------------
std::vector<std::string> ClassDefinitionModelNew::roles() const
{
	return ClassDefinitionModelDetails::s_RolesVec;
}

//------------------------------------------------------------------------------
AbstractItem* ClassDefinitionModelNew::item(int row) const /* override */
{
	TF_ASSERT(row >= 0);
	const auto index = static_cast<std::vector<AbstractItem*>::size_type>(row);
	TF_ASSERT(index < items_.size());
	return items_.at(index).get();
}

int ClassDefinitionModelNew::index(const AbstractItem* item) const /* override */
{
	auto it = std::find_if(items_.cbegin(), items_.cend(),
	                       [item](const std::unique_ptr<AbstractItem>& next) { return next.get() == item; });
	TF_ASSERT(it != items_.cend());
	return static_cast<int>(std::distance(items_.cbegin(), it));
}

int ClassDefinitionModelNew::rowCount() const /* override */
{
	return static_cast<int>(items_.size());
}

int ClassDefinitionModelNew::columnCount() const /* override */
{
	return 1;
}

const IClassDefinition* ClassDefinitionModelNew::definition() const
{
	return definition_;
}
} // end namespace wgt
