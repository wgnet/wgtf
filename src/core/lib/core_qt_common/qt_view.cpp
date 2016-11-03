#include "qt_view.hpp"
#include "i_qt_framework.hpp"
#include <QUiLoader>
#include <QVariantMap>

namespace wgt
{
QtView::QtView(const char* id, IQtFramework& qtFramework, QIODevice& source)
    : id_(id)
    , qtFramework_(qtFramework)
    , widgetView_(nullptr)
    , released_(false)
{
	QUiLoader loader;

	// Load optional plugins that may have custom widgets
	auto& pluginPath = qtFramework.getPluginPath();
	if (!pluginPath.empty())
		loader.addPluginPath(pluginPath.c_str());

	widgetView_ = qobject_cast<QWidget*>(loader.load(&source));

	if (widgetView_ == nullptr)
	{
		return;
	}
	init();
}

QtView::~QtView()
{
	if (!released_)
	{
		delete widgetView_;
	}
}

void QtView::init()
{
	auto hintsProperty = widgetView_->property("layoutHints");
	hint_.clear();

	if (hintsProperty.isValid())
	{
		auto tags = hintsProperty.toStringList();
		std::string key;
		for (auto it = tags.cbegin(); it != tags.cend(); ++it)
		{
			// value of string list should be like "hint string:float number"
			auto tmp = std::string(it->toUtf8());
			char* pch = nullptr;
			pch = strtok(const_cast<char*>(tmp.c_str()), ":");
			key = pch;
			pch = strtok(nullptr, ":");
			float v = static_cast<float>(atof(pch));
			hint_ += LayoutHint(key.c_str(), v);
		}
	}

	QVariant windowProperty = widgetView_->property("windowId");
	windowId_ = "";
	if (windowProperty.isValid())
	{
		windowId_ = windowProperty.toString().toUtf8().data();
	}

	QVariant titleProperty = widgetView_->property("title");
	title_ = "";
	if (titleProperty.isValid())
	{
		title_ = titleProperty.toString().toUtf8().data();
	}
}

const char* QtView::id() const
{
	return id_.c_str();
}

const char* QtView::title() const
{
	return title_.c_str();
}

const char* QtView::windowId() const
{
	return windowId_.c_str();
}

const LayoutHint& QtView::hint() const
{
	return hint_;
}

QWidget* QtView::releaseView()
{
	released_ = true;
	return view();
}

void QtView::retainView()
{
	released_ = false;
	widgetView_->setParent(nullptr);
}

QWidget* QtView::view() const
{
	return widgetView_;
}

void QtView::update()
{
}

void QtView::focusInEvent()
{
	for (auto& l : listeners_)
	{
		l->onFocusIn(this);
	}

	widgetView_->setFocus();
}

void QtView::focusOutEvent()
{
	for (auto& l : listeners_)
	{
		l->onFocusOut(this);
	}
}

void QtView::registerListener(IViewEventListener* listener)
{
	assert(std::find(listeners_.begin(), listeners_.end(), listener) == listeners_.end());
	listeners_.push_back(listener);
}

void QtView::deregisterListener(IViewEventListener* listener)
{
	auto it = std::find(listeners_.begin(), listeners_.end(), listener);
	assert(it != listeners_.end());
	listeners_.erase(it);
}
}