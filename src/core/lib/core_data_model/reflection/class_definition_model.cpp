#include "class_definition_model.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
namespace
{
class ClassDefinitionItem : public IItem
{
public:
	ClassDefinitionItem(const IClassDefinition* definition) : definition_(definition)
	{
	}

	const char* getDisplayText(int column) const
	{
		return definition_->getName();
	}

	ThumbnailData getThumbnail(int column) const
	{
		return nullptr;
	}

	Variant getData(int column, ItemRole::Id roleId) const
	{
		if (roleId == ValueRole::roleId_)
		{
			return const_cast<IClassDefinition*>(definition_);
		}
		else if (roleId == IndexPathRole::roleId_)
		{
			return definition_->getName();
		}
		return Variant();
	}

	bool setData(int column, ItemRole::Id roleId, const Variant& data)
	{
		return false;
	}

private:
	const IClassDefinition* definition_;
};
}

ClassDefinitionModel::ClassDefinitionModel(const IClassDefinition* definition,
                                           const IDefinitionManager& definitionManager)
{
	std::vector<IClassDefinition*> definitions;
	definitionManager.getDefinitionsOfType(definition, definitions);

	for (auto it = definitions.begin(); it != definitions.end(); ++it)
	{
		items_.push_back(new ClassDefinitionItem(*it));
	}
}

ClassDefinitionModel::~ClassDefinitionModel()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
	{
		delete *it;
	}
}

IItem* ClassDefinitionModel::item(size_t index) const
{
	TF_ASSERT(index < items_.size());
	return items_[index];
}

size_t ClassDefinitionModel::index(const IItem* item) const
{
	auto it = std::find(items_.begin(), items_.end(), item);
	TF_ASSERT(it != items_.end());
	return it - items_.begin();
}

bool ClassDefinitionModel::empty() const
{
	return items_.empty();
}

size_t ClassDefinitionModel::size() const
{
	return items_.size();
}

int ClassDefinitionModel::columnCount() const
{
	return 1;
}
} // end namespace wgt
