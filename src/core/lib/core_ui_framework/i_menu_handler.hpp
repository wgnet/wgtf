
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  menu_handler_base.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef MENU_HANDLER_BASE_H_
#define MENU_HANDLER_BASE_H_

#pragma once

#include <vector>
#include <memory>

namespace wgt
{
class IAction;
class IUIFramework;
class IUIApplication;

class IMenuHandler;
typedef std::unique_ptr<IMenuHandler> MenuHandlerPtr;

class IMenuHandler
{
public:
	virtual ~IMenuHandler();
	void setActionsVisible(bool show);

protected:
	typedef std::vector<std::unique_ptr<IAction>> Actions;

	IMenuHandler();

	virtual void registerActions(IUIFramework& uiFramework, Actions& actions) = 0;
	virtual void addAction(std::unique_ptr<IAction> action);
	virtual void removeAction(IAction& action);

private:
	template <typename TMenuHandler, typename... Args>
	friend std::unique_ptr<TMenuHandler> CreateMenuHandler(Args&&...);

	void registerActions();

	struct Impl;
	std::unique_ptr< Impl > impl_;
};

template <typename TMenuHandler, typename... Args>
std::unique_ptr<TMenuHandler> CreateMenuHandler(Args&&... args)
{
	std::unique_ptr<TMenuHandler> menuHandler(std::make_unique<TMenuHandler>(std::forward<Args>(args)...));
	menuHandler->IMenuHandler::registerActions();
	return std::move(menuHandler);
}
} // end namespace wgt
#endif // MENU_HANDLER_BASE_H_
