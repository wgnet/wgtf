#include "qt_application.hpp"

#include "core_common/assert.hpp"
#include "core_common/ngt_windows.hpp"
#include "core_common/platform_env.hpp"

#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qml_view.hpp"
#include "core_qt_common/qt_cursor.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_qt_common/qt_window.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>
#include <QTimer>
#include <QSplashScreen>

namespace wgt
{
QtApplication::QtApplication(int& argc, char** argv)
    : application_(nullptr), argc_(argc), argv_(argv), splash_(nullptr), bQuit_(false)
{
	char wgtHome[MAX_PATH];

	if (Environment::getValue<MAX_PATH>("WGT_HOME", wgtHome))
	{
		QCoreApplication::addLibraryPath(wgtHome);
	}

	application_.reset(new QApplication(argc_, argv_));

	QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	QApplication::setDesktopSettingsAware(false);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setFont(QFont("Noto Sans", 9));

	startTimer(10, [this]() { update(); });
}

QtApplication::~QtApplication()
{
	for (auto& timer : timers_)
	{
		timer.second->stop();
		timer.second->deleteLater();
	}
}

void QtApplication::initialise()
{
	// Splash
	QPixmap pixmap;

	// Query for custom splash screens
	bool foundCustomSplash = false;
	const auto pSplash = this->get<ISplash>();
	if (pSplash != nullptr)
	{
		std::unique_ptr<BinaryBlock> splashData;
		std::string format;
		const auto loaded = pSplash->loadData(splashData, format);
		if (loaded)
		{
			foundCustomSplash = pixmap.loadFromData(static_cast<const uchar*>(splashData->data()),
			                                        static_cast<uint>(splashData->length()), format.c_str());
		}
	}

	// Use default splash screen
	if (!foundCustomSplash)
	{
		pixmap.load(":/qt_app/splash");
	}

	splash_.reset(new QSplashScreen(pixmap));
	splash_->show();
	auto qtFramework = this->get<IQtFramework>();
	TF_ASSERT(qtFramework != nullptr);
	if (qtFramework != nullptr)
	{
		auto palette = qtFramework->palette();
		if (palette != nullptr)
		{
			application_->setPalette(palette->toQPalette());
		}
	}
}

void QtApplication::finalise()
{
	signalUpdate.clear();
}

void QtApplication::update()
{
	layoutManager_.update();
	auto qtFramework = this->get<IQtFramework>();
	if (qtFramework != nullptr)
	{
		qtFramework->incubate();
	}
	signalUpdate();
}

int QtApplication::startApplication()
{
	TF_ASSERT(application_ != nullptr);
	signalStartUp();
	splash_->close();
	if (bQuit_)
	{
		return 0;
	}

	if (auto automation = get<IAutomation>())
	{
		automation->notifyLoadingDone();
	}

	NGT_MSG("Starting %s\n%s",
		application_->applicationFilePath().toUtf8().constData(),
		splash_ ? splash_->message().toUtf8().constData() : "");

	return application_->exec();
}

void QtApplication::quitApplication()
{
	signalExit();
	QApplication::quit();
	splash_ = nullptr;
	bQuit_ = true;
}

IApplication::TimerId QtApplication::startTimer(int interval_ms, TimerCallback callback)
{
	if (application_ == nullptr)
		return 0;

	auto timer = new QTimer(application_.get());
	QObject::connect(timer, &QTimer::timeout, [callback]() { callback(); });
	timer->start(interval_ms);
	timers_[timer->timerId()] = timer;
	return timer->timerId();
}

void QtApplication::killTimer(TimerId id)
{
	auto found = timers_.find(id);
	if (found != timers_.end())
	{
		auto timer = found->second;
		timer->stop();
		timers_.erase(found);
		timer->deleteLater();
	}
}

void QtApplication::setAppSettingsName(const char* name)
{
	applicationSettingsName_ = name;
}

const char* QtApplication::getAppSettingsName()
{
	return applicationSettingsName_.c_str();
}

void QtApplication::addWindow(IWindow& window)
{
	layoutManager_.addWindow(window);
}

void QtApplication::removeWindow(IWindow& window)
{
	layoutManager_.removeWindow(window);
}

void QtApplication::addMenuPath(const char* path, const char* windowId)
{
	layoutManager_.addMenuPath(path, windowId);
}

void QtApplication::addView(IView& view)
{
	layoutManager_.addView(view);
}

void QtApplication::removeView(IView& view)
{
	layoutManager_.removeView(view);
}

void QtApplication::addMenu(IMenu& menu)
{
	layoutManager_.addMenu(menu);
}

void QtApplication::removeMenu(IMenu& menu)
{
	layoutManager_.removeMenu(menu);
}

void QtApplication::addAction(IAction& action)
{
	layoutManager_.addAction(action);
}

void QtApplication::removeAction(IAction& action)
{
	layoutManager_.removeAction(action);
}

void QtApplication::setWindowIcon(const char* path, const char* windowId)
{
	layoutManager_.setWindowIcon(path, windowId);
}

void QtApplication::setStatusMessage(const char* message, int timeout)
{
	layoutManager_.setStatusMessage(message, timeout);
}

void QtApplication::setOverrideCursor(CursorId id)
{
	if(id.id() <= LastCursor.id())
	{
		auto cursorShape = static_cast<Qt::CursorShape>(id.id());
		application_->setOverrideCursor(cursorShape);
	}
	else if(auto qCursor = reinterpret_cast<QCursor*>(id.nativeCursor()))
	{
		application_->setOverrideCursor(*qCursor);
	}
}

void QtApplication::restoreOverrideCursor()
{
	application_->restoreOverrideCursor();
}

ICursorPtr QtApplication::createCustomCursor(const char * filename, int hotX, int hotY)
{
	return std::make_unique<QtCursor>(filename, hotX, hotY);
}

void QtApplication::saveWindowPreferences()
{
	layoutManager_.saveWindowPreferences();
}

bool QtApplication::splashScreenIsShowing()
{
	if (!splash_.get()) return false;

	bool result = splash_->isVisible();
	return result;
}

void QtApplication::toggleShowSplashScreen()
{
	if (!splash_.get()) return;

	if (splash_->isVisible()) splash_->hide();
	else                      splash_->show();
	application_->processEvents();
}


bool QtApplication::setSplashScreen(const char* const path)
{
	QPixmap pixmap;
	if (pixmap.load(path))
	{
		splash_.reset(new QSplashScreen(pixmap));
		application_->processEvents();
		return true;
	}
	else 
	{
		return false;
	}
}

bool QtApplication::setSplashScreenMessage(const char* const message) 
{
	if (!splash_.get()) return false;
	splash_->showMessage(message);
	application_->processEvents();

	return true;
}

const char* const QtApplication::getSplashScreenMessage() 
{
	if (splash_.get()) {
		QString qString = splash_->message();
		const char *const result = qString.toUtf8().data();
		return result;
	}

	return "";
}

const Windows& QtApplication::windows() const
{
	return layoutManager_.windows();
}

/*
void QtApplication::getCommandLine()
{
    char* winCommandLine = GetCommandLine();
    std::vector<char*> parameters;
    char* position = winCommandLine;
    char* start = position;
    bool quote = false;
    bool space = false;
    size_t index = 0;

    auto copyData = [&position, &start]()->char*
    {
        size_t len = position - start;
        char* destination = new char[len + 1];
        memcpy(destination, start, len);
        destination[len] = 0;
        return destination;
    };

    while (*position)
    {
        switch (*position)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            if (!quote)
            {
                parameters.push_back(copyData());
                ++index;
                space = true;
            }

            break;

        case '"':
            if (!quote)
            {
                start = position;
            }

            space = false;
            quote = !quote;
            break;

        default:
            if (space)
            {
                start = position;
            }

            space = false;
        }

        ++position;
    }

    if (position > winCommandLine && !whiteSpace(*(position - 1)))
    {
        parameters.push_back(copyData());
        ++index;
    }

    argc = index;
    argv = new char*[argc + 1];
    argv[argc] = nullptr;

    for (unsigned int i = 0; i < index; ++i)
    {
        argv[i] = parameters[i];
    }
}

bool QtApplication::whiteSpace(char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}
*/
} // end namespace wgt
