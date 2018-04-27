#ifndef QT_DOCK_REGION_HPP
#define QT_DOCK_REGION_HPP

#include "core_ui_framework/i_region.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/layout_tags.hpp"
#include "core_dependency_system/depends.hpp"
#include <map>
#include <memory>

class QDockWidget;
class QWidget;
class QByteArray;

namespace wgt
{
class IQtFramework;
class IUIApplication;
class IComponentContext;
class QtWindow;

class QtDockRegion : public IRegion, public Depends<IQtFramework, IUIApplication>
{
public:
	QtDockRegion(QtWindow& qtWindow, QDockWidget& qDockWidget);

	const LayoutTags& tags() const override;

	void addView(IView& view) override;
	void removeView(IView& view) override;
	void restoreDockWidgets();

private:
	void setDefaultPreferenceForDockWidget(QDockWidget* qDockWidget);
	const QByteArray* getWidgetPersistentGeometry(const char* widgetName) const;
	void setWidgetPersistentGeometry(const char* widgetName, const QByteArray& geometry);

	QtWindow& qtWindow_;
	QDockWidget& qDockWidget_;
	LayoutTags tags_;
	std::string menuPath_;
	bool hidden_;
	std::map<IView*, std::unique_ptr<QDockWidget>> dockWidgetMap_;
	typedef std::pair<QDockWidget*, IView*> DockPair;
	std::vector<DockPair> needToRestorePreference_;
	std::map<std::string, QByteArray> persistentWidgetGeometry_;
};
} // end namespace wgt
#endif // QT_DOCK_REGION_HPP
