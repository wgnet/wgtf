#include "qt_view.hpp"

#include "i_qt_framework.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_common/assert.hpp"
#include "core_logging/logging.hpp"
#include "helpers/qt_helpers.hpp"
#include "qt_context_menu.hpp"

#include <QUiLoader>
#include <QVariantMap>
#include <QMenu>

namespace wgt
{
struct QtView::Implementation: Depends<IQtFramework>
{
	CursorId cursorId_ = ArrowCursor;
	std::vector<std::unique_ptr<IMenu>> menus_;
};

QtView::QtView(const char* id, QIODevice& source)
	: QtViewCommon(id), impl_(std::make_unique<Implementation>())
{
	QUiLoader loader;

	// Load optional plugins that may have custom widgets
	auto& pluginPath = impl_->get<IQtFramework>()->getPluginPath();

	if (!pluginPath.empty())
	{
		loader.addPluginPath(pluginPath.c_str());
	}

	setWidget(qobject_cast<QWidget*>(loader.load(&source)));

	if (widget() == nullptr)
	{
		NGT_ERROR_MSG(loader.errorString().toUtf8().constData());
		return;
	}

	initialise();
}

QtView::~QtView()
{
}

void QtView::initialise()
{
	auto viewWidget = widget();
	QtViewCommon::initialise(viewWidget);

	auto property = viewWidget->property("layoutHints");
	auto& hint = this->hint();
	hint.clear();

	if (property.isValid())
	{
		setLayoutHint(property.toStringList());
	}

	auto contextMenus = QtHelpers::getChildren<QMenu>(*viewWidget);

	for (auto& menu : contextMenus)
	{
		if (menu->property("path").isValid())
		{
			impl_->menus_.emplace_back(new QtContextMenu(*menu, viewWidget, windowId()));
		}
	}
}

void QtView::update()
{
	for (auto& menu : impl_->menus_)
	{
		menu->update();
	}
}

void QtView::reload()
{
	NGT_WARNING_MSG("QtView Reload not supported");
}

std::vector<IMenu*> QtView::menus() const
{
	std::vector<IMenu*> menus;

	for(auto& menu : impl_->menus_)
	{
		menus.push_back(menu.get());
	}

	return menus;
}

void QtView::setFocus(bool focus)
{
	QtViewCommon::setFocus(focus);
	auto view = widget();

	if (focus == view->hasFocus())
	{
		return;
	}

	focus ? view->setFocus() : view->clearFocus();
}

CursorId QtView::getCursor() const
{
	if(impl_->cursorId_ != ArrowCursor)
	{
		return impl_->cursorId_;
	}

	auto widget = this->widget();
	TF_ASSERT(widget->cursor().shape() <= LastCursor.id());
	return CursorId::make(widget->cursor().shape());
}

void QtView::setCursor(CursorId cursorId)
{
	impl_->cursorId_ = cursorId;
	auto widget = this->widget();
	if(widget == nullptr)
	{
		return;
	}

	if(cursorId.id() > LastCursor.id())
	{
		auto qCursor = reinterpret_cast<QCursor*>(cursorId.nativeCursor());
		widget->setCursor(*qCursor);
	}
	else
	{
		auto shape = static_cast<Qt::CursorShape>(cursorId.id());
		widget->setCursor(shape);
	}
}

void QtView::unsetCursor()
{
	impl_->cursorId_ = ArrowCursor;
	widget()->unsetCursor();
}

void QtView::setLayoutHint(const QStringList& tags)
{
	LayoutHint& layoutHint = this->hint();
	layoutHint.clear();

	for (auto it = tags.cbegin(); it != tags.cend(); ++it)
	{
		// value of string list should be like "hint string:float number"
		std::string hint(it->toUtf8().constData());
		auto split = StringUtils::split(hint, ':');
		layoutHint += LayoutHint(split[0].c_str(), atof(split[1].c_str()));
	}
}

void QtView::setWindowId(const char* windowId)
{
	QtViewCommon::setWindowId(windowId);
}
}