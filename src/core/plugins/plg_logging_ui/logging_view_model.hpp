#pragma once
#ifndef LOGGING_PANEL_HPP
#define LOGGING_PANEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_object/i_object_manager.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
class LoggingDataModel;
class IComponentContext;

class LoggingViewModel : public Depends<IObjectManager>
{
	DECLARE_REFLECTED

public:
	bool initialise(IComponentContext& componentContext);
    ObjectHandleT<LoggingDataModel> getModel() const;

private:
	ManagedObject<LoggingDataModel> dataModel_;
};
} // end namespace wgt

#endif // LOGGING_PANEL_HPP