#include "qt_desktop_application.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_env.hpp"
#include "core_qt_common/qt_palette.hpp"

#include <QTimer>
#include <QApplication>
#include <QCoreApplication>
#include <QStyleFactory>
#include <QAbstractEventDispatcher>

namespace wgt
{
IComponentContext* QtDesktopApplication::globalContext_ = nullptr;

namespace
{
class IdleLoop : public QObject
{
public:
	IdleLoop(QtDesktopApplication& qtApplication, QObject* parent) : QObject(parent), qtApplication_(qtApplication)
	{
		timer_ = new QTimer(this);
		timer_->setInterval(10);
		QObject::connect(timer_, &QTimer::timeout, [&]() { qtApplication_.update(); });
	}

public slots:
	void start()
	{
		qtApplication_.update();
		timer_->start();
	}

	void stop()
	{
		timer_->stop();
	}

private:
	QtDesktopApplication& qtApplication_;
	QTimer* timer_;
};
}

QtDesktopApplication::QtDesktopApplication(IComponentContext& context, QApplication* qApplication)
    : Depends<IQtFramework, ISplash>(context), application_(nullptr), splash_(nullptr), bQuit_(false)
{
	globalContext_ = &context;

	char wgtHome[MAX_PATH];

	if (Environment::getValue<MAX_PATH>("WGT_HOME", wgtHome))
	{
		QCoreApplication::addLibraryPath(wgtHome);
	}

	application_ = qApplication;

	// all this qapplication setup should probably be moved out of here
	// the whole point is to let the user set up all the Qt stuff
	QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	QApplication::setDesktopSettingsAware(false);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setFont(QFont("Noto Sans", 9));
	auto dispatcher = QAbstractEventDispatcher::instance();
	auto idleLoop = new IdleLoop(*this, application_);

	QObject::connect(dispatcher, &QAbstractEventDispatcher::aboutToBlock, idleLoop, &IdleLoop::start);
	QObject::connect(dispatcher, &QAbstractEventDispatcher::awake, idleLoop, &IdleLoop::stop);

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
}

QtDesktopApplication::~QtDesktopApplication()
{
}

void QtDesktopApplication::initialise()
{
	auto qtFramework = this->get<IQtFramework>();
	assert(qtFramework != nullptr);
	if (qtFramework != nullptr)
	{
		auto palette = qtFramework->palette();
		if (palette != nullptr)
		{
			application_->setPalette(palette->toQPalette());
		}
	}
}

void QtDesktopApplication::finalise()
{
	signalUpdate.clear();
}

void QtDesktopApplication::update()
{
	// Only called while app is idle
	// App may or may not have idle time
	layoutManager_.update();
	auto qtFramework = this->get<IQtFramework>();
	if (qtFramework != nullptr)
	{
		qtFramework->incubate();
	}

	signalUpdate();
}

int QtDesktopApplication::startApplication()
{
	assert(application_ != nullptr);
	signalStartUp();
	splash_->close();
	splash_ = nullptr;
	if (bQuit_)
	{
		return 0;
	}
	return application_->exec();
}

void QtDesktopApplication::quitApplication()
{
	QApplication::quit();
	bQuit_ = true;
}

void QtDesktopApplication::addWindow(IWindow& window)
{
	layoutManager_.addWindow(window);
}

void QtDesktopApplication::removeWindow(IWindow& window)
{
	layoutManager_.removeWindow(window);
}

void QtDesktopApplication::addView(IView& view)
{
	layoutManager_.addView(view);
}

void QtDesktopApplication::removeView(IView& view)
{
	layoutManager_.removeView(view);
}

void QtDesktopApplication::addMenu(IMenu& menu)
{
	layoutManager_.addMenu(menu);
}

void QtDesktopApplication::removeMenu(IMenu& menu)
{
	layoutManager_.removeMenu(menu);
}

void QtDesktopApplication::addAction(IAction& action)
{
	layoutManager_.addAction(action);
}

void QtDesktopApplication::removeAction(IAction& action)
{
	layoutManager_.removeAction(action);
}

void QtDesktopApplication::setWindowIcon(const char* path, const char* windowId)
{
	layoutManager_.setWindowIcon(path, windowId);
}

const Windows& QtDesktopApplication::windows() const
{
	return layoutManager_.windows();
}

IComponentContext* QtDesktopApplication::getContext()
{
	return globalContext_;
}

namespace Context
{
bool deregisterInterface(IInterface* pImpl)
{
	IComponentContext* context = QtDesktopApplication::getContext();
	assert(context != nullptr);
	return context->deregisterInterface(pImpl);
}

void* queryInterface(const TypeId& name)
{
	IComponentContext* context = QtDesktopApplication::getContext();
	assert(context != nullptr);
	return context->queryInterface(name);
}

void queryInterface(const TypeId& name, std::vector<void*>& o_Impls)
{
	IComponentContext* context = QtDesktopApplication::getContext();
	assert(context != nullptr);
	return context->queryInterface(name, o_Impls);
}

} /* Namespace context*/
}