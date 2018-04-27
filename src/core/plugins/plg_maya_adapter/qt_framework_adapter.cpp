#include "qt_framework_adapter.hpp"
#include "qt_window_adapter.hpp"

namespace wgt
{
QtFrameworkAdapter::QtFrameworkAdapter(IComponentContext& contextManager) : QtFramework(contextManager)
{
}

QtFrameworkAdapter::~QtFrameworkAdapter()
{
}

QtWindow* QtFrameworkAdapter::createQtWindow(QIODevice& source)
{
	return new QtWindowAdapter(source);
}
} // end namespace wgt
