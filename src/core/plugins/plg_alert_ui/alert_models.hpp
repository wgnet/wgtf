#ifndef ALERT_MODELS_HPP
#define ALERT_MODELS_HPP

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/i_list_model.hpp"

namespace wgt
{
class AlertObjectModel
{
public:

	AlertObjectModel();
	AlertObjectModel( const AlertObjectModel& rhs );
	AlertObjectModel( const char* message );

	virtual ~AlertObjectModel();

	void init( const char* message );

	const char* getMessage() const;

private:

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

class AlertPageModel
{
public:

	AlertPageModel();
	AlertPageModel( const AlertPageModel& rhs );

	virtual ~AlertPageModel();

	void init( IComponentContext& contextManager );

	void addAlert( const char* message );

	ObjectHandle removeAlert() const;

	const int & currentSelectedRowIndex() const;
	void currentSelectedRowIndex( const int & index );

private:

	DECLARE_REFLECTED

	const IListModel * getAlerts() const;

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // ALERT_MODELS_HPP
