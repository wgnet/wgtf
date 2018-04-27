#include "qt_window.hpp"

#include "core_common/assert.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"

#include "i_qt_framework.hpp"
#include "qt_context_menu.hpp"
#include "qt_dock_region.hpp"
#include "qt_menu_bar.hpp"
#include "qt_status_bar.hpp"
#include "qt_tab_region.hpp"
#include "qt_tool_bar.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/binary_block.hpp"
#include "helpers/qt_helpers.hpp"
#include <QApplication>
#include <QDockWidget>
#include <QElapsedTimer>
#include <QEvent>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMimeData>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QUiLoader>
#include <QWindow>
#include <chrono>
#include <thread>
#include "qt_global_settings.hpp"

namespace wgt
{
namespace
{
const char* g_internalPreferenceId = "E28A7FA9-08D4-464F-B073-47CB9DD20F62";
const std::unordered_map<std::string, Qt::Corner> g_cornerMap{
	{"topLeftCorner", Qt::TopLeftCorner}, {"topRightCorner", Qt::TopRightCorner},
	{"bottomLeftCorner", Qt::BottomLeftCorner}, {"bottomRightCorner", Qt::BottomRightCorner}};
const std::unordered_map<std::string, Qt::DockWidgetArea> g_dockMap{
	{"leftDockWidgetArea", Qt::LeftDockWidgetArea}, {"rightDockWidgetArea", Qt::RightDockWidgetArea},
	{"topDockWidgetArea", Qt::TopDockWidgetArea}, {"bottomDockWidgetArea", Qt::BottomDockWidgetArea},
	{"dockWidgetArea_Mask", Qt::DockWidgetArea_Mask}, {"allDockWidgetAreas", Qt::AllDockWidgetAreas}};
}

struct QtWindow::Impl : Depends<IQtFramework, IUIApplication>
{
	QtWindow& window_;
	std::unique_ptr<QMainWindow> mainWindow_;
	std::string id_;
	Menus menus_;
	Regions regions_;
	std::unique_ptr<IStatusBar> statusBar_;
	Qt::WindowModality modalityFlag_;
	bool isMaximizedInPreference_;
	bool firstTimeShow_;
	bool loadingPreferences_;
	QtConnectionHolder qtConnections_;
	IView* focusedView_;

	Impl(QIODevice& source, QtWindow& window)
	    : window_(window), isMaximizedInPreference_(false), firstTimeShow_(true), loadingPreferences_(false),
	      focusedView_(nullptr)
	{
		QUiLoader loader;

		// Load optional plugins that may have custom widgets
		auto qtFramework = get<IQtFramework>();
		TF_ASSERT(qtFramework != nullptr);
		auto& pluginPath = qtFramework->getPluginPath();
		if (!pluginPath.empty())
		{
			loader.addPluginPath(pluginPath.c_str());
		}
		auto qMainWindow = qobject_cast<QMainWindow*>(loader.load(&source));
		if (qMainWindow == nullptr)
		{
			return;
		}
		mainWindow_.reset(qMainWindow);
		mainWindow_->setAcceptDrops(true);
	}
	Impl(std::unique_ptr<QMainWindow>&& mainWindow, QtWindow& window)
	    : window_(window), isMaximizedInPreference_(false), firstTimeShow_(true), loadingPreferences_(false),
	      focusedView_(nullptr)
	{
		if (mainWindow_ == nullptr)
		{
			return;
		}
	}

	~Impl()
	{
	}

