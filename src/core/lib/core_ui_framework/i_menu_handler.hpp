
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
class IComponentContext;
class IUIFramework;

class IMenuHandler;
typedef std::unique_ptr<IMenuHandler> MenuHandlerPtr;

class IMenuHandler
{
public:
	virtual ~IMenuHandler();

protected:
	typedef std::vector<std::unique_ptr<IAction>> Actions;

	IMenuHandler(IComponentContext& contextManager);

	virtual void registerActions(IUIFramework& uiFramework, Actions& actions) = 0;

	void addAction(std::unique_ptr<IAction> action);
	void removeAction(IAction& action);

private:
	// When VS2012 support is retired change this code to use a variadic template
	template <class TMenuHandler, class TArg>
	friend MenuHandlerPtr CreateMenuHandler(TArg&&);
	template <class TMenuHandler, class TArg1, class TArg2>
	friend MenuHandlerPtr CreateMenuHandler(TArg1&&, TArg2&&);
	template <class TMenuHandler, class TArg1, class TArg2, class TArg3>
	friend MenuHandlerPtr CreateMenuHandler(TArg1&&, TArg2&&, TArg3&&);
	template <class TMenuHandler, class TArg1, class TArg2, class TArg3, class TArg4>
	friend MenuHandlerPtr CreateMenuHandler(TArg1&&, TArg2&&, TArg3&&, TArg4&&);

	void registerActions();

	IComponentContext& contextManager_;
	Actions actions_;
};

template <class TMenuHandler, class TArg>
MenuHandlerPtr CreateMenuHandler(TArg&& arg)
{
	MenuHandlerPtr menuHandler(new TMenuHandler(std::forward<TArg>(arg)));
	menuHandler->registerActions();
	return std::move(menuHandler);
}

template <class TMenuHandler, class TArg1, class TArg2>
MenuHandlerPtr CreateMenuHandler(TArg1&& arg1, TArg2&& arg2)
{
	MenuHandlerPtr menuHandler(new TMenuHandler(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2)));
	menuHandler->registerActions();
	return std::move(menuHandler);
}

template <class TMenuHandler, class TArg1, class TArg2, class TArg3>
MenuHandlerPtr CreateMenuHandler(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3)
{
	MenuHandlerPtr menuHandler(
	new TMenuHandler(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3)));
	menuHandler->registerActions();
	return std::move(menuHandler);
}

template <class TMenuHandler, class TArg1, class TArg2, class TArg3, class TArg4>
MenuHandlerPtr CreateMenuHandler(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4)
{
	MenuHandlerPtr menuHandler(new TMenuHandler(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2),
	                                            std::forward<TArg3>(arg3), std::forward<TArg4>(arg4)));
	menuHandler->registerActions();
	return std::move(menuHandler);
}
} // end namespace wgt
#endif // MENU_HANDLER_BASE_H_
