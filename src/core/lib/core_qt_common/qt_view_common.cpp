#include "qt_view_common.hpp"
#include "i_qt_framework.hpp"

#include <QVariant>

namespace wgt
{
struct QtViewCommon::Implementation
{
	Implementation(const char* id);

	QWidget* widget_ = nullptr;
	std::string id_;
	std::string title_;
	std::string windowId_;
	LayoutHint hint_;
	bool released_ = false;
	bool focused_ = false;
	Signal<IView::BooleanCallbackSignature> focusChanged_;
	Signal<IView::CharStringCallbackSignature> titleChanged_;
};

QtViewCommon::Implementation::Implementation(const char* id)
	: id_(id)
{
}


QtViewCommon::QtViewCommon(const char* id)
	: impl_(std::make_unique<Implementation>(id))
{
}

QtViewCommon::~QtViewCommon()
{
	deleteWidget();
}

const char* QtViewCommon::id() const
{
	return impl_->id_.c_str();
}

const char* QtViewCommon::title() const
{
	return impl_->title_.c_str();
}

void QtViewCommon::title(const char* title)
{
	impl_->title_ = title;

	if (auto widgetView = widget())
	{
		widgetView->setProperty("title", title);
	}

	impl_->titleChanged_(this->title());
}

const char* QtViewCommon::windowId() const
{
	return impl_->windowId_.c_str();
}

const LayoutHint& QtViewCommon::hint() const
{
	return impl_->hint_;
}

uintptr_t QtViewCommon::getNativeWindowHandle()
{
	if (auto viewWidget = widget())
	{
		return viewWidget->winId();
	}

	return 0;
}

QWidget* QtViewCommon::releaseWidget()
{
	impl_->released_ = true;
	return widget();
}

void QtViewCommon::retainWidget()
{
	impl_->released_ = false;
	widget()->setParent(nullptr);
}

QWidget* QtViewCommon::widget() const
{
	return impl_->widget_;
}

bool QtViewCommon::focused()
{
	return impl_->focused_;
}

void QtViewCommon::setFocus(bool focus)
{
	if (focused() == focus)
	{
		return;
	}

	impl_->focused_ = focus;
	auto widget = this->widget();

	if (focus && !widget->isActiveWindow())
	{
		widget->activateWindow();
	}

	focusChanged();
}

void QtViewCommon::focusChanged()
{
	impl_->focusChanged_(focused());
}

Connection QtViewCommon::connectFocusChanged(const BooleanCallback& callback)
{
	return impl_->focusChanged_.connect(callback);
}

Connection QtViewCommon::connectTitleChanged(const CharStringCallback& callback)
{
	return impl_->titleChanged_.connect(callback);
}

void QtViewCommon::initialise(QObject* object)
{
	auto property = object->property("windowId");
	impl_->windowId_ = property.isValid() ? property.toString().toUtf8().data() : "";

	property = object->property("title");
	if (property.isValid())
	{
		this->title(property.toString().toUtf8().data());
	}
	else
	{
		this->title("");
	}
}

void QtViewCommon::setWidget(QWidget* widget)
{
	impl_->widget_ = widget;
}

void QtViewCommon::setWindowId(const char* id)
{
	if (id == nullptr)
	{
		return;
	}

	impl_->windowId_ = id;
}

LayoutHint& QtViewCommon::hint()
{
	return impl_->hint_;
}

void QtViewCommon::deleteWidget()
{
	if (!impl_->released_)
	{
		delete impl_->widget_;
		impl_->widget_ = nullptr;
	}
}
}