	void init()
	{
		auto qtFramework = get<IQtFramework>();
		TF_ASSERT(qtFramework != nullptr);
		auto idProperty = mainWindow_->property("id");
		if (idProperty.isValid())
		{
			id_ = idProperty.toString().toUtf8().operator const char*();
		}

		for (const auto& docRegion: g_dockMap)
		{
			auto docRegionProperty = mainWindow_->property(docRegion.first.c_str());
			if (docRegionProperty.isValid())
			{
				auto cornersProperty = docRegionProperty.toStringList();
				for (auto it = cornersProperty.cbegin(); it != cornersProperty.cend(); ++it)
				{
					auto corner = g_cornerMap.find(std::string(it->toUtf8().constData()));
					if (corner != g_cornerMap.end())
					{
						mainWindow_->setCorner(corner->second, docRegion.second);
					}
				}
			}
		}

		auto windowMaximizedProperty = mainWindow_->property("windowMaximized");
		if (windowMaximizedProperty.isValid())
		{
			isMaximizedInPreference_ = windowMaximizedProperty.toBool();
		}

		Palette::Theme theme = Palette::Dark;
		auto themeProperty = mainWindow_->property("theme");
		if (themeProperty.isValid())
		{
			const auto themePropertyValue = themeProperty.toUInt();
			if(themePropertyValue < Palette::Theme::MaxThemes)
			{
				theme = static_cast<Palette::Theme>(themePropertyValue);
			}
		}

		qtFramework->palette()->setTheme(theme);

		auto menuBars = QtHelpers::getChildren<QMenuBar>(*mainWindow_);
		for (auto& menuBar : menuBars)
		{
			if (menuBar->property("path").isValid())
			{
				menus_.emplace_back(new QtMenuBar(*menuBar, id_.c_str()));
			}
		}

		auto toolBars = QtHelpers::getChildren<QToolBar>(*mainWindow_);
		for (auto& toolBar : toolBars)
		{
			if (toolBar->property("path").isValid())
			{
				menus_.emplace_back(new QtToolBar(*toolBar, id_.c_str()));
			}
		}

		auto dockWidgets = QtHelpers::getChildren<QDockWidget>(*mainWindow_);
		for (auto& dockWidget : dockWidgets)
		{
			if (dockWidget->property("layoutTags").isValid())
			{
				regions_.emplace_back(new QtDockRegion(window_, *dockWidget));
			}
		}

		auto tabWidgets = QtHelpers::getChildren<QTabWidget>(*mainWindow_);
		for (auto& tabWidget : tabWidgets)
		{
			if (tabWidget->property("layoutTags").isValid())
			{
				regions_.emplace_back(new QtTabRegion(&window_, *tabWidget));
			}
		}

		auto statusBar = QtHelpers::getChildren<QStatusBar>(*mainWindow_);
		if (statusBar.size() > 0)
		{
			statusBar_.reset(new QtStatusBar(*statusBar.at(0)));
		}

		if(mainWindow_->contextMenuPolicy() == Qt::CustomContextMenu)
		{
			createCustomContextMenu();
		}

		modalityFlag_ = mainWindow_->windowModality();
		mainWindow_->installEventFilter(&window_);
		loadPreference();
		window_.onPaletteChanged();
		auto globalSettings = qtFramework->qtGlobalSettings();
		qtConnections_ += QObject::connect(globalSettings, &QtGlobalSettings::prePreferencesChanged, &window_,
		                                   &QtWindow::onPrePreferencesChanged);
		qtConnections_ += QObject::connect(globalSettings, &QtGlobalSettings::postPreferencesChanged, &window_,
		                                   &QtWindow::onPostPreferencesChanged);
		qtConnections_ += QObject::connect(globalSettings, &QtGlobalSettings::prePreferencesSaved, &window_,
		                                   &QtWindow::onPrePreferencesSaved);
		qtConnections_ += QObject::connect(qApp, &QApplication::paletteChanged, &window_, &QtWindow::onPaletteChanged);
	}

	void fini()
	{
		if (mainWindow_)
		{
			mainWindow_->removeEventFilter(&window_);
			statusBar_.reset();
			regions_.clear();
			mainWindow_ = nullptr;
		}
	}

	void createCustomContextMenu()
	{
		auto contextMenu = mainWindow_->property("contextMenuName");
		if (contextMenu.isValid())
		{
			if (auto menu = mainWindow_->findChild<QMenu*>(contextMenu.toString()))
			{
				if (menu->property("path").isValid())
				{
					menus_.emplace_back(new QtContextMenu(*menu, mainWindow_.get(), id_.c_str()));
					qtConnections_ += QObject::connect(mainWindow_.get(),
						&QWidget::customContextMenuRequested, [menu, this](const QPoint& pos)
					{
						if(showCustomContextMenu(pos))
						{
							menu->exec(mainWindow_->mapToGlobal(pos));
						}
					});
				}
			}
			else
			{
				NGT_WARNING_MSG("Could not find custom context menu %s", 
					contextMenu.toString().toUtf8().constData());
			}
		}
	}

