#include "qml_window.hpp"
#include "qt_dock_region.hpp"
#include "qt_menu_bar.hpp"
#include "qt_status_bar.hpp"
#include "qt_tab_region.hpp"
#include "qt_tool_bar.hpp"
#include "qt_global_settings.hpp"
#include "i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_logging/logging.hpp"
#include <cassert>
#include <thread>
#include <chrono>
#include <QQmlComponent>
#include <QDockWidget>
#include <QQuickWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QQmlContext>
#include <QQmlEngine>
#include <QApplication>
#include <QWindow>
#include <QElapsedTimer>
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_qt_common/qt_script_object.hpp"

namespace wgt
{
namespace
{
template <typename T>
std::vector<T*> getChildren(const QObject& parent)
{
	std::vector<T*> children;
	foreach (auto child, parent.children())
	{
		T* childT = qobject_cast<T*>(child);
		if (childT != nullptr)
		{
			children.push_back(childT);
		}
		auto grandChildren = getChildren<T>(*child);
		children.insert(children.end(), grandChildren.begin(), grandChildren.end());
	}
	return children;
}

const char* g_internalPreferenceId = "13CC6CD7-E935-488D-9E3D-8BED9454F554";
}

struct QmlWindow::Impl : Depends<IQtFramework>
{
	QQmlEngine& qmlEngine_;
	std::unique_ptr<QQmlContext> qmlContext_;
	QQuickWidget* mainWindow_;
	std::string id_;
	std::string title_;
	Menus menus_;
	Regions regions_;
	std::unique_ptr<IStatusBar> statusBar_;
	bool released_;
	Qt::WindowModality modalityFlag_;
	bool isMaximizedInPreference_;
	bool firstTimeShow_;
	QmlWindow& window_;
	QUrl url_;
	QtConnectionHolder qtConnections_;

	Impl(IComponentContext& context, QQmlEngine& qmlEngine, QmlWindow& window)
	    : Depends(context), qmlEngine_(qmlEngine), qmlContext_(new QQmlContext(qmlEngine.rootContext())),
	      mainWindow_(new QQuickWidget(&qmlEngine, nullptr)), released_(false), isMaximizedInPreference_(false),
	      firstTimeShow_(true), window_(window)
	{
		mainWindow_->setMinimumSize(QSize(100, 100));
		QQmlEngine::setContextForObject(mainWindow_, qmlContext_.get());

		auto qtFramework = context.queryInterface<IQtFramework>();
		assert(qtFramework != nullptr);
		auto globalSettings = qtFramework->qtGlobalSettings();

		qtConnections_ += QObject::connect(globalSettings, &QtGlobalSettings::prePreferencesChanged, &window_,
		                                   &QmlWindow::onPrePreferencesChanged);
		qtConnections_ += QObject::connect(globalSettings, &QtGlobalSettings::postPreferencesChanged, &window_,
		                                   &QmlWindow::onPostPreferencesChanged);
		qtConnections_ += QObject::connect(globalSettings, &QtGlobalSettings::prePreferencesSaved, &window_,
		                                   &QmlWindow::onPrePreferencesSaved);
	}

