#include "qt_menu.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include "core_common/assert.hpp"
#include "core_common/signal.hpp"
#include "core_string_utils/string_utils.hpp"
#include <QApplication>
#include <QAction>
#include <QObject>
#include <QString>
#include <QMenu>
#include <QPainter>
#include <QWindow>
#include <qevent.h>

namespace wgt
{

/*! 
	This class resolves ambiguous actions by forwarding to the action for current context
*/
class QForwardingAction : public QAction
{
public:
	QForwardingAction(const QString &text, QObject* parent)
		: QAction(text, parent)
	{
	}

	bool event(QEvent * e) override
	{
		if (e->type() == QEvent::Shortcut)
		{
			auto se = static_cast<QShortcutEvent *>(e);
			auto focusWidget = QApplication::focusWidget();
			if (se->isAmbiguous() && focusWidget != nullptr)
			{
				for (const auto& action : QtMenu::sharedQActions_)
				{
					auto qAction = action.second.lock();
					if(!qAction)
					{
						continue;
					}
					bool keyMatches = qAction->shortcut() == se->key();
					bool isContextual = qAction->shortcutContext() == Qt::WidgetShortcut;
					auto associatedWidgets = qAction->associatedWidgets();
					if (!keyMatches || !isContextual)
					{
						continue;
					}
					for(auto& widget : associatedWidgets)
					{
						if(widget == focusWidget && qAction->isEnabled() && qAction->isVisible())
						{
							qAction->trigger();
							return true;
						}
					}
				}
			}
		}
		return QAction::event(e);
	}
};

namespace QtMenu_Locals
{
const char* modeDisabled = "_disabled";
const char* modeActive = "_active";
const char* modeSelected = "_selected";
const char* stateOn = "_checked";

QIcon generateIcon(const char* iconPath)
{
	QIcon icon;

	QString normalPath(iconPath);
	auto tokens = normalPath.split('.');
	TF_ASSERT(tokens.length() == 2);

	QString off(tokens[0]);
	QString on(tokens[0] + stateOn);
	QString ext("." + tokens[1]);

	QString disabledPath(off + modeDisabled + ext);
	QString activePath(off + modeActive + ext);
	QString selectedPath(off + modeSelected + ext);

	QString normalOnPath(on + ext);
	QString disabledOnPath(on + modeDisabled + ext);
	QString activeOnPath(on + modeActive + ext);
	QString selectedOnPath(on + modeSelected + ext);

	auto offIcon = QPixmap(normalPath);
	icon.addPixmap(offIcon, QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(disabledPath), QIcon::Disabled, QIcon::Off);
	icon.addPixmap(QPixmap(activePath), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(selectedPath), QIcon::Selected, QIcon::Off);

	auto onIcon = QPixmap(normalOnPath);
	if (onIcon.isNull())
	{
		QPainter p;

		auto img = offIcon.toImage().convertToFormat(QImage::Format_ARGB32);
		auto mask(img);

		auto width = mask.width();
		auto height = mask.height();
		auto outerRect = mask.rect();
		auto innerRect = QRect(width / 16, height / 16, width - width / 8, height - height / 8);

		p.begin(&mask);
		p.setCompositionMode(QPainter::CompositionMode_SourceIn);
		p.fillRect(outerRect, QApplication::palette().highlight());
		p.fillRect(innerRect, Qt::transparent);
		p.end();

		p.begin(&img);
		p.setCompositionMode(QPainter::CompositionMode_SourceOver);
		p.drawImage(0, 0, mask);
		p.end();

		onIcon.convertFromImage(img);
	}
	icon.addPixmap(onIcon, QIcon::Normal, QIcon::On);
	icon.addPixmap(QPixmap(disabledOnPath), QIcon::Disabled, QIcon::On);
	icon.addPixmap(QPixmap(activeOnPath), QIcon::Active, QIcon::On);
	icon.addPixmap(QPixmap(selectedOnPath), QIcon::Selected, QIcon::On);

	return icon;
}
}

SharedActions QtMenu::sharedQActions_;

QtMenu::QtMenu(QObject& menu, const char* windowId)
	: menu_(menu), windowId_(windowId)
{
	allInvisible_ = true;
	auto pathProperty = menu_.property("path");
	if (pathProperty.isValid())
	{
		path_ = pathProperty.toString().toUtf8().operator const char*();
	}

	QMenu* qMenu = dynamic_cast<QMenu*>(&menu_);
	if (qMenu)
	{
		QObject::connect(qMenu, &QMenu::aboutToShow, [=]() { update(); });
	}
}

const char* QtMenu::path() const
{
	return path_.c_str();
}

const char* QtMenu::windowId() const
{
	return windowId_.c_str();
}

void QtMenu::update()
{
	allInvisible_ = true;
	for (auto& action : actions_)
	{
		bool visible = action.first->visible();
		allInvisible_ = !visible && allInvisible_;
		action.second->setVisible(visible);
		if(action.second->isVisible())
		{
			action.second->setEnabled(action.first->enabled());
			if (action.second->isCheckable())
			{
				action.second->setChecked(action.second->actionGroup() ?
					action.second->actionGroup()->checkedAction() == action.second.data() :
					action.first->checked());
			}
		}
	}
}

const char* QtMenu::relativePath(const char* path) const
{
	auto menuPath = this->path();
	if (path == nullptr || menuPath == nullptr)
	{
		return nullptr;
	}

	auto menuPathLen = strlen(menuPath);
	if (menuPathLen == 0)
	{
		return path;
	}

	// Action paths come in the format -
	//	minimum.path.to.match|.optional.path.to.match
	// We need to be able to match the action path -
	//	a.b.c|.d.e.f
	// To the menu path -
	//	a.b.c.d.e
	// To do this we split the action path into its minimum and optional components
	// and compare them to the appropriate subpaths of the menu path

	auto chr = strchr(path, '|');
	auto minPath = chr != nullptr ? path : nullptr;
	auto minPathLen = chr != nullptr ? static_cast<size_t>(chr - path) : 0;
	auto optPath = chr != nullptr ? chr + 1 : path;
	auto optPathLen = strlen(optPath);

	if (minPathLen > menuPathLen || strncmp(minPath, menuPath, minPathLen) != 0)
	{
		return nullptr;
	}

	if (minPathLen + optPathLen < menuPathLen || strncmp(optPath, menuPath + minPathLen, menuPathLen - minPathLen) != 0)
	{
		return nullptr;
	}

	optPath += menuPathLen - minPathLen;

	if (optPath[0] == '\0')
	{
		return optPath;
	}

	if (optPath[0] == '.')
	{
		return ++optPath;
	}

	return nullptr;
}

QAction* QtMenu::createQAction(IAction& action)
{
	auto qAction = getQAction(action);
	if (qAction != nullptr)
	{
		NGT_WARNING_MSG("Action %s already existing.\n", action.text());
		return nullptr;
	}

	actions_[&action] = createSharedQAction(action);
	qAction = getQAction(action);
	TF_ASSERT(qAction != nullptr);

	return qAction;
}

void QtMenu::destroyQAction(IAction& action)
{
	auto it = actions_.find(&action);
	if (it != actions_.end())
	{
		const std::string groupID(action.group());
		if (!groupID.empty())
		{
			auto groupItr = groups_.find(groupID);
			if (groupItr != groups_.end())
			{
				groupItr->second->removeAction(it->second.data());
				if (groupItr->second->actions().empty())
				{
					groups_.erase(groupItr);
				}
			}
		}
		actions_.erase(it);
	}

	auto findIt = connections_.find(&action);
	if (findIt != connections_.end())
	{
		connections_.erase(findIt);
	}
}

QAction* QtMenu::getQAction(IAction& action)
{
	auto it = actions_.find(&action);
	if (it != actions_.end())
	{
		return it->second.data();
	}
	return nullptr;
}

const Actions& QtMenu::getActions() const
{
	return actions_;
}

QMenu* QtMenu::addMenuPath(QMenu& qMenu, const char* path)
{
	QMenu* menu = &qMenu;
	while (path != nullptr)
	{
		auto tok = strchr(path, '.');
		auto subPath = tok != nullptr ? QString::fromUtf8(path, tok - path) : path;
		if (!subPath.isEmpty())
		{
			QMenu* subMenu = nullptr;
			auto actions = menu->actions();
			for (auto& action : actions)
			{
				subMenu = action->menu();
				if (subMenu != nullptr &&
					subMenu->objectName() == subPath)
				{
					break;
				}
				subMenu = nullptr;
			}

			if (subMenu == nullptr)
			{
				subMenu = menu->addMenu(subPath);
				subMenu->setObjectName(subPath);
			}
			menu = subMenu;
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}
	return menu;
}

void QtMenu::addMenuAction(QMenu& qMenu, QAction& qAction, const char* path)
{
	QMenu* menu = addMenuPath(qMenu, path);

	TF_ASSERT(menu != nullptr);
	auto order = qAction.property("order").toInt();

	auto actions = menu->actions();
	auto it = std::find_if(actions.begin(), actions.end(),
	                       [&](QAction* action) { return action->property("order").toInt() > order; });
	menu->insertAction(it == actions.end() ? nullptr : *it, &qAction);
}

void QtMenu::removeMenuAction(QMenu& qMenu, QAction& qAction)
{
	qMenu.removeAction(&qAction);
	auto children = qMenu.findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);
	for (auto& child : children)
	{
		removeMenuAction(*child, qAction);
		// we check actions.isEmpty rather than child.isEmpty as child.isEmpty doesnt include hidden actions
		if (child->actions().isEmpty())
		{
			auto childAction = child->menuAction();
			qMenu.removeAction(childAction);
			child->deleteLater();
		}
	}
}

void QtMenu::updateMenuVisibility(QMenu& qMenu)
{
	auto children = qMenu.findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);
	for (auto& child : children)
	{
		updateMenuVisibility(*child);
	}

