#include "qt_context_menu.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"

#include <QMenu>
#include <assert.h>
#include <sstream>

namespace wgt
{
QtContextMenu::QtContextMenu(QMenu& qMenu, QWidget* qView, const char* windowId)
    : QtMenu(qMenu, windowId), qMenu_(qMenu), qView_(qView)
{
}

void QtContextMenu::addAction(IAction& action, const char* path)
{
	auto qAction = getQAction(action);
	if (qAction == nullptr)
	{
		qAction = createQAction(action);
		if (qAction == nullptr)
		{
			return;
		}

		qAction->setShortcutContext(Qt::WidgetShortcut);
	}

	QtMenu::addMenuAction(qMenu_, *qAction, relativePath(path));

	onPaletteChanged();

	if (qView_ != nullptr)
	{
		qView_->addAction(qAction);
	}
}

void QtContextMenu::onPaletteChanged()
{
	QColor highlightShade = qMenu_.palette().color(QPalette::Highlight);
	QColor textShade = qMenu_.palette().color(QPalette::Disabled, QPalette::Text);

	QString menuStyleSheet(
	"QMenu { background-color: palette(window); margin: 2px;}"
	"QMenu::item { padding: 2px 25px 2px 20px; border: 1px solid transparent;}"
	"QMenu::item:selected { border-color: palette(highlight); background: rgba(%1, %2, %3, 128); color: "
	"palette(highlighted-text);}"
	"QMenu::item:disabled { color: rgba(%4, %5, %6, %7);}"
	"QMenu::separator { height: 1px; background: palette(dark); margin-left: 10px; margin-right: 5px;}"
	"QMenu::indicator { width: 13px; height: 13px;}");

	menuStyleSheet = menuStyleSheet.arg(highlightShade.red()).arg(highlightShade.green()).arg(highlightShade.blue());
	menuStyleSheet =
	menuStyleSheet.arg(textShade.red()).arg(textShade.green()).arg(textShade.blue()).arg(textShade.alpha());

	qMenu_.setStyleSheet(menuStyleSheet);
}

void QtContextMenu::removeAction(IAction& action)
{
	auto qAction = getQAction(action);
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action '%s' '%s' does not exist\n", action.text(),
		              StringUtils::join(action.paths(), ';').c_str());
		return;
	}

	if (qView_ != nullptr)
	{
		qView_->removeAction(qAction);
	}

	QtMenu::removeMenuAction(qMenu_, *qAction);

	destroyQAction(action);
}
} // end namespace wgt