	bool showCustomContextMenu(const QPoint& pos)
	{
		// Only show the context menu for direct QDockWidget children
		// Reflects behaviour of QMainWindow::contextMenuEvent
		// This is so views can control their own context menus

		QWidget *child = mainWindow_->childAt(pos);
		while (child && child != mainWindow_.get()) 
		{
			if (QMenuBar* mb = qobject_cast<QMenuBar*>(child))
			{
				return mb->parentWidget() == mainWindow_.get()
					? child != mainWindow_.get() : false;
			}
			if (QDockWidget* dw = qobject_cast<QDockWidget*>(child)) 
			{
				if (dw->parentWidget() != mainWindow_.get())
				{
					return false;
				}
				if (dw->widget() && dw->widget()->geometry().contains(child->mapFrom(mainWindow_.get(), pos)))
				{
					// ignore the event if the mouse is over the QDockWidget contents
					return false;
				}
				return child != mainWindow_.get();
			}
			if (QToolBar* tb = qobject_cast<QToolBar*>(child)) 
			{
				return tb->parentWidget() == mainWindow_.get()
					? child != mainWindow_.get() : false;
			}
			child = child->parentWidget();
		}
		return child != mainWindow_.get();
	}

	bool savePreference()
	{
		if (!window_.isReady())
		{
			return false;
		}
		auto qtFramework = get<IQtFramework>();
		TF_ASSERT(qtFramework != nullptr);
		auto preferences = qtFramework->getPreferences();
		if (preferences == nullptr)
		{
			return false;
		}
		std::string key = (id_ == "") ? g_internalPreferenceId : id_;
		auto& preference = preferences->getPreference(key.c_str());
		QByteArray geometryData = mainWindow_->saveGeometry();
		QByteArray layoutData = mainWindow_->saveState();
		std::shared_ptr<BinaryBlock> geometry =
		std::make_shared<BinaryBlock>(geometryData.constData(), geometryData.size(), false);
		std::shared_ptr<BinaryBlock> state =
		std::make_shared<BinaryBlock>(layoutData.constData(), layoutData.size(), false);
		preference->set("geometry", geometry);
		preference->set("layoutState", state);
		bool isMaximized = mainWindow_->isMaximized();
		preference->set("maximized", isMaximized);
		if (!isMaximized)
		{
			auto pos = mainWindow_->pos();
			auto size = mainWindow_->size();
			preference->set("pos", Vector2(pos.x(), pos.y()));
			preference->set("size", Vector2(size.width(), size.height()));
		}

		preference->set("theme", qtFramework->palette()->getTheme());
		preference->set("customWindowColor", qtFramework->palette()->getColor(Palette::CustomWindowColor).name().toUtf8().constData());
		preference->set("customHighlightColor", qtFramework->palette()->getColor(Palette::CustomHighlightColor).name().toUtf8().constData());
		preference->set("customReadonlyColor", qtFramework->palette()->getColor(Palette::CustomReadonlyColor).name().toUtf8().constData());
		preference->set("customDarkText", qtFramework->palette()->getCustomDarkText());
		preference->set("customDarkContrast", qtFramework->palette()->getCustomContrast(true));
		preference->set("customLightContrast", qtFramework->palette()->getCustomContrast(false));

		auto dockWidgets = QtHelpers::getChildren<QDockWidget>(*mainWindow_);
		for(auto& dockWidget : dockWidgets)
		{
			QByteArray dockGeometryData = dockWidget->saveGeometry();
			std::shared_ptr<BinaryBlock> dockGeometry = 
			std::make_shared<BinaryBlock>(dockGeometryData.constData(), dockGeometryData.size(), false);
			std::string dockWidgetName = dockWidget->objectName().toUtf8().constData();
			std::string dockWidgetNameClean;
			// GenericObject doesn't allow property names with a . in them
			for(size_t i = 0; i < dockWidgetName.size(); ++i)
			{
				if(dockWidgetName[i] == '.')
					continue;
				dockWidgetNameClean.push_back(dockWidgetName[i]);
			}

			if(!preference->set(dockWidgetNameClean.c_str(), dockGeometry))
			{
				NGT_DEBUG_MSG("Failed to save preference for dock widget %s", dockWidgetName.c_str());
			}
		}
		return true;
	}

