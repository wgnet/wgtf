#ifndef QT_TOOL_BAR_HPP
#define QT_TOOL_BAR_HPP

#include "qt_menu.hpp"

class QToolBar;

namespace wgt
{
class QtToolBar : public QtMenu
{
public:
	QtToolBar(QToolBar& qToolBar, const char* windowId);

	void addAction(IAction& action, const char* path) override;
	void removeAction(IAction& action) override;
	void addPath(const char* path) override
	{
	}

	void update() override;

private:
	QToolBar& qToolBar_;
};
} // end namespace wgt
#endif // QT_TOOL_BAR_HPP
