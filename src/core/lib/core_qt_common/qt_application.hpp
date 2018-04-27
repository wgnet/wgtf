#ifndef QT_APPLICATION_HPP
#define QT_APPLICATION_HPP

#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/layout_manager.hpp"
#include "interfaces/core_splash/i_splash.hpp"
#include "core_automation/interfaces/i_automation.hpp"

#include <memory>
#include <unordered_map>

class QApplication;
class QSplashScreen;
class QTimer;

namespace wgt
{
class IComponentContext;
class IQtFramework;

class QtApplication : public Implements<IUIApplication>, public Depends<IAutomation, IQtFramework, ISplash>
{
	typedef Signal<void(void)> SignalVoid;

public:
	QtApplication(int& argc, char** argv);
	virtual ~QtApplication();

	void initialise();
	void finalise();
	void update();

	// IApplication
	int startApplication() override;
	void quitApplication() override;
	TimerId startTimer(int interval_ms, TimerCallback callback) override;
	void killTimer(TimerId id) override;
	void setAppSettingsName(const char* name) override;
	const char* getAppSettingsName() override;

	// IUIApplication
	void addWindow(IWindow& window) override;
	void addMenuPath(const char* path, const char* windowId) override;
	void removeWindow(IWindow& window) override;
	void addView(IView& view) override;
	void removeView(IView& view) override;
	void addMenu(IMenu& menu) override;
	void removeMenu(IMenu& menu) override;
	void addAction(IAction& action) override;
	void removeAction(IAction& action) override;
	void setWindowIcon(const char* path, const char* windowId = "") override;
	void setStatusMessage(const char* message, int timeout = 0) override;
	void saveWindowPreferences() override;

	void setOverrideCursor(CursorId id) override;
	void restoreOverrideCursor() override;
	virtual ICursorPtr createCustomCursor(const char* filename, int hotX = -1, int hotY = -1) override;

	bool splashScreenIsShowing () override;
	void toggleShowSplashScreen() override;
	// Sets the splash_ and resets any state associated with the splash screen.
	bool              setSplashScreen       (const char* const path)    override;
	bool              setSplashScreenMessage(const char* const message) override;
	// Returns a pointer "belonging" to splashscreen, i.e. invalid if splash screen is changed.
	const char* const getSplashScreenMessage()                          override;

	const Windows& windows()              const override;

protected:
	std::unique_ptr<QApplication> application_;

private:
	int& argc_;
	char** argv_;

	LayoutManager layoutManager_;
	std::string applicationSettingsName_;
	std::unique_ptr<QSplashScreen> splash_;
	std::unordered_map<TimerId, QTimer*> timers_;
	bool bQuit_;
};
} // end namespace wgt
#endif // QT_APPLICATION_HPP
