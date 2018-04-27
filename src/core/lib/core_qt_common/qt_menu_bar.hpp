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
	QtMenuBar(QMenuBar& qMenuBar, const char* windowId);

	void addPath(const char* path) override;
	void addAction(IAction& action, const char* path) override;
	void removeAction(IAction& action) override;

	void update() override;

private:
	void removeFromMenuBar(QMenu* menu);
	QMenu* addMenuToBar(QString path);

	QStringList midMenus_;
	QStringList frontMenus_;
	QStringList backMenus_;

	QMenuBar& qMenuBar_;
};
} // end namespace wgt
#endif // QT_MENU_BAR_HPP
