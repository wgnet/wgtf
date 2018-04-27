#include "qt_tool_bar.hpp"

#include "core_common/assert.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <assert.h>
#include <algorithm>

namespace wgt
{
static const auto DropDownStyle = "\
QToolButton[popupMode=\"2\"] {\
	height: 24px;\
	vertical-align: middle;\
	padding-right: 11px; /* make way for the arrow */\
}\
QToolButton::menu-indicator {\
	height: 22px;\
}";

QtToolBar::QtToolBar(QToolBar& qToolBar, const char* windowId) : QtMenu(qToolBar, windowId), qToolBar_(qToolBar)
{
	qToolBar_.setVisible(false);
}

void QtToolBar::update()
{
	QtMenu::update();
	if (!allInvisible_ && !qToolBar_.actions().empty())
	{
		qToolBar_.setVisible(true);
	}
	else
	{
		qToolBar_.setVisible(false);
	}
}

void QtToolBar::addAction(IAction& action, const char* path)
{
	auto qAction = getQAction(action);
	if (qAction == nullptr)
	{
		qAction = createQAction(action);
	}
	TF_ASSERT(qAction != nullptr);

	// Support DrowDown using "." separator
	auto endPath = path + strlen(path);
	auto separator = std::find(path, endPath, '.');
	if(separator != endPath)
	{
		auto widgetName = QString(path) + "Widget";
		auto menuPath = QString::fromLocal8Bit(separator + 1, endPath - separator);
		auto toolButton = qToolBar_.findChild<QToolButton*>(path);
		auto menu = qToolBar_.findChild<QMenu*>(menuPath);
		auto toolWidget = qToolBar_.findChild<QAction*>(widgetName);
		if (menu == nullptr)
		{
			toolButton = new QToolButton(&qToolBar_);
			toolButton->setObjectName(path);
			menu = new QMenu(menuPath, toolButton);
			toolButton->setMenu(menu);
			toolButton->setPopupMode( QToolButton::InstantPopup );
			toolButton->setStyleSheet(DropDownStyle);
			menu->setObjectName(menuPath);
			toolWidget = qToolBar_.addWidget(toolButton);
			toolWidget->setObjectName(widgetName);
			toolWidget->setVisible(action.visible());
		}
		menu->addAction(qAction);
		QObject::connect(qAction, &QAction::changed, [toolButton, toolWidget, qAction]()
		{
			if(qAction->isChecked())
			{
				toolButton->setDefaultAction(qAction);
			}
			// If visibility of an action changes update the widget's visibility
			if (qAction->isVisible() != toolWidget->isVisible())
			{
				auto actions = toolButton->menu()->actions();
				auto isVisible = std::any_of(actions.begin(), actions.end(), [](QAction* childAction){
					return childAction->isVisible();
				});
				toolWidget->setVisible(isVisible);
			}
		});
		if(action.checked())
		{
			toolButton->setDefaultAction(qAction);
		}
		// Does the new action's visibility match the widget's visibility
		if (action.visible() != toolWidget->isVisible())
		{
			// If the action is visible then the widget should be visible too
			if(action.visible())
			{
				toolWidget->setVisible(action.visible());
			}
		}
	}
	else
	{
		// TODO: deal with nested tool bars
		qToolBar_.addAction(qAction);
	}

	qToolBar_.setVisible(true);
}

void QtToolBar::removeAction(IAction& action)
{
	auto qAction = getQAction(action);
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action '%s' '%s' does not exist\n", action.text(),
		              StringUtils::join(action.paths(), ';').c_str());
		return;
	}

	qToolBar_.removeAction(qAction);
	if (qToolBar_.actions().empty())
	{
		qToolBar_.setVisible(false);
	}

	destroyQAction(action);
}
} // end namespace wgt
