#ifndef ACTION_MANAGER_HPP
#define ACTION_MANAGER_HPP

#include <memory>
#include <functional>
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "interfaces/i_action_manager.hpp"

namespace wgt
{
class IWindow;
class AbstractListModel;
class ObjectHandle;

class ActionManager : public Implements<IActionManager>
					, public Depends<IUIFramework, IDefinitionManager, IUIApplication>
{
public:
	ActionManager();
	virtual ~ActionManager();

	void init();
	void fini();

	virtual IAction* findAction(const char* id) const override;

	std::unique_ptr<IAction> createAction(const char* id, std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc,
										  std::function<bool(const IAction*)> visibleFunc);

	std::unique_ptr<IAction> createAction(const char* id, const char* text, const char* path, int actionOrder,
	                                      std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc,
										  std::function<bool(const IAction*)> visibleFunc);

	std::unique_ptr<IAction> createSeperator(const char* id, const char* path, int actionOrder);
	std::unique_ptr<IAction> createSeperator(const char* id);

	void loadActionData(IDataStream& dataStream);
	ObjectHandle getContextObject() const;
	void registerEventHandler(IWindow* window);

protected:
	void onActionDestroyed(IAction* action);

private:
	virtual std::unique_ptr<IAction> createAction(const char* id, const char* text, const char* icon, const char* windowId,
	                                              const char* path, const char* shortcut, int order,
	                                              std::function<void(IAction*)> func,
	                                              std::function<bool(const IAction*)> enableFunc,
	                                              std::function<bool(const IAction*)> checkedFunc, 
												  std::function<bool(const IAction*)> visibleFunc, const char* group,
	                                              bool isSeparator = false) = 0;

	struct Impl;
	std::unique_ptr<Impl> impl_;

	static int s_defaultActionOrder;
};
} // end namespace wgt
#endif // ACTION_MANAGER_HPP
