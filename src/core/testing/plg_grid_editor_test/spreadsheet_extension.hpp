#ifndef SPREADSHEET_EXTENSION_HPP
#define SPREADSHEET_EXTENSION_HPP

#include "core_data_model/i_model_extension.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/abstract_item_model.hpp"

#include <unordered_map>
#include <ctime>
#include <vector>

namespace wgt
{
class SpreadsheetExtension : public IModelExtension
{
	using ModelIndex = IModelExtension::ModelIndex;

public:
	SpreadsheetExtension();
	virtual ~SpreadsheetExtension();

	Variant data(const ModelIndex& index, ItemRole::Id roleId) const override;

	void onDataChanged(const ModelIndex& topLeft, const ModelIndex& bottomRight,
	                   const std::vector<ItemRole::Id>& roles) override;

	void commitData();

private:
	std::unordered_map<ModelIndex, time_t> lastEdits_;
	time_t commitTime_;
};
} // end namespace wgt
#endif // SPREADSHEET_EXTENSION_HPP
