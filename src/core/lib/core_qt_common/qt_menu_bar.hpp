#ifndef QT_MENU_BAR_HPP
#define QT_MENU_BAR_HPP

#include "qt_menu.hpp"

class QMenu;
class QMenuBar;

namespace wgt
{
class QtMenuBar : public QtMenu
{
public:
	QtMenuBar( QMenuBar & qMenuBar, const char * windowId );

	void addAction( IAction & action, const char* path ) override;
	void removeAction( IAction & action ) override;

private:
	QMenuBar & qMenuBar_;
};
} // end namespace wgt
#endif//QT_MENU_BAR_HPP
