#include "history_model.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_command_system/command_instance.hpp"
#include "core_command_system/command.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_variant/collection.hpp"

namespace wgt
{
namespace HistoryModel_Detail
{
class HistoryItem : public CollectionItem
{
public:
	HistoryItem(CollectionModel& model, size_t index) : CollectionItem(model, index)
	{
	}

	virtual Variant getData(int row, int column, ItemRole::Id roleId) const override
	{
		auto& collection = model_.getSource();
		if (row < 0 || row >= static_cast<int>(collection.size()))
		{
			return Variant();
		}

		if (roleId == ItemRole::valueId)
		{
			CommandInstancePtr ins;
			collection[row].tryCast(ins);
			TF_ASSERT(ins != nullptr);
			return ins->getCommandDescription();
		}

		return Variant();
	}

	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override
	{
		return false;
	}
};
} // end namespace CollectionModel_Detail

HistoryModel::HistoryModel()
{
}

HistoryModel::~HistoryModel()
{
}

AbstractItem* HistoryModel::item(int index) const
{
	// Do not create items past the end of the collection
	if (static_cast<size_t>(index) >= collection_.size())
	{
		return nullptr;
	}

	if (items_.size() <= (size_t)index)
	{
		items_.resize(index + 1);
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	item = new HistoryModel_Detail::HistoryItem(*const_cast<HistoryModel*>(this), index);
	items_[index] = std::unique_ptr<AbstractItem>(item);
	return item;
}
} // end namespace wgt