	bool loadPreference()
	{
		auto logDebugMessage = [](const std::string& message) {
			NGT_DEBUG_MSG("Qt Window Preferences Failed: %s", message.c_str());
		};

		auto qtFramework = get<IQtFramework>();
		TF_ASSERT(qtFramework != nullptr);
		auto preferences = qtFramework->getPreferences();
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
		BinaryBlock state;
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

		accessor = preference->findProperty("layoutState");
		if (!accessor.isValid())
		{
			logDebugMessage(key + " No layoutState property");
			return false;
		}

		isOk = preference->get("layoutState", state);
		if (!isOk)
		{
			logDebugMessage(key + " LayoutState property failed");
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

		std::string customCol;
		accessor = preference->findProperty("customWindowColor");
		if (accessor.isValid())
		{
			if (preference->get("customWindowColor", customCol))
			{
				qtFramework->palette()->setCustomColor(Palette::CustomWindowColor, QColor(customCol.c_str()));
			}
		}
		accessor = preference->findProperty("customHighlightColor");
		if (accessor.isValid())
		{
			if (preference->get("customHighlightColor", customCol))
			{
				qtFramework->palette()->setCustomColor(Palette::CustomHighlightColor, QColor(customCol.c_str()));
			}
		}
		accessor = preference->findProperty("customReadonlyColor");
		if (accessor.isValid())
		{
			if (preference->get("customReadonlyColor", customCol))
			{
				qtFramework->palette()->setCustomColor(Palette::CustomReadonlyColor, QColor(customCol.c_str()));
			}
		}

		bool customDText;
		accessor = preference->findProperty("customDarkText");
		if (accessor.isValid())
		{
			if (preference->get("customDarkText", customDText))
			{
				qtFramework->palette()->setCustomDarkText(customDText);
			}
		}
		int customVal;
		accessor = preference->findProperty("customDarkContrast");
		if (accessor.isValid())
		{
			if (preference->get("customDarkContrast", customVal))
			{
				qtFramework->palette()->setCustomContrast(customVal, true);
			}
		}
		accessor = preference->findProperty("customLightContrast");
		if (accessor.isValid())
		{
			if (preference->get("customLightContrast", customVal))
			{
				qtFramework->palette()->setCustomContrast(customVal, false);
			}
		}

		// restore preferences
		int theme;
		accessor = preference->findProperty("theme");
		if (accessor.isValid())
		{
			isOk = preference->get("theme", theme);
			if (isOk && theme >= 0 && theme < Palette::MaxThemes)
			{
				qtFramework->palette()->setTheme(static_cast<Palette::Theme>(theme));
			}
		}

		auto dockWidgets = QtHelpers::getChildren<QDockWidget>(*mainWindow_);
		for(auto& dockWidget : dockWidgets)
		{
			std::string widgetName = dockWidget->objectName().toUtf8().constData();
			std::string widgetNameClean;
			// GenericObject doesn't allow property names with a . in them
			for(size_t i = 0; i < widgetName.size(); ++i)
			{
				if(widgetName[i] == '.')
					continue;
				widgetNameClean.push_back(widgetName[i]);
			}
			accessor = preference->findProperty(widgetNameClean.c_str());
			if(accessor.isValid())
			{
				BinaryBlock widgetGeometryData;
				isOk = preference->get(widgetNameClean.c_str(), widgetGeometryData);
				if(isOk)
				{
					isOk = dockWidget->restoreGeometry(QByteArray(widgetGeometryData.cdata(), 
						static_cast<int>(widgetGeometryData.length())));
				}
				
				if(!isOk)
				{
					logDebugMessage(widgetName + " geometry data restore failed!");
				}
			}
		}

		isMaximizedInPreference_ = isMaximized;
		isOk = mainWindow_->restoreGeometry(QByteArray(geometry.cdata(), static_cast<int>(geometry.length())));
		if(!isOk)
		{
			logDebugMessage(key + " Could not restore geometry");
			//return false;
		}

		isOk = mainWindow_->restoreState(QByteArray(state.cdata(), static_cast<int>(state.length())));
		if (!isOk)
		{
			logDebugMessage(key + " Could not restore state");
			return false;
		}

		if (!isMaximized)
		{
			mainWindow_->move(QPoint(static_cast<int>(pos.x), static_cast<int>(pos.y)));
			mainWindow_->resize(QSize(static_cast<int>(size.x), static_cast<int>(size.y)));
		}

		return true;
	}

	void show(bool wait)
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		mainWindow_->setWindowModality(modalityFlag_);
		if (firstTimeShow_ && isMaximizedInPreference_)
		{
			mainWindow_->setWindowState(Qt::WindowMaximized);
		}
		mainWindow_->show();

		if (firstTimeShow_)
		{
			emit window_.windowReady();
		}
		firstTimeShow_ = false;
		if (wait)
		{
			waitForWindowExposed();
		}
	}

