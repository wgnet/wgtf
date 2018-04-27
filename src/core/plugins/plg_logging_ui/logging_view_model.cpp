#include "logging_view_model.hpp"
#include "logging_data_model.hpp"
#include "metadata/logging_view_model.mpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
bool LoggingViewModel::initialise(IComponentContext& componentContext)
{
	auto defManager = componentContext.queryInterface<IDefinitionManager>();
	if (defManager == nullptr)
	{
		return false;
	}

    dataModel_ = ManagedObject<LoggingDataModel>::make(componentContext);
	return true;
}

ObjectHandleT<LoggingDataModel> LoggingViewModel::getModel() const
{
    return dataModel_.getHandleT();
}

} // end namespace wgt
