#pragma once
#ifndef LOGGING_PANEL_HPP
#define LOGGING_PANEL_HPP

#include "core_reflection/reflected_object.hpp"

namespace wgt
{
class ILoggingModel;
class LoggingDataModel;
class IComponentContext;

class LoggingViewModel
{
	DECLARE_REFLECTED

public:
	bool initialise(IComponentContext& componentContext);
	std::shared_ptr<ILoggingModel> getModel() const;

private:
	std::shared_ptr<LoggingDataModel> dataModel_;
};
} // end namespace wgt

#endif // LOGGING_PANEL_HPP