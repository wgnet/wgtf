#include "active_filters_test_view_model.hpp"
#include "metadata/active_filters_test_view_model.mpp"
#include "core_data_model/simple_active_filters_model.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_data_model/i_item_role.hpp"
#include "testing/data_model_test/test_tree_model.hpp"

namespace wgt
{
//------------------------------------------------------------------------------
// Implementation (PIMPL)
//------------------------------------------------------------------------------
struct ActiveFiltersTestViewModel::Implementation
{
	Implementation(ActiveFiltersTestViewModel& self);
	void init(IDefinitionManager& defManager, IUIFramework& uiFramework);

	ActiveFiltersTestViewModel& self_;
	std::unique_ptr<IActiveFiltersModel> simpleActiveFiltersModel_;
	std::unique_ptr<AbstractTreeModel> sampleDataToFilter_;
};

ActiveFiltersTestViewModel::Implementation::Implementation(ActiveFiltersTestViewModel& self)
    : self_(self), simpleActiveFiltersModel_(nullptr), sampleDataToFilter_(new TestTreeModel())
{
}

void ActiveFiltersTestViewModel::Implementation::init(IDefinitionManager& defManager, IUIFramework& uiFramework)
{
	auto def = defManager.getDefinition<IActiveFiltersModel>();
	simpleActiveFiltersModel_.reset(new SimpleActiveFiltersModel("testActiveFilter", defManager, uiFramework));
}

//------------------------------------------------------------------------------
// View Model
// Data passed to the QML panel to be used by the control(s).
//------------------------------------------------------------------------------

ActiveFiltersTestViewModel::ActiveFiltersTestViewModel() : impl_(new Implementation(*this))
{
}

ActiveFiltersTestViewModel::~ActiveFiltersTestViewModel()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}

void ActiveFiltersTestViewModel::init(IDefinitionManager& defManager, IUIFramework& uiFramework)
{
	impl_->init(defManager, uiFramework);
}

IActiveFiltersModel* ActiveFiltersTestViewModel::getSimpleActiveFiltersModel() const
{
	return impl_->simpleActiveFiltersModel_.get();
}

AbstractTreeModel* ActiveFiltersTestViewModel::getSampleDataToFilter() const
{
	return impl_->sampleDataToFilter_.get();
}
} // end namespace wgt
