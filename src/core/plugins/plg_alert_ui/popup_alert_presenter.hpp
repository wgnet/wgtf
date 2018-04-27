#ifndef POPUP_ALERT_PRESENTER_HPP
#define POPUP_ALERT_PRESENTER_HPP

#include "core_logging_system/alerts/i_alert_presenter.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_object/managed_object.hpp"

class QQuickView;
class QQmlContext;
class QObject;

namespace wgt
{
class AlertPageModel;
class IAction;
class IComponentContext;

class PopupAlertPresenter : public IAlertPresenter, public Depends<IViewCreator>
{
public:
	explicit PopupAlertPresenter(IComponentContext& contextManager);
	virtual ~PopupAlertPresenter();

	virtual void show(const char* text);

	void addTestAlert(IAction* action);

private:
	mutable IComponentContext* contextManager_;
	wg_future<std::unique_ptr<IView>> alertWindow_;
    ManagedObject<AlertPageModel> alertPageModel_;

	std::unique_ptr<IAction> testAddAlert_;
	int alertCounter_;
};
} // end namespace wgt
#endif // POPUP_ALERT_PRESENTER_HPP
