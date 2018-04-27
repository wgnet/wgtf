//-----------------------------------------------------------------------------
//
//  toolbox_panel.hpp
//
//-----------------------------------------------------------------------------
//  Copyright (c) 2017-2017 Wargaming World, Ltd. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "core_dependency_system/depends.hpp"
#include <core_dependency_system/i_interface.hpp>

namespace wgt
{
class IEngineAdapter;
class IObjectManager;
class IDefinitionManager;
class IUIApplication;
class IViewCreator;
class IToolbox;

class ToolboxPanel : public Depends<IUIApplication, IViewCreator, IDefinitionManager, IToolbox>
{
public:
	ToolboxPanel();
	~ToolboxPanel();

	void init();
	void fini();

	void activate();

private:
	struct Impl;
	std::unique_ptr<struct Impl> impl_;
};

} // end namespace wgt
