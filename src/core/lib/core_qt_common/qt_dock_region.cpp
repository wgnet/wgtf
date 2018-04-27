#include "qt_dock_region.hpp"

#include "qt_window.hpp"
#include "i_qt_framework.hpp"
#include "core_common/assert.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "qml_view.hpp"
#include <QDockWidget>
#include <QLayout>
#include <QMainWindow>
#include <QVariant>
#include <QEvent>
#include <QCoreApplication>

namespace wgt
{
// This ugly class handles all the cases for switching between docked and floating DockWigets
// Unfortunately Qt does not provide generous solution for it
class NGTDockWidget : public QDockWidget, public Depends<IUIApplication>
{
public:
	NGTDockWidget(IView* view, IWindow* window)
	    : QDockWidget(view->title()), view_(view), active_(false), visible_(false), window_(window)
	{
		using namespace std::placeholders;
		connections_.push_back(view->connectFocusChanged(std::bind(&NGTDockWidget::onFocusChanged, this, _1)));
		connections_.push_back(view->connectTitleChanged(std::bind(&NGTDockWidget::onTitleChanged, this, _1)));
	}

	~NGTDockWidget()
	{
		for (auto& connection : connections_)
		{
			connection.disconnect();
		}

		connections_.clear();
		
		if (action_)
		{
			get<IUIApplication>()->removeAction(*action_);
			action_ = nullptr;
		}
	}

	void visibilityChanged(bool visible)
	{
		visible_ = visible;
		if (!visible_ || this->widget() == nullptr)
		{
			return;
		}
		QmlView* qmlView = dynamic_cast<QmlView*>(view_);
		if (qmlView != nullptr)
		{
			qmlView->setNeedsToLoad(true);
		}
	}

	bool getVisibility() const
	{
		return visible_;
	}

	void createAction(IQtFramework& framework, const std::string& menuPath)
	{
		const char* title = view_->title();
		std::string actionId("View.");
		actionId += title;

		auto executeFn = [this](IAction*) {
			if (isHidden())
			{
				show();
				raise();
				view_->setFocus(true);
			}
			else
			{
				hide();
				lower();
				view_->setFocus(false);
			}
		};

		auto enabledFn = [this](const IAction*) { return isEnabled(); };
		auto checkedFn = [this](const IAction*) { return !isHidden(); };
		auto visibleFn = [this](const IAction*) { return isActionVisible(); };

		action_ = framework.createAction(actionId.c_str(), title, menuPath.c_str(), executeFn, enabledFn, checkedFn, visibleFn);

		get<IUIApplication>()->addAction(*action_);
	}

protected:
	virtual bool event(QEvent* e) override
	{
		switch (e->type())
		{
		case QEvent::WindowActivate:
			active_ = true;
			break;

		case QEvent::WindowDeactivate:
			active_ = false;
			break;

		case QEvent::FocusIn:
			if (visible_)
			{
				view_->setFocus(true);
			}

			break;

		case QEvent::FocusOut:
			view_->setFocus(false);
			break;

		case QEvent::ActivationChange:
			if (isFloating())
			{
				view_->setFocus(active_);
			}

			break;
		}

		return QDockWidget::event(e);
	}

private:
	bool isActionVisible()
	{
		const char* title = view_->title();
		return title != nullptr && title[0] != '\0'; 
	}

	void onFocusChanged(bool focus)
	{
		if (focus)
		{
			window_->setFocusedView(view_);
			raise();
			return;
		}

		if (window_->getFocusedView() != view_)
		{
			return;
		}

		window_->setFocusedView(nullptr);
	}

	void onTitleChanged(const char* title)
	{
		setWindowTitle(title);
		action_->text(title);
		action_->signalVisibilityChanged(isActionVisible()); // Title determines visibility (see visibleFn callback).
	}

