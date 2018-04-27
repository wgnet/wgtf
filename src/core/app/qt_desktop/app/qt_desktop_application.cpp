#include "qt_desktop_application.hpp"
#include "core_common/assert.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_env.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_qt_common/qt_cursor.hpp"
#include "core_logging/logging.hpp"

#include <QtNetwork>
#include <QTimer>
#include <QApplication>
#include <QCoreApplication>
#include <QStyleFactory>
#include <QAbstractEventDispatcher>

namespace wgt
{

MultiInstanceServer::MultiInstanceServer(QLocalServer *server) 
	: server_(server)
{
}

MultiInstanceServer::~MultiInstanceServer() 
{
	if (server_) server_->close();
}

void MultiInstanceServer::serverHasQueuedConnections() 
{
	QLocalSocket *socket = server_->nextPendingConnection();
	readySockets_.push(socket);
	QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyForRead()));
}

void MultiInstanceServer::socketReadyForRead()
{
	QLocalSocket *socket = readySockets_.front();
	if (!socket) return;

	QDataStream in(socket); in.setVersion(QDataStream::Qt_5_6);
	QString message; in >> message;

	QFileInfo path(message);
	if (path.exists() && path.isFile()) 
	{
		QString filePath = path.filePath();
		queuedFiles_.push_back(filePath.toUtf8().data());
	}

	socket->disconnectFromServer();
	readySockets_.pop();
}

const std::string& MultiInstanceServer::getServerName() {
	static std::string serverName;
	if (serverName.empty()) 
	{
		char userName[MAX_PATH] = {};
		if (!Environment::getValue<MAX_PATH>("USERNAME", userName))
			Environment::getValue<MAX_PATH>("USER", userName);
		serverName.append(userName);
		serverName.append("_WGTitanEditor_");
	}

	return serverName;
}

QtDesktopApplication::QtDesktopApplication(QApplication* qApplication, QLocalServer *server)
	: application_(qApplication), splash_(nullptr), bQuit_(false), multiInstanceServer_(server)
{
	char wgtHome[MAX_PATH];
	if (Environment::getValue<MAX_PATH>("WGT_HOME", wgtHome))
	{
		QCoreApplication::addLibraryPath(wgtHome);
	}

	// Startup multiple instance servers
	if (server)
	{
		QObject::connect(server, SIGNAL(newConnection()), &multiInstanceServer_, SLOT(serverHasQueuedConnections()));
		if (!server->isListening())
		{
			server->setSocketOptions(QLocalServer::UserAccessOption);
			QString serverName = QString(multiInstanceServer_.getServerName().c_str());
			if (!server->listen(serverName)) 
			{
				switch (server->serverError()) 
				{
					case QAbstractSocket::AddressInUseError:
					case QAbstractSocket::SocketAddressNotAvailableError:
					{
						// Try recover once, otherwise move on.
						QLocalServer::removeServer(serverName);
						server->listen(serverName);
						break;
					}

					default: break;
				}
			}
		}
	}

	// Queue load any files sitting on the argv line
	{
		QStringList args = qApplication->arguments();
		for (int i = 1; i < args.count(); i++) 
		{
			const QString& arg = args[i];
			QFileInfo path(arg);

			if (path.exists() && path.isFile())
			{
				std::string argv = arg.toUtf8().toStdString();
				multiInstanceServer_.queuedFiles_.push_back(argv);
			}
		}
	}

	// all this qapplication setup should probably be moved out of here
	// the whole point is to let the user set up all the Qt stuff
	QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	QApplication::setDesktopSettingsAware(false);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setFont(QFont("Noto Sans", 9));

	startTimer(10, [this]() { update(); });
}

QtDesktopApplication::~QtDesktopApplication()
{
	for (auto& timer : timers_)
	{
		timer.second->stop();
		timer.second->deleteLater();
	}
}

void QtDesktopApplication::initialise()
{
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

void QtDesktopApplication::finalise()
{
	signalUpdate.clear();
	signalStartUp.clear();
}

void QtDesktopApplication::update()
{
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
	TF_ASSERT(application_ != nullptr);
	signalStartUp();
	layoutManager_.loadWindowPreferences();

	if(splash_)
	{
		splash_->close();
	}

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

void QtDesktopApplication::quitApplication()
{
	signalExit();
	QApplication::quit();
	splash_ = nullptr;
	bQuit_ = true;
}

IApplication::TimerId QtDesktopApplication::startTimer(int interval_ms, TimerCallback callback)
{
	if (application_ == nullptr)
		return 0;

	auto timer = new QTimer(application_);
	QObject::connect(timer, &QTimer::timeout, [callback]() { callback(); });
	timer->start(interval_ms);
	timers_[timer->timerId()] = timer;
	return timer->timerId();
}

void QtDesktopApplication::killTimer(TimerId id)
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

void QtDesktopApplication::setAppSettingsName(const char* name)
{
	applicationSettingsName_ = name;
}

const char* QtDesktopApplication::getAppSettingsName()
{
	return applicationSettingsName_.c_str();
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

void QtDesktopApplication::addMenuPath(const char* path, const char* windowId)
{
	layoutManager_.addMenuPath(path, windowId);
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

void QtDesktopApplication::setStatusMessage(const char* message, int timeout)
{
	layoutManager_.setStatusMessage(message, timeout);
}

void QtDesktopApplication::setOverrideCursor(CursorId id)
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

void QtDesktopApplication::restoreOverrideCursor()
{
	application_->restoreOverrideCursor();
}

ICursorPtr QtDesktopApplication::createCustomCursor(const char * filename, int hotX, int hotY)
{
	return std::make_unique<QtCursor>(filename, hotX, hotY);
}

void QtDesktopApplication::saveWindowPreferences()
{
	layoutManager_.saveWindowPreferences();
}

bool QtDesktopApplication::splashScreenIsShowing()
{
	if (!splash_.get()) return false;

	bool result = splash_->isVisible();
	return result;
}

void QtDesktopApplication::toggleShowSplashScreen()
{
	if (!splash_.get()) return;

	if (splash_->isVisible()) 
	{
		splash_->hide();
	}
	else 
	{
		splash_->show();
	}
	application_->processEvents();
}

bool QtDesktopApplication::setSplashScreen(const char* const path)
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

bool QtDesktopApplication::setSplashScreenMessage(const char* const message) 
{
	if (!splash_.get()) return false;
	splash_->showMessage(message);
	application_->processEvents();

	return true;
}

const char* const QtDesktopApplication::getSplashScreenMessage() 
{
	if (splash_.get()) {
		QString qString = splash_->message();
		const char *const result = qString.toUtf8().data();
		return result;
	}

	return "";
}

std::vector<std::string> *QtDesktopApplication::getQueuedFileLoads()
{
	return &multiInstanceServer_.queuedFiles_;
}

const Windows& QtDesktopApplication::windows() const
{
	return layoutManager_.windows();
}
}