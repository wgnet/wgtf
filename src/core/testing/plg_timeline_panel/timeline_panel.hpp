#pragma once
#ifndef _TIMELINE_PANEL_HPP
#define _TIMELINE_PANEL_HPP
 
#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "timeline_model.hpp"
 
#include <memory>
 
namespace wgt
{
class TimelinePanel
    : Depends< IUIApplication, IViewCreator >
{
public:
    TimelinePanel( IComponentContext & context );
 
    bool addPanel();
    void removePanel();
 
private:
    wg_future<std::unique_ptr< IView >> timelineView_;
	TimelineModel timelineModel_;
};
 
} // end namespace wgt
#endif // _TIMELINE_PANEL_HPP
