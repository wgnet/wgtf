#ifndef QT_ACTION_MANAGER_HPP
#define QT_ACTION_MANAGER_HPP

#include "core_ui_framework/action_manager.hpp"
#include <functional>
#include <memory>

namespace wgt
{
class IAction;

class QtActionManager : public ActionManager
{
	typedef ActionManager base;

public:
	explicit QtActionManager(IComponentContext& contextManager);
	~QtActionManager();

private:
	friend class QtAction;
	void onQtActionDestroy(IAction* action);
	virtual std::unique_ptr<IAction> createAction(const char* text, const char* icon, const char* windowId,
	                                              const char* path, const char* shortcut, int order,
	                                              std::function<void(IAction*)> func,
	                                              std::function<bool(const IAction*)> enableFunc,
	                                              std::function<bool(const IAction*)> checkedFunc, const char* group,
	                                              bool isSeparator = false) override;
};
} // end namespace wgt
#endif // ACTION_MANAGER_HPP
