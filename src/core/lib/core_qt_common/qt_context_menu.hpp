#ifndef QT_CONTEXT_MENU
#define QT_CONTEXT_MENU

#include "qt_menu.hpp"

class QMenu;

namespace wgt
{
class QtContextMenu : public QtMenu
{
public:
	QtContextMenu( QMenu & qMenu, QWidget * qView, const char * windowId );

	void addAction( IAction & action, const char* path ) override;
	void removeAction( IAction & action ) override;

private:
	QMenu & qMenu_;
	QWidget * qView_;
};
} // end namespace wgt
#endif // QT_CONTEXT_MENU