	void handleLoaded(QQmlComponent* qmlComponent)
	{
		auto content = std::unique_ptr<QObject>(qmlComponent->create(qmlContext_.get()));

		auto qtFrameWork = get<IQtFramework>();
		assert(qtFrameWork != nullptr);

		QVariant windowProperty = content->property("id");
		if (windowProperty.isValid())
		{
			id_ = windowProperty.toString().toUtf8().data();
		}

		auto windowMaximizedProperty = content->property("windowMaximized");
		if (windowMaximizedProperty.isValid())
		{
			isMaximizedInPreference_ = windowMaximizedProperty.toBool();
		}

		QVariant titleProperty = content->property("title");
		if (titleProperty.isValid())
		{
			title_ = titleProperty.toString().toUtf8().data();
		}

		auto menuBars = getChildren<QMenuBar>(*mainWindow_);
		for (auto& menuBar : menuBars)
		{
			if (menuBar->property("path").isValid())
			{
				menus_.emplace_back(new QtMenuBar(*menuBar, id_.c_str()));
			}
		}

		auto toolBars = getChildren<QToolBar>(*mainWindow_);
		for (auto& toolBar : toolBars)
		{
			if (toolBar->property("path").isValid())
			{
				menus_.emplace_back(new QtToolBar(*toolBar, id_.c_str()));
			}
		}
		auto dockWidgets = getChildren<QDockWidget>(*mainWindow_);
		if (!dockWidgets.empty())
		{
			NGT_WARNING_MSG("Qml window doesn't support docking");
		}
		auto tabWidgets = getChildren<QTabWidget>(*mainWindow_);
		for (auto& tabWidget : tabWidgets)
		{
			if (tabWidget->property("layoutTags").isValid())
			{
				regions_.emplace_back(new QtTabRegion(*this, *tabWidget));
			}
		}
		auto statusBar = getChildren<QStatusBar>(*mainWindow_);
		if (statusBar.size() > 0)
		{
			statusBar_.reset(new QtStatusBar(*statusBar.at(0)));
		}

		mainWindow_->setContent(url_, qmlComponent, content.release());
		mainWindow_->setResizeMode(QQuickWidget::SizeRootObjectToView);
		mainWindow_->installEventFilter(&window_);
		loadPreference();
		modalityFlag_ = mainWindow_->windowModality();
	}

	//--------------------------------------------------------------------------
	bool loadPreference()
	{
		auto logDebugMessage = [](const std::string& message) {
			NGT_DEBUG_MSG("QML Window Preferences Failed: %s", message.c_str());
		};

		auto preferences = get<IQtFramework>()->getPreferences();
		if (preferences == nullptr)
		{
			logDebugMessage("Could not get preferences");
			return false;
		}

		std::string key = (id_ == "") ? g_internalPreferenceId : id_;
		if (!preferences->preferenceExists(key.c_str()))
		{
			return false; // No preference has been loaded for this key
		}
		auto& preference = preferences->getPreference(key.c_str());

		// check the preference data first
		BinaryBlock geometry;
		bool isMaximized = false;
		Vector2 pos;
		Vector2 size;
		bool isOk = false;

		auto accessor = preference->findProperty("geometry");
		if (!accessor.isValid())
		{
			logDebugMessage(key + " No geometry property");
			return false;
		}

		isOk = preference->get("geometry", geometry);
		if (!isOk)
		{
			logDebugMessage(key + " Geometry property failed");
			return false;
		}

		accessor = preference->findProperty("maximized");
		if (!accessor.isValid())
		{
			logDebugMessage(key + " No maximized property");
			return false;
		}

		isOk = preference->get("maximized", isMaximized);
		if (!isOk)
		{
			logDebugMessage(key + " Maximized property failed");
			return false;
		}

		if (!isMaximized)
		{
			accessor = preference->findProperty("pos");
			if (!accessor.isValid())
			{
				logDebugMessage(key + " No pos property");
				return false;
			}

			isOk = preference->get("pos", pos);
			if (!isOk)
			{
				logDebugMessage(key + " Pos property failed");
				return false;
			}

			accessor = preference->findProperty("size");
			if (!accessor.isValid())
			{
				logDebugMessage(key + " No size property");
				return false;
			}

			isOk = preference->get("size", size);
			if (!isOk)
			{
				logDebugMessage(key + " Size property failed");
				return false;
			}
		}

		// restore preferences
		isMaximizedInPreference_ = isMaximized;
		isOk = mainWindow_->restoreGeometry(QByteArray(geometry.cdata(), static_cast<int>(geometry.length())));
		if (!isOk)
		{
			logDebugMessage(key + " Could not restore geometry");
			return false;
		}

		if (!isMaximized)
		{
			mainWindow_->move(QPoint(static_cast<int>(pos.x), static_cast<int>(pos.y)));
			mainWindow_->resize(QSize(static_cast<int>(size.x), static_cast<int>(size.y)));
		}

		return true;
	}
};

QmlWindow::QmlWindow(IComponentContext& context, QQmlEngine& qmlEngine) : impl_(new Impl(context, qmlEngine, *this))
{
	QObject::connect(impl_->mainWindow_, SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)), this,
	                 SLOT(error(QQuickWindow::SceneGraphError, const QString&)));
}

