#pragma once

#include "interfaces/core_viewport/i_viewport.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_environment_system/i_env_system.hpp"

#include "core_ui_framework/i_view.hpp"
#include "core_common/wg_future.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class Viewport : public IViewport
	, public Depends< IUIApplication, IUIFramework, IEnvManager >
{
public:
	Viewport(std::string name, std::string resourcePath, Variant context = Variant(),
	         IUIFramework::ResourceType resourceType = IUIFramework::ResourceType::Url);
	virtual ~Viewport();

	virtual void onViewportLoaded(IView& view) override;
	virtual std::string getId() const override final;
	virtual void onSelected(bool selected) override;
	virtual void reset() override final;
	virtual void initialise(const std::string& id) override final;
	virtual void finalise() override;
	virtual uintptr_t getNativeWindowHandle() final;
	virtual void setName(const std::string& name);
	virtual const std::string& getName() const override;

protected:
	wg_shared_future<std::unique_ptr<IView>> view_;
	bool isLoaded_ = false;
	bool isSelected_ = false;
	std::string name_;

	virtual void onFocusIn();

private:
	std::string id_;
	std::string resourcePath_;
	IUIFramework::ResourceType resourceType_;
	Variant contextObj_;
	IComponentContext* context_ = nullptr;
	Connection focusChangedConnection_;
};
}