	qMenu.menuAction()->setVisible(!qMenu.isEmpty());
}

QSharedPointer<QAction> QtMenu::createSharedQAction(IAction& action)
{
	auto qAction = getSharedQAction(action);
	if (qAction)
	{
		return qAction;
	}

	qAction.reset(new QForwardingAction(action.text(), QApplication::instance()), &QObject::deleteLater);
	sharedQActions_[&action] = qAction;

	qAction->setProperty("order", action.order());
	qAction->setEnabled(action.enabled());
	qAction->setVisible(action.visible());

	if (action.isSeparator())
	{
		qAction->setSeparator(true);
	}
	else
	{
		std::vector<QIcon> qIcons;
		auto icons = StringUtils::split(std::string(action.icon()), '|');
		for(auto& icon : icons)
		{			
			StringUtils::trim_string(icon);
			qIcons.push_back(QtMenu_Locals::generateIcon(icon.c_str()));
		}

		if(!qIcons.empty())
		{
			qAction->setIcon(qIcons[0]);
		}

		qAction->setShortcut(QKeySequence(action.shortcut()));

		if (action.isCheckable())
		{
			qAction->setCheckable(true);
			qAction->setChecked(action.checked());
		}

		QObject::connect(qAction.data(), &QAction::triggered, [&action]() {
			if (!action.enabled())
			{
				return;
			}
			action.execute();
		});

		connections_[&action] = action.signalShortcutChanged.connect([qAction](const char* shortcut) {
			TF_ASSERT(qAction != nullptr);
			qAction->setShortcut(QKeySequence(shortcut));
		});

		connections_[&action] = action.signalTextChanged.connect([qAction](const char* text) {
			TF_ASSERT(qAction != nullptr);
			qAction->setText(text);
		});

		connections_[&action] = action.signalVisibilityChanged.connect([qAction](bool visible) {
			TF_ASSERT(qAction != nullptr);
			qAction->setVisible(visible);
		});

		connections_[&action] = action.signalIconChanged.connect([qAction, qIcons](int index) {
			TF_ASSERT(qAction != nullptr);
			if(index >= 0 && index < (int)qIcons.size())
			{
				qAction->setIcon(qIcons[index]);
			}
		});

		const std::string groupID(action.group());
		if (!groupID.empty())
		{
			auto itr = groups_.find(groupID);
			if (itr == groups_.end())
			{
				groups_[groupID].reset(new QActionGroup(&menu_));
			}

			groups_.at(groupID)->addAction(qAction.data());
			TF_ASSERT(qAction->actionGroup());
		}
	}

	return qAction;
}

QSharedPointer<QAction> QtMenu::getSharedQAction(IAction& action)
{
	auto it = sharedQActions_.find(&action);
	if (it != sharedQActions_.end())
	{
		return it->second.lock();
	}
	return QSharedPointer<QAction>();
}
} // end namespace wgt
