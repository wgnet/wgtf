#ifndef QT_DESKTOP_APPLICATION
#define QT_DESKTOP_APPLICATION

#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/layout_manager.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "interfaces/core_splash/i_splash.hpp"
#include "core_automation/interfaces/i_automation.hpp"

#include <queue>
#include <vector>
#include <unordered_map>
#include <QSplashScreen>

class QApplication;
class QLocalSocket;
class QLocalServer;

namespace wgt
{

class MultiInstanceServer :  public QObject
{
	Q_OBJECT
	std::queue<QLocalSocket *> readySockets_;
	QLocalServer *server_;

public:
	MultiInstanceServer(QLocalServer *server);
	MultiInstanceServer::~MultiInstanceServer();

	Q_SLOT void serverHasQueuedConnections();
	Q_SLOT void socketReadyForRead();

	static const std::string& getServerName();
	std::vector<std::string> queuedFiles_;
};

class QtDesktopApplication : public Implements<IUIApplication>, public Depends<IAutomation, IQtFramework, ISplash>
{
public:
	QtDesktopApplication(QApplication* qApplication, QLocalServer *server);
	~QtDesktopApplication();

	void initialise();
	void finalise();
	void update();

	// IApplication
	virtual int startApplication() override;
	virtual void quitApplication() override;
	virtual TimerId startTimer(int, TimerCallback) override;
	virtual void killTimer(TimerId) override;
	virtual void setAppSettingsName(const char* name) override;
	virtual const char* getAppSettingsName() override;

	virtual void addWindow(IWindow& window) override;
	virtual void addMenuPath(const char* path, const char* windowId) override;
	virtual void removeWindow(IWindow& window) override;
	virtual void addView(IView& view) override;
	virtual void removeView(IView& view) override;
	virtual void addMenu(IMenu& menu) override;
	virtual void removeMenu(IMenu& menu) override;
	virtual void addAction(IAction& action) override;
	virtual void removeAction(IAction& action) override;
	virtual void setWindowIcon(const char* path, const char* windowId = "") override;
	virtual void setStatusMessage(const char* message, int timeout = 0) override;
	virtual void saveWindowPreferences() override;

	virtual void setOverrideCursor(CursorId id) override;
	virtual void restoreOverrideCursor() override;
	virtual ICursorPtr createCustomCursor(const char* filename, int hotX = -1, int hotY = -1) override;

	virtual bool              splashScreenIsShowing ()                          override;
	virtual void              toggleShowSplashScreen()                          override;
	// Sets the splash_ and resets any state associated with the splash screen.
	virtual bool              setSplashScreen       (const char* const path)    override;
	virtual bool              setSplashScreenMessage(const char* const message) override;
	// Returns a pointer "belonging" to splashscreen, i.e. invalid if splash screen is changed.
	virtual const char* const getSplashScreenMessage()                          override;

	virtual std::vector<std::string> *getQueuedFileLoads() override;
	virtual const Windows&            windows() const;

private:
	QApplication* application_;
	QWidget* focusWidget_ = nullptr;
	LayoutManager layoutManager_;
	MultiInstanceServer multiInstanceServer_;

	std::string applicationSettingsName_;
	std::unique_ptr<QSplashScreen> splash_;
	std::unordered_map<TimerId, QTimer*> timers_;
	bool bQuit_;
};
}
#endif // QT_DESKTOP_APPLICATION