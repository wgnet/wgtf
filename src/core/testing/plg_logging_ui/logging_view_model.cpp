#include "logging_view_model.hpp"
#include "logging_data_model.hpp"
#include "metadata/logging_view_model.mpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{

bool LoggingViewModel::initialise( IComponentContext& componentContext )
{
	auto defManager = componentContext.queryInterface< IDefinitionManager >();
	if (defManager == nullptr)
	{
		return false;
	}

	dataModel_ = std::shared_ptr< LoggingDataModel >( new LoggingDataModel() );

	return true;
}

std::shared_ptr< ILoggingModel > LoggingViewModel::getModel() const
{
	return dataModel_;
}

} // end namespace wgt
