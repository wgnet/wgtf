#ifndef QT_TAB_REGION_HPP
#define QT_TAB_REGION_HPP

#include "core_ui_framework/i_region.hpp"
#include "core_ui_framework/layout_tags.hpp"
#include "core_ui_framework/i_view.hpp"

#include <QMetaObject>
#include <memory>

class QWidget;
class QTabWidget;

namespace wgt
{
class IWindow;

class QtTabRegion : public IRegion
{
public:
	QtTabRegion(IWindow* window, QTabWidget& qTabWidget);
	virtual ~QtTabRegion();

	const LayoutTags& tags() const override;

	void addView(IView& view) override;
	void removeView(IView& view) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // QT_DOCK_REGION_HPP
