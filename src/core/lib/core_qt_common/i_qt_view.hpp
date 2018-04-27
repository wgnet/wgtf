#ifndef I_QT_VIEW_HPP
#define I_QT_VIEW_HPP

#include "core_ui_framework/i_view.hpp"

class QWidget;

namespace wgt
{
class IQtView : public IView
{
public:
	virtual ~IQtView()
	{
	}

	virtual QWidget* releaseWidget() = 0;
	virtual void retainWidget() = 0;
	virtual QWidget* widget() const = 0;
};
} // end namespace wgt
#endif // I_QT_VIEW_HPP
