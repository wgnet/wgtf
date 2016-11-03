#ifndef QT_TAB_REGION_HPP
#define QT_TAB_REGION_HPP

#include "core_ui_framework/i_region.hpp"
#include "core_ui_framework/layout_tags.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_dependency_system/depends.hpp"
#include <QMetaObject>

class QWidget;
class QTabWidget;

namespace wgt
{
class IQtFramework;
class IComponentContext;
class QtTabRegion : public IRegion, public Depends<IQtFramework>, public IViewEventListener
{
public:
	QtTabRegion( IComponentContext & context, QTabWidget & qTabWidget );
	~QtTabRegion();

	const LayoutTags & tags() const override;

	void addView( IView & view ) override;
	void removeView( IView & view ) override;

private:
	typedef std::vector<std::pair<QWidget*, IView*>> Tabs;

	virtual void onFocusIn(IView*) override;
	virtual void onFocusOut(IView*) override;
	virtual void onLoaded(IView* view) override;

	/**
    * Finds the tab from the view or end if not found
    */
	Tabs::iterator findTabFromView(IView& view);

	/**
    * Sets the tab title text
    */
	void setTabTitle(int index, const char* text);

	QTabWidget & qTabWidget_;
	LayoutTags tags_;
	Tabs tabs_;
	QMetaObject::Connection m_connection;
	QWidget* current_;
};
} // end namespace wgt
#endif//QT_DOCK_REGION_HPP
