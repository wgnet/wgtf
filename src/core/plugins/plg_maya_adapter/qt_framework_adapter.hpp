#ifndef QT_FRAMEWORK_ADAPTER_HPP
#define QT_FRAMEWORK_ADAPTER_HPP

#include "core_qt_common/qt_framework.hpp"

namespace wgt
{
class QtFrameworkAdapter : public QtFramework
{
public:
	QtFrameworkAdapter(IComponentContext& contextManager);
	virtual ~QtFrameworkAdapter();
	virtual QtWindow* createQtWindow(QIODevice& source) override;
};
} // end namespace wgt
#endif
