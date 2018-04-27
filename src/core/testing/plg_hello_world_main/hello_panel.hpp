#pragma once
#ifndef _HELLO_PANEL_HPP
#define _HELLO_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_object/managed_object.hpp"
#include "core_common/wg_future.hpp"
#include <memory>

namespace wgt
{
class HelloPanelExposedSource;
class IUIFramework;
class IUIApplication;
class IDefinitionManager;
class IViewCreator;

class HelloPanel : Depends<IUIFramework, IUIApplication, IDefinitionManager, IViewCreator>
{
public:
	bool addPanel();
	void removePanel();

private:
	wg_future<std::unique_ptr<IView>> helloView_;
    ManagedObject<HelloPanelExposedSource> helloPanelExposed_;
};
} // end namespace wgt

#endif // _HELLO_PANEL_HPP