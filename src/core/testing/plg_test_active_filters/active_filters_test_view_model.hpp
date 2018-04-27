#ifndef ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP
#define ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include <memory>

namespace wgt
{
class IDefinitionManager;
class IUIFramework;

//------------------------------------------------------------------------------

class ActiveFiltersTestViewModel
{
	DECLARE_REFLECTED

public:
	ActiveFiltersTestViewModel();
	~ActiveFiltersTestViewModel();

	void init(IDefinitionManager& defManager, IUIFramework& uiFramework);

	AbstractTreeModel* getSampleDataToFilter() const;

private:
	struct Implementation;
	Implementation* impl_;
};
} // end namespace wgt
#endif // ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP
