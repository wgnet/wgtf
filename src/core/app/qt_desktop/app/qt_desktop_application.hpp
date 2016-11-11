#ifndef QT_DESKTOP_APPLICATION
#define QT_DESKTOP_APPLICATION

#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/layout_manager.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "interfaces/core_splash/i_splash.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include <QSplashScreen>

class QApplication;

namespace wgt
{
class QtDesktopApplication : public Implements<IUIApplication>, public Depends<IQtFramework, ISplash>
{
public:
	QtDesktopApplication(IComponentContext& context, QApplication* qApplication);
	~QtDesktopApplication();
	virtual int startApplication();
	virtual void quitApplication();

	void initialise();
	void finalise();
	void update();

	virtual void addWindow(IWindow& window);
	virtual void removeWindow(IWindow& window);
	virtual void addView(IView& view);
	virtual void removeView(IView& view);
	virtual void addMenu(IMenu& menu);
	virtual void removeMenu(IMenu& menu);
	virtual void addAction(IAction& action);
	virtual void removeAction(IAction& action);
	virtual void setWindowIcon(const char* path, const char* windowId = "");
	virtual const Windows& windows() const;

	static IComponentContext* getContext();

private:
	Windows windows_;
	QApplication* application_;
	LayoutManager layoutManager_;

	std::unique_ptr<QSplashScreen> splash_;
	bool bQuit_;

	static IComponentContext* globalContext_;
};
}
#endif // QT_DESKTOP_APPLICATION