
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  menu_handler_base.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "i_menu_handler.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{

struct IMenuHandler::Impl
	: Depends< IUIFramework, IUIApplication >
{
	Impl()
	{}

	Actions actions_;
};


//==============================================================================
IMenuHandler::IMenuHandler()
	: impl_( new Impl() )
{
}

IMenuHandler::~IMenuHandler()
{
	// Actions clean up
	auto uiApplication = impl_->get<IUIApplication>();
	TF_ASSERT(uiApplication != nullptr);

	if (!uiApplication)
		return;

	for (auto itr = impl_->actions_.begin(); itr != impl_->actions_.end(); ++itr)
	{
		uiApplication->removeAction(**itr);
		(*itr).reset();
	}
}

void IMenuHandler::setActionsVisible(bool show)
{
	auto uiApplication = impl_->get<IUIApplication>();
	TF_ASSERT(uiApplication != nullptr);

	if (!uiApplication)
		return;

	for (auto itr = impl_->actions_.begin(); itr != impl_->actions_.end(); ++itr)
	{
		uiApplication->removeAction(**itr);
	}
	if (show)
	{
		for (auto itr = impl_->actions_.begin(); itr != impl_->actions_.end(); ++itr)
		{
			uiApplication->addAction(**itr);
		}
	}
}

void IMenuHandler::registerActions()
{
	auto uiFramework = impl_->get<IUIFramework>();
	auto uiApplication = impl_->get<IUIApplication>();
	TF_ASSERT(uiFramework != nullptr && uiApplication != nullptr);

	if (!uiFramework || !uiApplication)
		return;

	registerActions(*uiFramework, impl_->actions_);

	for (auto itr = impl_->actions_.cbegin(); itr != impl_->actions_.cend(); ++itr)
	{
		// Remove the action from the application in case is has already been added
		uiApplication->removeAction(**itr);
		// Add the action to the application
		uiApplication->addAction(**itr);
	}
}

void IMenuHandler::addAction(std::unique_ptr<IAction> action)
{
	impl_->actions_.emplace_back(std::move(action));

	auto uiApplication = impl_->get<IUIApplication>();
	if (uiApplication)
	{
		// Add the action to the application
		uiApplication->addAction(**impl_->actions_.rbegin());
	}
}

void IMenuHandler::removeAction(IAction& action)
{
	auto found = std::find_if(impl_->actions_.begin(), impl_->actions_.end(),
	                          [&action](const std::unique_ptr<IAction>& cur) { return cur.get() == &action; });

	if (found == impl_->actions_.end())
		return;

	auto uiApplication = impl_->get<IUIApplication>();
	if (uiApplication)
	{
		// Remove the action from the application
		uiApplication->removeAction(action);
	}

	impl_->actions_.erase(found);
}

} // end namespace wgt
