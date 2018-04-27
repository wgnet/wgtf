#include "qt_tab_region.hpp"

#include "i_qt_framework.hpp"
#include "qml_view.hpp"

#include "core_common/assert.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/interfaces/i_action_manager.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_dependency_system/depends.hpp"

#include <QTabWidget>
#include <QVariant>
#include <unordered_map>

namespace wgt
{
struct OnTabCloseFn: Depends<IActionManager>
{
	OnTabCloseFn(QTabWidget& widget, const std::string& id)
		: id_(id)
		, widget_(widget)
	{
	}

	void operator()(int index)
	{
		if (index >= 0)
		{
			if (widget_.currentIndex() != index)
			{
				widget_.setCurrentIndex(index);
			}

			auto actionManager = get<IActionManager>();
			TF_ASSERT(actionManager);
			auto action = actionManager->findAction(id_.c_str());
			if (action && action->enabled())
			{
				action->execute();
			}
		}
	}

	QTabWidget& widget_;
	const std::string id_;
};


struct QtTab
{
	QtTab(IView* view = nullptr);
	virtual ~QtTab();

	IView* view_;
	Connection focusChangedConnection_;
	Connection titleChangedConnection_;
};

QtTab::QtTab(IView* view)
	: view_(view)
{
}

QtTab::~QtTab()
{
	focusChangedConnection_.disconnect();
	titleChangedConnection_.disconnect();
}


struct QtTabRegion::Implementation: public QObject, public Depends<IQtFramework>
{
	Implementation(QtTabRegion& self, IWindow* window, QTabWidget& tabWidget);
	virtual ~Implementation();

	bool eventFilter(QObject* object, QEvent* event);
	void setFocus(bool focus);
	void addView(IView& view);
	void removeView(IView& view);
	QString generateTabTitle(int index, const char* text);

	QtTabRegion& self_;
	QTabWidget& tabWidget_;
	LayoutTags tags_;
	std::unordered_map<QWidget*, QtTab> tabs_;
	std::unordered_map<IView*, QWidget*> views_;
	std::vector<QMetaObject::Connection> connections_;
	QWidget* current_;
	IWindow* window_;
	bool active_;
};

QtTabRegion::Implementation::Implementation(QtTabRegion& self, IWindow* window, QTabWidget& tabWidget)
	: self_(self), window_(window), tabWidget_(tabWidget), current_(nullptr), active_(false)
{
	tabWidget_.setVisible(false);
	auto layoutTagsProperty = tabWidget_.property("layoutTags");

	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.cbegin(); it != tags.cend(); ++it)
		{
			tags_.tags_.push_back(std::string(it->toUtf8()));
		}
	}

	auto currentChangedFn = [this](int index)
	{
		if (index < 0)
		{
			if (!current_)
			{
				return;
			}

			tabs_[current_].view_->setFocus(false);
			current_ = nullptr;
			return;
		}

		current_ = tabWidget_.widget(index);
		IView* view = tabs_[current_].view_;

		if (QmlView* qmlView = dynamic_cast<QmlView*>(view))
		{
			qmlView->setNeedsToLoad(true);
		}

		view->setFocus(true);
	};

	connections_.push_back(QObject::connect(&tabWidget_, &QTabWidget::currentChanged, currentChangedFn));
	auto tabsCloseActionProperty = tabWidget_.property("tabsCloseAction");

	if (tabsCloseActionProperty.isValid())
	{
		const auto id = tabsCloseActionProperty.toString().toUtf8().constData();
		connections_.push_back(QObject::connect(&tabWidget_, &QTabWidget::tabCloseRequested, OnTabCloseFn(tabWidget_, id)));
	}

	tabWidget_.installEventFilter(this);
}

QtTabRegion::Implementation::~Implementation()
{
	tabWidget_.removeEventFilter(this);

	for (auto& connection : connections_)
	{
		QObject::disconnect(connection);
	}
}

