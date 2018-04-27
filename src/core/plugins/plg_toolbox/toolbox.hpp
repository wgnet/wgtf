//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  toolbox.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) 2017-2017 Wargaming World, Ltd. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

#include "toolbox/i_toolbox.hpp"
#include "core_dependency_system/depends.hpp"

#include <memory>

namespace wgt
{
class ToolboxPanel;

class Toolbox : public Implements<IToolbox>
{
public:
	Toolbox(ToolboxPanel* toolboxPanel);
	~Toolbox();

	virtual void registerTool(ObjectHandleT<ITool> tool) override;
	virtual void deregisterTool(ObjectHandleT<ITool> tool) override;

	virtual bool setDefaultTool(ObjectHandleT<ITool> tool) override;
	virtual ObjectHandleT<ITool> getDefaultTool() override;
	virtual bool setActiveTool(ObjectHandleT<ITool> tool) override;
	virtual ObjectHandleT<ITool> getActiveTool() const override;
	virtual ObjectHandleT<ITool> getTool(uint64_t id) const override;

	virtual std::vector<ObjectHandleT<ITool>> getAllTools() const override;

	virtual const char* getDefaultIcon() const override;
	virtual void setDefaultIcon(const char* icon) override;

	virtual void activatePanel() override;

	virtual void init() override;
	virtual void fini() override;

	virtual Connection connectToolsetChanged(ToolsetChangedCallback callback) override;
	virtual Connection connectActiveToolChanged(ActiveToolChangedCallback callback) override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // end namespace wgt
