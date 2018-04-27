#ifndef I_UI_APPLICATION_HPP
#define I_UI_APPLICATION_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_ui_framework/i_cursor.hpp"
#include "layout_hint.hpp"

#include <map>
#include <vector>
#include <memory>

namespace wgt
{
class IAction;
class IComponent;
class IMenu;
class IWindow;
class IView;

using Windows = std::map<std::string, IWindow*>;

class IUIApplication : public Implements<IApplication>
{
public:
	virtual ~IUIApplication()
	{
	}

	virtual void addWindow(IWindow& window) = 0;
	virtual void addMenuPath(const char* path, const char* windowId) = 0;
	virtual void removeWindow(IWindow& window) = 0;
	virtual void addView(IView& view) = 0;
	virtual void removeView(IView& view) = 0;
	virtual void addMenu(IMenu& menu) = 0;
	virtual void removeMenu(IMenu& menu) = 0;
	virtual void addAction(IAction& action) = 0;
	virtual void removeAction(IAction& action) = 0;
	virtual void setWindowIcon(const char* path, const char* windowId = "") = 0;
	virtual void setStatusMessage(const char* message, int timeout = 0) = 0;
	virtual void setOverrideCursor(CursorId cursor) = 0;
	virtual void restoreOverrideCursor() = 0;
	virtual ICursorPtr createCustomCursor(const char * filename, int hotX, int hotY) = 0;
	virtual void saveWindowPreferences() = 0;

	virtual bool              splashScreenIsShowing ()                          = 0;
	virtual void              toggleShowSplashScreen()                          = 0;
	virtual bool              setSplashScreen       (const char* const path)    = 0;
	virtual bool              setSplashScreenMessage(const char* const message) = 0;
	virtual const char* const getSplashScreenMessage()                          = 0;

	virtual std::vector<std::string> *getQueuedFileLoads() { return nullptr;  }

	virtual const Windows& windows()              const = 0;
};
} // end namespace wgt
#endif // I_UI_APPLICATION_HPP
