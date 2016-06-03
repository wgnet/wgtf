#ifndef QT_TAB_REGION_HPP
#define QT_TAB_REGION_HPP

#include "core_ui_framework/i_region.hpp"
#include "core_ui_framework/layout_tags.hpp"
#include "core_ui_framework/i_view.hpp"

#include <QMetaObject>

class QWidget;
class QTabWidget;

namespace wgt
{
class IQtFramework;

class QtTabRegion : public IRegion
{
public:
	QtTabRegion( IQtFramework & qtFramework, QTabWidget & qTabWidget );
	~QtTabRegion();

	const LayoutTags & tags() const override;

	void addView( IView & view ) override;
	void removeView( IView & view ) override;

private:
	IQtFramework & qtFramework_;
	QTabWidget & qTabWidget_;
	LayoutTags tags_;
	typedef std::vector<std::pair<QWidget*, IView*>> Tabs;
	Tabs tabs_;
	QMetaObject::Connection m_connection;
	QWidget* current_;
};
} // end namespace wgt
#endif//QT_DOCK_REGION_HPP