	void showMaximized(bool wait)
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		mainWindow_->setWindowModality(modalityFlag_);

		mainWindow_->showMaximized();

		if (firstTimeShow_)
		{
			emit window_.windowReady();
		}
		firstTimeShow_ = false;
		if (wait)
		{
			waitForWindowExposed();
		}
	}

	void showModal()
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		mainWindow_->setWindowModality(Qt::ApplicationModal);
		if (firstTimeShow_ && isMaximizedInPreference_)
		{
			mainWindow_->setWindowState(Qt::WindowMaximized);
		}
		mainWindow_->show();
		if (firstTimeShow_)
		{
			emit window_.windowReady();
		}
		firstTimeShow_ = false;
	}

	void hide()
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}

		mainWindow_->hide();
	}

	void waitForWindowExposed()
	{
		if (mainWindow_.get() == nullptr)
		{
			return;
		}
		enum
		{
			TimeOutMs = 10
		};
		QElapsedTimer timer;
		const int timeout = 1000;
		if (!mainWindow_->windowHandle())
		{
			mainWindow_->createWinId();
		}
		auto window = mainWindow_->windowHandle();
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

	bool reloadPreferences()
	{
		loadingPreferences_ = true;
		bool success = this->loadPreference();
		loadingPreferences_ = false;
		return success;
	}
};

QtWindow::QtWindow(QIODevice& source) : impl_(new Impl(source, *this))
{
	impl_->init();
}

QtWindow::QtWindow(std::unique_ptr<QMainWindow>&& mainWindow)
    : impl_(new Impl(std::forward<std::unique_ptr<QMainWindow>&&>(mainWindow), *this))
{
	impl_->init();
}

QtWindow::~QtWindow()
{
	if (impl_)
	{
		impl_->fini();
	}
}

const char* QtWindow::id() const
{
	return impl_->id_.c_str();
}

const char* QtWindow::title() const
{
	if (impl_->mainWindow_.get() == nullptr)
	{
		return "";
	}

	return impl_->mainWindow_->windowTitle().toUtf8().constData();
}

void QtWindow::update()
{
	for (auto& menu : impl_->menus_)
	{
		menu->update();
	}
}

void QtWindow::close()
{
	if (impl_->mainWindow_.get() == nullptr)
	{
		return;
	}

	impl_->mainWindow_->close();
}

bool QtWindow::isReady() const
{
	if (impl_ && !impl_->firstTimeShow_)
	{
		return true;
	}
	return false;
}

void QtWindow::setIcon(const char* path)
{
	if (!path || !impl_->mainWindow_)
		return;

	impl_->mainWindow_->setWindowIcon(QIcon(path));
}

void QtWindow::show(bool wait /* = false */)
{
	impl_->show(wait);
}

void QtWindow::showMaximized(bool wait /* = false */)
{
	impl_->showMaximized(wait);
}

void QtWindow::showModal()
{
	impl_->showModal();
}

void QtWindow::hide()
{
	impl_->hide();
}

void QtWindow::title(const char* title)
{
	impl_->mainWindow_->setWindowTitle(title);
}

const Menus& QtWindow::menus() const
{
	return impl_->menus_;
}

const Regions& QtWindow::regions() const
{
	return impl_->regions_;
}

IStatusBar* QtWindow::statusBar() const
{
	return impl_->statusBar_.get();
}

QMainWindow* QtWindow::window() const
{
	return impl_->mainWindow_.get();
}

