#ifndef LAYOUT_MANAGER_HPP
#define LAYOUT_MANAGER_HPP
#include "core_ui_framework/i_ui_application.hpp"

#include <map>
#include <string>
#include <vector>
#include <set>

namespace wgt
{
class IAction;
class IMenu;
class IRegion;
class IView;
class IWindow;
struct LayoutHint;

class LayoutManager
{
public:
	LayoutManager();
	~LayoutManager();

	void update() const;

	void addAction( IAction & action );
	void addMenu( IMenu & menu );
	void addView( IView & view );
	void addWindow( IWindow & window );

	void removeAction( IAction & action );
	void removeMenu( IMenu & menu );
	void removeView( IView & view );
	void removeWindow( IWindow & window );

	void setWindowIcon( const char* path, const char* windowId );
	const Windows & windows() const;

private:
	bool matchMenu( IMenu & menu, const char * path );

	std::vector< IMenu * > findAllMenus( IWindow & window, const char * path );
	IRegion * findBestRegion( IWindow & window, const LayoutHint & hint );

	void addAction( IAction & action, IWindow & window );
	void addView( IView & view, IWindow & window );

	void refreshActions( IWindow & window );
	void refreshViews( IWindow & window );

	void removeActions( IWindow & window );
	void removeViews( IWindow & window );

	IWindow * getWindow( const char * windowId );

	std::map< IAction *, std::set<IMenu *> > actions_;
	std::map< IView *, IRegion * > views_;
	std::vector< IMenu * > dynamicMenus_;
	Windows windows_;
};
} // end namespace wgt
#endif//LAYOUT_MANAGER_HPP
