
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  menu_handler_base.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "i_menu_handler.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include <assert.h>

namespace wgt
{
IMenuHandler::IMenuHandler(IComponentContext& contextManager)
	: contextManager_(contextManager)
{
	auto uiFramework = contextManager_.queryInterface< IUIFramework >();
	assert(uiFramework != nullptr);

	auto uiApplication = contextManager_.queryInterface< IUIApplication >();
	assert(uiApplication != nullptr);
}

IMenuHandler::~IMenuHandler()
{
	// Actions clean up
	auto uiApplication = contextManager_.queryInterface< IUIApplication >();
	assert(uiApplication != nullptr);

	if(!uiApplication)
		return;
	
	for ( auto itr = actions_.begin(); itr != actions_.end(); ++itr )
	{
		uiApplication->removeAction(**itr);
		( *itr ).reset();
	}
}

void IMenuHandler::registerActions()
{
	auto uiFramework = contextManager_.queryInterface< IUIFramework >();
	auto uiApplication = contextManager_.queryInterface< IUIApplication >();
	assert(uiFramework != nullptr && uiApplication != nullptr);

	if(!uiFramework || !uiApplication)
		return;

	registerActions(*uiFramework, actions_);
	
	// Add the actions to the application
	for ( auto itr = actions_.cbegin(); itr != actions_.cend(); ++itr )
	{
		uiApplication->addAction(**itr);
	}
}
} // end namespace wgt