QMainWindow* QtWindow::releaseWindow()
{
	return impl_->mainWindow_.release();
}

void QtWindow::onPaletteChanged()
{
	impl_->mainWindow_->setStyleSheet(
		"QMainWindow::separator:vertical{background: palette(dark); width: 4px; border-right: 1px solid palette(midlight);}"
		"QMainWindow::separator:vertical{background: palette(dark); width: 4px; border-left: 1px solid palette(midlight);}"
		"QMainWindow::separator:horizontal{background: palette(dark); height: 4px; border-bottom: 1px solid "
		"palette(midlight);}"
		"QMainWindow::separator:horizontal{background: palette(dark); height: 4px; border-top: 1px solid "
		"palette(midlight);}"
		"QTabBar::close-button{image: url(:/WGControls/icons/close_sml_16x16.png); subcontrol-position: right;}"
		"QTabBar::close-button:hover{image: url(:/WGControls/icons/delete_sml_16x16.png); subcontrol-position: right;}"
		"QToolBar{spacing: 0px;}"
		"QToolBar::handle{width: 20px; height: 32px;}"
		"QToolButton{border: 1px solid transparent; padding: 0px -1px -1px 0px;}"
		"QToolButton:hover{border: 1px solid #7e8081; background-color: palette(midlight); padding: 0px -1px -1px 0px;}"
		"QToolButton:pressed{border: 1px solid palette(dark); background-color: palette(dark); padding: 0px -1px -1px 0px;}"
		"QStatusBar{background: palette(dark); border: 1px solid palette(midlight);}");
}

void QtWindow::init()
{
	impl_->init();
}

bool QtWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == impl_->mainWindow_.get())
	{
		if (event->type() == QEvent::Close)
		{
			bool shouldClose = true;

			this->signalTryClose(shouldClose);

			if (shouldClose)
			{
				this->signalClose();
			}
			else
			{
				event->ignore();
			}

			return true;
		}
		else if (event->type() == QEvent::DragEnter || event->type() == QEvent::Drop)
		{
			const QMimeData* mimeData = static_cast<QDragEnterEvent*>(event)->mimeData();
			if (mimeData == nullptr || !mimeData->hasUrls()) return false;

			if (event->type() == QEvent::DragEnter) 
			{
				event->accept();
			}
			else 
			{
				TF_ASSERT(event->type() == QEvent::Drop);
				auto uiApplication = impl_->get<IUIApplication>();
				if (uiApplication) 
				{
					std::vector<std::string> *queuedLoads = uiApplication->getQueuedFileLoads();
					if (queuedLoads) 
					{
						QList<QUrl> urls = mimeData->urls();
						for (const QUrl& url : urls) 
						{
							std::string filePath = url.toString(QUrl::PreferLocalFile).toStdString();
							queuedLoads->push_back(filePath);
						}
					}
				}
			}
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

void QtWindow::onPrePreferencesChanged()
{
	impl_->savePreference();
}

void QtWindow::onPostPreferencesChanged()
{
	impl_->reloadPreferences();
}

void QtWindow::onPrePreferencesSaved()
{
	impl_->savePreference();
}

bool QtWindow::isLoadingPreferences() const
{
	return impl_->loadingPreferences_;
}

IView* QtWindow::getFocusedView() const
{
	return impl_->focusedView_;
}

void QtWindow::setFocusedView(IView* view)
{
	if (impl_->focusedView_ == view)
	{
		return;
	}

	auto oldView = impl_->focusedView_;
	impl_->focusedView_ = view;

	if (oldView && oldView->focused())
	{
		oldView->setFocus(false);
	}

	if (view && !view->focused())
	{
		view->setFocus(true);
	}
}

bool QtWindow::resetLayout()
{
	return impl_->reloadPreferences();
}

bool QtWindow::savePreference()
{
	return impl_->savePreference();
}

bool QtWindow::loadPreference()
{
	return impl_->loadPreference();
}

uintptr_t QtWindow::getNativeWindowHandle()
{
	if (impl_.get() == nullptr)
	{
		return 0;
	}
	if (impl_->mainWindow_ == nullptr)
	{
		return 0;
	}
	return impl_->mainWindow_->winId();
}
} // end namespace wgt