QmlWindow::~QmlWindow()
{
	if (!impl_->released_)
	{
		this->savePreference();
		impl_->mainWindow_->removeEventFilter(this);
		delete impl_->mainWindow_;
	}
	impl_->qmlEngine_.collectGarbage();
	// call sendPostedEvents to give chance to QScriptObject's DeferredDeleted event get handled in time
	QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

void QmlWindow::setContextObject(QObject* object)
{
	auto qtScriptObject = dynamic_cast<QtScriptObject*>(object);
	if (qtScriptObject)
	{
		qtScriptObject->setParent(impl_->qmlContext_.get());
	}
	else
	{
		object->setParent(impl_->qmlContext_.get());
	}
	impl_->qmlContext_->setContextObject(object);
}

void QmlWindow::setContextProperty(const QString& name, const QVariant& property)
{
	if (property.canConvert<QObject*>())
	{
		auto object = property.value<QObject*>();
		if (!object->isWidgetType() && !object->isWindowType())
		{
			auto qtScriptObject = dynamic_cast<QtScriptObject*>(object);
			if (qtScriptObject)
			{
				qtScriptObject->setParent(impl_->qmlContext_.get());
			}
			else
			{
				object->setParent(impl_->qmlContext_.get());
			}
		}
	}
	impl_->qmlContext_->setContextProperty(name, property);
}

void QmlWindow::error(QQuickWindow::SceneGraphError error, const QString& message)
{
	NGT_ERROR_MSG("QmlWindow::error, rendering error: %s\n", message.toLatin1().constData());
}

const char* QmlWindow::id() const
{
	return impl_->id_.c_str();
}

const char* QmlWindow::title() const
{
	return impl_->title_.c_str();
}

void QmlWindow::update()
{
	for (auto& menu : impl_->menus_)
	{
		menu->update();
	}
}

void QmlWindow::close()
{
	impl_->mainWindow_->close();
}

void QmlWindow::setIcon(const char* path)
{
	if (!path || !impl_->mainWindow_)
		return;
	impl_->mainWindow_->setWindowIcon(QIcon(path));
}

void QmlWindow::show(bool wait /* = false */)
{
	impl_->mainWindow_->setWindowModality(impl_->modalityFlag_);
	if (impl_->firstTimeShow_ && impl_->isMaximizedInPreference_)
	{
		impl_->mainWindow_->setWindowState(Qt::WindowMaximized);
	}
	impl_->mainWindow_->show();
	if (title())
	{
		impl_->mainWindow_->setWindowTitle(title());
	}
	impl_->firstTimeShow_ = false;
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QmlWindow::showMaximized(bool wait /* = false */)
{
	impl_->mainWindow_->setWindowModality(impl_->modalityFlag_);

	impl_->mainWindow_->showMaximized();
	impl_->firstTimeShow_ = false;
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QmlWindow::showModal()
{
	impl_->mainWindow_->setWindowModality(Qt::ApplicationModal);
	if (title())
	{
		impl_->mainWindow_->setWindowTitle(title());
	}
	if (impl_->firstTimeShow_ && impl_->isMaximizedInPreference_)
	{
		impl_->mainWindow_->setWindowState(Qt::WindowMaximized);
	}
	impl_->mainWindow_->show();
	impl_->firstTimeShow_ = false;
}

void QmlWindow::hide()
{
	impl_->mainWindow_->hide();
}

void QmlWindow::title(const char* title)
{
	impl_->mainWindow_->setWindowTitle(title);
}

const Menus& QmlWindow::menus() const
{
	return impl_->menus_;
}

const Regions& QmlWindow::regions() const
{
	return impl_->regions_;
}

IStatusBar* QmlWindow::statusBar() const
{
	return impl_->statusBar_.get();
}

QQuickWidget* QmlWindow::release()
{
	impl_->released_ = true;
	return window();
}

QQuickWidget* QmlWindow::window() const
{
	return impl_->mainWindow_;
}

void QmlWindow::waitForWindowExposed()
{
	if (impl_->mainWindow_ == nullptr)
	{
		return;
	}
	enum
	{
		TimeOutMs = 10
	};
	QElapsedTimer timer;
	const int timeout = 1000;
	if (!impl_->mainWindow_->windowHandle())
	{
		impl_->mainWindow_->createWinId();
	}
	auto window = impl_->mainWindow_->windowHandle();
	timer.start();
	while (!window->isExposed())
	{
		const int remaining = timeout - int(timer.elapsed());
		if (remaining <= 0)
		{
			break;
		}
		QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
		QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
		std::this_thread::sleep_for(std::chrono::milliseconds(uint32_t(TimeOutMs)));
	}
}

//------------------------------------------------------------------------------
bool QmlWindow::load(QUrl& qUrl, bool async, std::function<void()> loadedHandler)
{
	impl_->url_ = qUrl;
	auto qtFrameWork = impl_->get<IQtFramework>();
	assert(qtFrameWork != nullptr);
	auto preferences = qtFrameWork->getPreferences();
	auto preference = preferences->getPreference(impl_->id_.c_str());
	auto value = qtFrameWork->toQVariant(preference, impl_->qmlContext_.get());
	this->setContextProperty(QString("Preference"), value);
	this->setContextProperty(QString("View"), QVariant::fromValue(impl_->mainWindow_));
	impl_->qmlContext_->setContextProperty(QString("qmlView"), this);

	auto qmlEngine = impl_->qmlContext_->engine();

	auto qmlComponent = new QQmlComponent(qmlEngine, impl_->mainWindow_);

	QmlComponentLoaderHelper helper(qmlComponent, qUrl);
	using namespace std::placeholders;
	helper.data_->connections_ += helper.data_->sig_Loaded_.connect(std::bind(&Impl::handleLoaded, impl_.get(), _1));
	helper.data_->connections_ +=
	helper.data_->sig_Loaded_.connect([loadedHandler](QQmlComponent*) { loadedHandler(); });
	helper.load(async);
	return true;
}

//------------------------------------------------------------------------------
bool QmlWindow::eventFilter(QObject* object, QEvent* event)
{
	if (object == impl_->mainWindow_)
	{
		if (event->type() == QEvent::Close)
		{
			bool shouldClose = true;

			this->signalTryClose(shouldClose);

			if (shouldClose)
			{
				this->signalClose();
				// TODO: remove this workaround
				// explicitly call this because CollectionModel doesn't emit a signal when underly source changed
				emit windowClosed();
			}
			else
			{
				event->ignore();
			}

			return true;
		}
	}
	return QObject::eventFilter(object, event);
}

void QmlWindow::savePreference()
{
	auto qtFramework = impl_->get<IQtFramework>();
	assert(qtFramework != nullptr);
	if (qtFramework == nullptr)
	{
		return;
	}
	auto preferences = qtFramework->getPreferences();
	if (preferences == nullptr)
	{
		return;
	}
	std::string key = impl_->id_ + g_internalPreferenceId;
	auto& preference = preferences->getPreference(key.c_str());
	QByteArray geometryData = impl_->mainWindow_->saveGeometry();
	std::shared_ptr<BinaryBlock> geometry =
	std::make_shared<BinaryBlock>(geometryData.constData(), geometryData.size(), false);
	preference->set("geometry", geometry);
	bool isMaximized = impl_->mainWindow_->isMaximized();
	preference->set("maximized", isMaximized);
	if (!isMaximized)
	{
		auto pos = impl_->mainWindow_->pos();
		auto size = impl_->mainWindow_->size();
		preference->set("pos", Vector2(pos.x(), pos.y()));
		preference->set("size", Vector2(size.width(), size.height()));
	}
}

void QmlWindow::onPrePreferencesChanged()
{
	savePreference();
}

void QmlWindow::onPostPreferencesChanged()
{
	impl_->loadPreference();
}

void QmlWindow::onPrePreferencesSaved()
{
	savePreference();
}

} // end namespace wgt
