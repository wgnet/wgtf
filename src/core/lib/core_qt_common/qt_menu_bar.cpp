#include "qt_menu_bar.hpp"

#include "core_common/assert.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_logging/logging.hpp"

#include <QStringList>
#include <QMenuBar>

namespace wgt
{
QtMenuBar::QtMenuBar(QMenuBar& qMenuBar, const char* windowId) 
	: QtMenu(qMenuBar, windowId)
	, qMenuBar_(qMenuBar)
{
	auto frontOrdering = qMenuBar.property("frontOrdering");
	if(frontOrdering.isValid())
	{
		frontMenus_ = frontOrdering.toStringList();
	}

	auto backOrdering = qMenuBar.property("backOrdering");
	if (backOrdering.isValid())
	{
		backMenus_ = backOrdering.toStringList();
	}
}

void QtMenuBar::update()
{
	QtMenu::update();

	auto menus = qMenuBar_.findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);
	for (auto& menu : menus)
	{
		updateMenuVisibility(*menu);
	}
}

void QtMenuBar::removeFromMenuBar(QMenu* menu)
{
	midMenus_.erase(std::find(midMenus_.begin(), 
		midMenus_.end(), menu->objectName()), midMenus_.end());
	delete menu;
}

QMenu* QtMenuBar::addMenuToBar(QString path)
{
	auto findMenu = [this](const QString& name) -> QMenu*
	{
		return qMenuBar_.findChild<QMenu*>(name, Qt::FindDirectChildrenOnly);
	};

	auto insertMenu = [this, &path](QMenu* nextMenu) -> QMenu*
	{
		auto menu = qMenuBar_.addMenu(path);
		menu->setObjectName(path);
		if(nextMenu != nullptr)
		{
			qMenuBar_.insertMenu(nextMenu->menuAction(), menu);
		}
		return menu;
	};

	auto frontItr = std::find(frontMenus_.begin(), frontMenus_.end(), path);
	if(frontItr != frontMenus_.end())
	{
		++frontItr;

		// Check for the next menu from the front menus after
		while (frontItr != frontMenus_.end())
		{
			if (auto nextMenu = findMenu(*frontItr))
			{
				return insertMenu(nextMenu);
			}
			++frontItr;
		}

		// Check for the next menu from the mid menus
		for(const auto& name : midMenus_)
		{
			if (auto nextMenu = findMenu(name))
			{
				return insertMenu(nextMenu);
			}
		}

		// Check for the next menu from the back menus
		for (const auto& name : backMenus_)
		{
			if (auto nextMenu = findMenu(name))
			{
				return insertMenu(nextMenu);
			}
		}

		// None found, add to the end
		return insertMenu(nullptr);
	}

	auto backItr = std::find(backMenus_.begin(), backMenus_.end(), path);
	if(backItr != backMenus_.end())
	{
		++backItr;

		// Check for the next menu from the back menus
		while (backItr != backMenus_.end())
		{
			if (auto nextMenu = findMenu(*backItr))
			{
				return insertMenu(nextMenu);
			}
			++backItr;
		}

		// None found, add to the end
		return insertMenu(nullptr);
	}

	// Is a middle menu, check for the next menu from the back menus
	midMenus_.push_back(path);
	for (const auto& name : backMenus_)
	{
		if (auto nextMenu = findMenu(name))
		{
			return insertMenu(nextMenu);
		}
	}
	return insertMenu(nullptr);
}

void QtMenuBar::addPath(const char* path)
{
	path = relativePath(path);
	auto tok = strchr(path, '.');
	auto menuPath = tok != nullptr ? QString::fromUtf8(path, tok - path) : path;
	QMenu* menu = qMenuBar_.findChild<QMenu*>(menuPath, Qt::FindDirectChildrenOnly);
	if (menu == nullptr)
	{
		menu = addMenuToBar(menuPath);
	}
	path = tok != nullptr ? tok + 1 : nullptr;

	QtMenu::addMenuPath(*menu, path);
}

void QtMenuBar::addAction(IAction& action, const char* path)
{
	auto qAction = getQAction(action);
	if (qAction == nullptr)
	{
		qAction = createQAction(action);
	}
	TF_ASSERT(qAction != nullptr);

	path = relativePath(path);
	if (path == nullptr || strlen(path) == 0)
	{
		path = action.text();
	}

	auto tok = strchr(path, '.');
	auto menuPath = tok != nullptr ? QString::fromUtf8(path, tok - path) : path;
	QMenu* menu = qMenuBar_.findChild<QMenu*>(menuPath, Qt::FindDirectChildrenOnly);
	if (menu == nullptr)
	{
		menu = addMenuToBar(menuPath);
	}
	path = tok != nullptr ? tok + 1 : nullptr;

	QtMenu::addMenuAction(*menu, *qAction, path);
	qMenuBar_.repaint();
}

void QtMenuBar::removeAction(IAction& action)
{
	auto qAction = getQAction(action);
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action '%s' '%s' does not exist\n", action.text(),
		              StringUtils::join(action.paths(), ';').c_str());
		return;
	}

	auto menus = qMenuBar_.findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);
	for (auto& menu : menus)
	{
		QtMenu::removeMenuAction(*menu, *qAction);
		// we check actions.isEmpty rather than menu->isEmpty as menu->isEmpty doesnt include hidden actions
		if (menu->actions().isEmpty())
		{
			removeFromMenuBar(menu);
		}
	}

	destroyQAction(action);
}
} // end namespace wgt
