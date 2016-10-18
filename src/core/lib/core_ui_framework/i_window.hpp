#ifndef I_WINDOW_HPP
#define I_WINDOW_HPP

#include "core_common/signal.hpp"

#include <vector>
#include <memory>

namespace wgt
{
class IMenu;
class IRegion;
class IUIApplication;
class IStatusBar;

typedef std::vector< std::unique_ptr<IMenu> > Menus;
typedef std::vector< std::unique_ptr<IRegion> > Regions;

class IWindow
{
	typedef Signal<void( void )> SignalVoid;
	typedef Signal<void( bool& )> SignalConfirm;

public:
	virtual ~IWindow() {}

	virtual const char * id() const = 0;
	virtual const char * title() const = 0;

	virtual void update() = 0;
	virtual void close() = 0;

	virtual void setIcon(const char* path) = 0;
	virtual void show( bool wait = false ) = 0;
	virtual void showMaximized( bool wait = false ) = 0;
	virtual void showModal() {}
	virtual void hide() = 0;
	virtual void title(const char* title) = 0;

	virtual const Menus & menus() const = 0;
	virtual const Regions & regions() const = 0;
	virtual IStatusBar* statusBar() const = 0;

	SignalVoid signalClose;
	SignalConfirm signalTryClose;
};
} // end namespace wgt
#endif //I_WINDOW_HPP
