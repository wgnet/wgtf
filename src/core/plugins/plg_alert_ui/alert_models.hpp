#ifndef ALERT_MODELS_HPP
#define ALERT_MODELS_HPP

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{
class AlertObjectModel
{
public:
	AlertObjectModel(const char* message);
    AlertObjectModel(const AlertObjectModel& rhs) = delete;
    AlertObjectModel& operator=(const AlertObjectModel& rhs) = delete;
    virtual ~AlertObjectModel() = default;

	const char* getMessage() const;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

class AlertPageModel
{
public:
	AlertPageModel();
    AlertPageModel(const AlertPageModel& rhs) = delete;
    AlertPageModel& operator=(const AlertPageModel& rhs) = delete;
	virtual ~AlertPageModel() = default;

	void addAlert(const char* message);
	ObjectHandle removeAlert() const;
	const int& currentSelectedRowIndex() const;
	void currentSelectedRowIndex(const int& index);

private:
	DECLARE_REFLECTED

	const AbstractListModel* getAlerts() const;

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // ALERT_MODELS_HPP