bool QtTabRegion::Implementation::eventFilter(QObject* object, QEvent* event)
{
	switch (event->type())
	{
	case QEvent::WindowActivate:
		active_ = true;
		break;

	case QEvent::WindowDeactivate:
		active_ = false;
		break;

	case QEvent::FocusIn:
		setFocus(true);
		break;

	case QEvent::FocusOut:
		setFocus(false);
		break;

	case QEvent::ActivationChange:
		setFocus(active_);
		break;
	}

	return QObject::eventFilter(object, event);
}

void QtTabRegion::Implementation::setFocus(bool focus)
{
	if (current_ == nullptr || tabWidget_.indexOf(current_) < 0)
	{
		return;
	}

	tabs_[current_].view_->setFocus(focus);
}

void QtTabRegion::Implementation::addView(IView& view)
{
	// IView will not control qWidget's life-cycle after this call.
	auto qtFramework = get<IQtFramework>();
	TF_ASSERT(qtFramework != nullptr);
	auto widget = qtFramework->toQWidget(view);

	if (widget == nullptr)
	{
		return;
	}

	int index = tabWidget_.indexOf(widget);

	if (index != -1)
	{
		// already added into the tabWidget
		return;
	}

	tabs_.emplace(widget, &view);
	views_.emplace(&view, widget);
	index = tabWidget_.count();
	auto tabText = generateTabTitle(index, view.title());

	auto focusChangedCallback = [widget, this](bool focus)
	{
		auto& tab = tabs_[widget];
		auto view = tab.view_;

		if (!focus && window_->getFocusedView() == view)
		{
			window_->setFocusedView(nullptr);
			return;
		}

		if (!focus || window_->getFocusedView() == view)
		{
			return;
		}

		window_->setFocusedView(view);
		const int index = tabWidget_.indexOf(widget);

		if (tabWidget_.currentIndex() != index)
		{
			tabWidget_.setCurrentIndex(index);
		}
	};

	auto titleChangedCallback = [widget, this](const char* title)
	{
		const int index = tabWidget_.indexOf(widget);

		if (index >= 0)
		{
			tabWidget_.setTabText(index, generateTabTitle(index, title));
		}
	};

	Connection focusChangedConnection_ = view.connectFocusChanged(focusChangedCallback);
	Connection titleChangedConnection_ = view.connectTitleChanged(titleChangedCallback);
	tabWidget_.insertTab(index, widget, tabText);
	auto& tab = tabs_[widget];
	tab.focusChangedConnection_ = focusChangedConnection_;
	tab.titleChangedConnection_ = titleChangedConnection_;
	tabWidget_.setVisible(true);
}

void QtTabRegion::Implementation::removeView(IView& view)
{
	// IView will not control qWidget's life-cycle after this call.
	auto qtFramework = get<IQtFramework>();
	TF_ASSERT(qtFramework != nullptr);

	auto viewIterator = views_.find(&view);
	TF_ASSERT(viewIterator != views_.end());
	auto widget = viewIterator->second;
	auto tabIterator = tabs_.find(viewIterator->second);
	TF_ASSERT(tabIterator != tabs_.end());
	const int index = tabWidget_.indexOf(widget);

	if (current_ == widget)
	{
		view.setFocus(false);
		current_ = nullptr;
	}

	tabs_.erase(tabIterator);
	views_.erase(viewIterator);
	tabWidget_.removeTab(index);

	// call this function to let IView control the qWidget's life-cycle again.
	qtFramework->retainQWidget(view);
}

QString QtTabRegion::Implementation::generateTabTitle(int index, const char* text)
{
	if (text != nullptr && *text != 0)
	{
		return text;
	}

	std::string tabText = "New " + std::to_string(index);
	return tabText.c_str();
}


QtTabRegion::QtTabRegion(IWindow* window, QTabWidget& qTabWidget)
    : impl_(std::make_unique<Implementation>(*this, window, qTabWidget))
{
}

QtTabRegion::~QtTabRegion()
{
}

const LayoutTags& QtTabRegion::tags() const
{
	return impl_->tags_;
}

void QtTabRegion::addView(IView& view)
{
	impl_->addView(view);
}

void QtTabRegion::removeView(IView& view)
{
	impl_->removeView(view);
}
} // end namespace wgt
