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

namespace wgt
{
class IQtFramework;
class IUIApplication;
class IComponentContext;
class QtWindow;

class QtDockRegion : public IRegion
				   , public Depends<IQtFramework, IUIApplication>
{
public:
	QtDockRegion( IComponentContext & context, QtWindow & qtWindow, QDockWidget & qDockWidget );

	const LayoutTags & tags() const override;

	void addView( IView & view ) override;
	void removeView( IView & view ) override;
    void restoreDockWidgets();

private:
	void setDefaultPreferenceForDockWidget( QDockWidget * qDockWidget );

	QtWindow & qtWindow_;
	QDockWidget & qDockWidget_;
	LayoutTags tags_;
	bool hidden_;
	typedef std::pair< std::unique_ptr< QDockWidget >, std::unique_ptr< IAction > > DockData;
	std::map< IView*, DockData > dockWidgetMap_;
    typedef std::pair<QDockWidget*, IView*> DockPair;
    std::vector<DockPair> needToRestorePreference_;
};
} // end namespace wgt
#endif//QT_DOCK_REGION_HPP
