#include "wg_action.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "helpers/qt_helpers.hpp"

#include <QQmlEngine>
#include <QQmlContext>

namespace wgt
{
struct WGAction::Implementation : Depends<IQtHelpers, IUIApplication, IUIFramework>
{
	Implementation(WGAction* self)
	    // TODO: This is extremely wasteful as we don't need per instance lambdas
	    : func_([self](IAction*) { emit self->triggered(); }),
	      enabledFunc_([self](const IAction*) { return self->getEnabled(); }),
	      checkedFunc_([self](const IAction*) { return self->getChecked(); }), checkable_(false), checked_(false),
	      enabled_(true), visible_(true), self_(self), active_(false), separator_(false), componentComplete_(false)
	{
	}

	~Implementation()
	{
		destroyAction();
	}

	void onComponentComplete()
	{
		componentComplete_ = true;
		createAction();
	}

	bool getActive() const
	{
		return active_;
	}

	void setActive(bool active)
	{
		if (active_ == active)
		{
			return;
		}

		active_ = active;

		auto uiApplication = get<IUIApplication>();
		if (uiApplication == nullptr || action_ == nullptr || visible_ == false)
		{
			return;
		}

		active_ ? uiApplication->addAction(*action_) : uiApplication->removeAction(*action_);
	}

	QString getActionId() const
	{
		return actionId_.c_str();
	}

	void setActionId(const QString& actionId)
	{
		destroyAction();
		actionId_ = actionId.toUtf8().data();
		createAction();
	}

	QString getActionText() const
	{
		return actionText_.c_str();
	}

	void setActionText(const QString& actionText)
	{
		destroyAction();
		actionText_ = actionText.toUtf8().data();
		createAction();
	}

	QString getActionPath() const
	{
		return actionPath_.c_str();
	}

	void setActionPath(const QString& actionPath)
	{
		destroyAction();
		actionPath_ = actionPath.toUtf8().data();
		createAction();
	}

	bool getCheckable() const
	{
		return checkable_;
	}

	void setCheckable(bool checkable)
	{
		destroyAction();
		checkable_ = checkable;
		createAction();
	}

	bool getChecked() const
	{
		return checked_;
	}

	void setChecked(bool checked)
	{
		destroyAction();
		checked_ = checked;
		createAction();
	}

	bool getEnabled() const
	{
		return enabled_;
	}

	void setEnabled(bool enabled)
	{
		enabled_ = enabled;
	}

	bool getVisible() const
	{
		return visible_;
	}

	bool getSeparator() const
	{
		return separator_;
	}

	void setSeparator(bool separator)
	{
		separator_ = separator;
	}

	void setVisible(bool visible)
	{
		if (visible_ == visible)
		{
			return;
		}

		visible_ = visible;

		auto uiApplication = get<IUIApplication>();
		if (uiApplication == nullptr || action_ == nullptr || active_ == false)
		{
			return;
		}

		visible_ ? uiApplication->addAction(*action_) : uiApplication->removeAction(*action_);
	}

	QVariant data()
	{
		if (action_ && self_)
		{
			auto qdata = get<IQtHelpers>()->toQVariant(action_->getData(), this->self_);
			return qdata;
		}

		return QVariant();
	}

	void setData(const QVariant& qdata)
	{
		if (action_)
		{
			auto data = get<IQtHelpers>()->toVariant(qdata);
			action_->setData(data);
			emit self_->dataChanged();
		}
	}

	void createAction()
	{
		if (componentComplete_ == false)
		{
			return;
		}

		auto uiFramework = get<IUIFramework>();
		if (uiFramework == nullptr)
		{
			return;
		}

		if (actionId_.empty() && getSeparator())
		{
			action_ = uiFramework->createAction("", "", actionPath_.c_str(), nullptr, enabledFunc_, nullptr);
		}
		else if (actionPath_.empty() || actionText_.empty())
		{
			action_ =
			uiFramework->createAction(actionId_.c_str(), func_, enabledFunc_, checkable_ ? checkedFunc_ : nullptr);
		}
		else
		{
			action_ = uiFramework->createAction(actionId_.c_str(), actionText_.c_str(), actionPath_.c_str(), func_,
			                                    enabledFunc_, checkable_ ? checkedFunc_ : nullptr);
		}

		auto uiApplication = get<IUIApplication>();
		if (action_ == nullptr || uiApplication == nullptr || active_ == false || visible_ == false)
		{
			return;
		}

		uiApplication->addAction(*action_);
	}

	void destroyAction()
	{
		if (action_ == nullptr)
		{
			return;
		}

		auto uiApplication = get<IUIApplication>();
		if (uiApplication != nullptr && active_ == true && visible_ == true)
		{
			uiApplication->removeAction(*action_);
		}

		action_.reset();
	}

	WGAction* self_;
	std::function<void(IAction*)> func_;
	std::function<bool(const IAction*)> enabledFunc_;
	std::function<bool(const IAction*)> checkedFunc_;
	bool checkable_ : 1;
	bool checked_ : 1;
	bool enabled_ : 1;
	bool visible_ : 1;
	bool active_ : 1;
	bool separator_ : 1;
	bool componentComplete_ : 1;
	std::string actionId_;
	std::string actionText_;
	std::string actionPath_;
	std::unique_ptr<IAction> action_;
};

WGAction::WGAction(QQuickItem* parent) : QQuickItem(parent)
{
	impl_.reset(new Implementation(this));
}

WGAction::~WGAction()
{
}

void WGAction::componentComplete()
{
	QQuickItem::componentComplete();
	impl_->onComponentComplete();
}

bool WGAction::getActive() const
{
	return impl_->getActive();
}

void WGAction::setActive(bool active)
{
	impl_->setActive(active);
}

QString WGAction::getActionId() const
{
	return impl_->getActionId();
}

void WGAction::setActionId(const QString& actionId)
{
	impl_->setActionId(actionId);
}

QString WGAction::getActionText() const
{
	return impl_->getActionText();
}

void WGAction::setActionText(const QString& actionId)
{
	impl_->setActionText(actionId);
}

QString WGAction::getActionPath() const
{
	return impl_->getActionPath();
}

void WGAction::setActionPath(const QString& actionId)
{
	impl_->setActionPath(actionId);
}

bool WGAction::getCheckable() const
{
	return impl_->getCheckable();
}

void WGAction::setCheckable(bool checkable)
{
	impl_->setCheckable(checkable);
}

bool WGAction::getChecked() const
{
	return impl_->getChecked();
}

void WGAction::setChecked(bool checked)
{
	impl_->setChecked(checked);
}

bool WGAction::getEnabled() const
{
	return impl_->getEnabled();
}

void WGAction::setEnabled(bool enabled)
{
	impl_->setEnabled(enabled);
}

bool WGAction::getVisible() const
{
	return impl_->getVisible();
}

bool WGAction::getSeparator() const
{
	return impl_->getSeparator();
}

void WGAction::setSeparator(bool separator)
{
	impl_->setSeparator(separator);
}

QVariant WGAction::data() const
{
	return impl_->data();
}

void WGAction::setData(const QVariant& userData)
{
	impl_->setData(userData);
}

void WGAction::setVisible(bool visible)
{
	impl_->setVisible(visible);
}
} // end namespace wgt