	IView* view_;
	IWindow* window_;
	std::unique_ptr<IAction> action_;
	bool active_;
	bool visible_;
	std::vector<Connection> connections_;
};

QtDockRegion::QtDockRegion(QtWindow& qtWindow, QDockWidget& qDockWidget)
    : qtWindow_(qtWindow), qDockWidget_(qDockWidget), hidden_(false)
{
	auto qMainWindow = qtWindow_.window();
	TF_ASSERT(qMainWindow != nullptr);

	// Walk our parent hierarchy and make sure we are tabified with the topmost dock widget.
	// Dock widgets as children of anything but the main window are not supported.
	// We support this in the Qt designer so that we can override the properties of a tab or collection of tabs within a
	// dock region
	QWidget* qWidget = &qDockWidget_;
	while (qWidget != nullptr)
	{
		qWidget = qWidget->parentWidget();
		if (qWidget == nullptr)
		{
			break;
		}
		auto qDockWidget = qobject_cast<QDockWidget*>(qWidget);
		if (qDockWidget != nullptr)
		{
			qMainWindow->tabifyDockWidget(qDockWidget, &qDockWidget_);
		}
	}
	qDockWidget_.setVisible(false);

	auto layoutTagsProperty = qDockWidget_.property("layoutTags");
	if (layoutTagsProperty.isValid())
	{
		auto tags = layoutTagsProperty.toStringList();
		for (auto it = tags.cbegin(); it != tags.cend(); ++it)
		{
			tags_.tags_.push_back(std::string(it->toUtf8()));
		}
	}

	auto hiddenProperty = qDockWidget_.property("hidden");
	if (hiddenProperty.isValid())
	{
		hidden_ = hiddenProperty.toBool();
	}

	auto menuPathProperty = qDockWidget_.property("menuPath");
	menuPath_ = menuPathProperty.isValid() ? menuPathProperty.toString().toUtf8().constData() : "View";

	QObject::connect(&qtWindow_, &QtWindow::windowReady, [&]() {
		if (needToRestorePreference_.empty())
		{
			return;
		}
		auto qtFramework = get<IQtFramework>();
		TF_ASSERT(qtFramework != nullptr);
		auto qMainWindow = qtWindow_.window();
		for (auto& it : needToRestorePreference_)
		{
			auto qtDockWidget = it.first;
			TF_ASSERT(qtDockWidget != nullptr);
			auto pView = it.second;
			TF_ASSERT(pView != nullptr);
			bool isOk = qMainWindow->restoreDockWidget(qtDockWidget);
			if (!isOk)
			{
				setDefaultPreferenceForDockWidget(qtDockWidget);
			}
			auto pQWidget = qtFramework->toQWidget(*pView);
			TF_ASSERT(pQWidget != nullptr);
			qtDockWidget->setWidget(pQWidget);
			QmlView* qmlView = dynamic_cast<QmlView*>(pView);
			auto ngtDockWidget = dynamic_cast<NGTDockWidget*>(qtDockWidget);
			if (ngtDockWidget && qmlView)
			{
				if (ngtDockWidget->isVisible() && ngtDockWidget->getVisibility())
				{
					qmlView->setNeedsToLoad(true);
				}
			}
		}
		needToRestorePreference_.clear();
	});
}

const LayoutTags& QtDockRegion::tags() const
{
	return tags_;
}

void QtDockRegion::restoreDockWidgets()
{
	auto qtFramework = get<IQtFramework>();
	TF_ASSERT(qtFramework != nullptr);
	auto qMainWindow = qtWindow_.window();
	TF_ASSERT(qMainWindow != nullptr);
	for (auto& it : dockWidgetMap_)
	{
		auto view = it.first;
		TF_ASSERT(view != nullptr);
		auto qDockWidget = it.second.get();
		TF_ASSERT(qDockWidget != nullptr);
		if (qtWindow_.isReady())
		{
			qMainWindow->restoreDockWidget(qDockWidget);
		}
		else
		{
			needToRestorePreference_.push_back(std::make_pair(qDockWidget, view));
		}
	}
}

void QtDockRegion::setDefaultPreferenceForDockWidget(QDockWidget* qDockWidget)
{
	auto qMainWindow = qtWindow_.window();
	TF_ASSERT(qMainWindow != nullptr);
	qDockWidget->setVisible(!hidden_);
	if(qDockWidget_.isFloating())
	{
		qDockWidget->setFloating(true);
		auto savedGeometry = getWidgetPersistentGeometry(qDockWidget->objectName().toUtf8().constData());
		if(savedGeometry != nullptr)
		{
			qDockWidget->restoreGeometry(*savedGeometry);
		}
	}
	else
	{
		qMainWindow->tabifyDockWidget(&qDockWidget_, qDockWidget);
		auto tabifiedWidgets = qMainWindow->tabifiedDockWidgets(&qDockWidget_);
		if(!tabifiedWidgets.contains(qDockWidget))
		{
			// Tabify failed because the default widget isn't part of the main window. qDockWidget will appear
			// improperly in the mainwindow if we do nothing, so we float the widget instead.
			// This isn't ideal, but seems to be the best we can do without modifying and recompiling Qt.
			qDockWidget->setFloating(true);
			auto savedGeometry = getWidgetPersistentGeometry(qDockWidget->objectName().toUtf8().constData());;
			if(savedGeometry != nullptr)
			{
				qDockWidget->restoreGeometry(*savedGeometry);
			}
		}
	}
	qDockWidget->setFeatures(qDockWidget_.features());
	qDockWidget->setAllowedAreas(qDockWidget_.allowedAreas());
}

const QByteArray* QtDockRegion::getWidgetPersistentGeometry(const char* widgetName) const
{
	auto it = persistentWidgetGeometry_.find(widgetName);
	if(it != persistentWidgetGeometry_.end())
	{
		return &it->second;
	}
	return nullptr;
}

void QtDockRegion::setWidgetPersistentGeometry(const char* widgetName, const QByteArray& geometry)
{
	persistentWidgetGeometry_[widgetName] = geometry;
}

void QtDockRegion::addView(IView& view)
{
	auto qMainWindow = qtWindow_.window();
	TF_ASSERT(qMainWindow != nullptr);

	auto findIt = dockWidgetMap_.find(&view);
	if (findIt != dockWidgetMap_.end())
	{
		// already added into the dockWidget
		return;
	}
	// IView will not control qWidget's life-cycle after this call.
	auto qtFramework = get<IQtFramework>();
	TF_ASSERT(qtFramework != nullptr);
	auto qWidget = qtFramework->toQWidget(view);
	if (qWidget == nullptr)
	{
		return;
	}
	auto centralWidget = qMainWindow->centralWidget();
	if (centralWidget != nullptr)
	{
		centralWidget->layout()->addWidget(qWidget);
	}
	qWidget->setSizePolicy(qDockWidget_.sizePolicy());
	qWidget->setMinimumSize(qDockWidget_.minimumSize());
	qWidget->setMaximumSize(qDockWidget_.maximumSize());
	qWidget->setSizeIncrement(qDockWidget_.sizeIncrement());
	qWidget->setBaseSize(qDockWidget_.baseSize());
	qWidget->resize(qWidget->baseSize());

	auto qDockWidget = new NGTDockWidget(&view, &qtWindow_);
	qDockWidget->setObjectName(view.id());
	IView* pView = &view;
	QtWindow* pWindow = &qtWindow_;
	QObject::connect(qDockWidget, &QDockWidget::visibilityChanged, [qDockWidget, pWindow](bool visible) {
		qDockWidget->visibilityChanged(visible);
		if (pWindow->isLoadingPreferences())
		{
			return;
		}
		QCoreApplication::postEvent(qDockWidget, new QEvent(visible ? QEvent::FocusIn : QEvent::FocusOut));
	});

	if (qtWindow_.isReady())
	{
		bool isOk = qMainWindow->restoreDockWidget(qDockWidget);
		if (!isOk)
		{
			setDefaultPreferenceForDockWidget(qDockWidget);
		}
		QmlView* qmlView = dynamic_cast<QmlView*>(pView);
		if (qmlView != nullptr)
		{
			if (qDockWidget->isVisible() && qDockWidget->getVisibility())
			{
				qmlView->setNeedsToLoad(true);
			}
		}
	}
	else
	{
		needToRestorePreference_.push_back(std::make_pair(qDockWidget, pView));
	}

	qDockWidget->setWidget(qWidget);
	qDockWidget->createAction(*qtFramework, menuPath_);
	dockWidgetMap_[&view] = std::unique_ptr<QDockWidget>(qDockWidget);
}

void QtDockRegion::removeView(IView& view)
{
	auto qMainWindow = qtWindow_.window();
	if (qMainWindow == nullptr)
	{
		return;
	}

	auto findIt = dockWidgetMap_.find(&view);
	if (findIt == dockWidgetMap_.end())
	{
		return;
	}

	if (qtWindow_.getFocusedView() == &view)
	{
		qtWindow_.setFocusedView(nullptr);
	}

	// TODO: save dockWidget state
	auto dockWidget = std::move(findIt->second);
	dockWidgetMap_.erase(findIt);

	TF_ASSERT(dockWidget != nullptr);

	setWidgetPersistentGeometry(dockWidget->objectName().toUtf8().constData() , dockWidget->saveGeometry());

	dockWidget->setWidget(nullptr);
	qMainWindow->removeDockWidget(dockWidget.get());

	// call this function to let IView control the qWidget's life-cycle again.
	auto qtFramework = get<IQtFramework>();
	TF_ASSERT(qtFramework != nullptr);
	qtFramework->retainQWidget(view);
	dockWidget = nullptr;
}
} // end namespace wgt
