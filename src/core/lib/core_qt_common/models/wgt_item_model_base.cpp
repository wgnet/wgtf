#include "wgt_item_model_base.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/interfaces/i_wgt_interface_provider.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
QueryHelper::QueryHelper()
	: currentIndex_(0)
{
}


//------------------------------------------------------------------------------
void QueryHelper::clear()
{
	itemModels_.clear();
}


//------------------------------------------------------------------------------
void QueryHelper::reset()
{
	currentIndex_ = 0;
}


//------------------------------------------------------------------------------
void QueryHelper::nextModel()
{
	currentIndex_++;
}


//------------------------------------------------------------------------------
void QueryHelper::pushModel(const IWgtItemModel & model)
{
	itemModels_.push_back(&model);
}


//------------------------------------------------------------------------------
const IWgtItemModel & QueryHelper::getModel() const
{
	return *itemModels_[currentIndex_];
}

//==============================================================================
bool WgtItemModelBase::canUse(QueryHelper & o_Helper) const 
{
	auto sourceModel = getSourceModel();
	if (sourceModel == nullptr)
	{
		return false;
	}
	auto interfaceProvider =
		IWGTInterfaceProvider::getInterfaceProvider(*sourceModel);
	if (interfaceProvider == nullptr)
	{
		return false;
	}
	auto wgtItemModel = interfaceProvider->queryInterface< IWgtItemModel >();
	if (wgtItemModel == nullptr)
	{
		return false;
	}
	o_Helper.pushModel(*wgtItemModel);
	return wgtItemModel->canUse( o_Helper );
}


Variant WgtItemModelBase::variantData(
	QueryHelper & helper,
	const QModelIndex& index, int role) const 
{
	auto sourceIndex = getSourceIndex( index );
	auto && model = helper.getModel();
	helper.nextModel();
	return model.variantData( helper, sourceIndex, role);
}


} //end namespace wgt