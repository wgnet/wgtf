#ifndef UI_VIEW_CREATOR_HPP
#define UI_VIEW_CREATOR_HPP

#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>

namespace wgt
{
class ObjectHandle;
class IComponentContext;
class IUIFramework;

class UIViewCreator : public Implements<IViewCreator>, public Depends<IUIFramework, IUIApplication>
{
public:
	UIViewCreator(IComponentContext& context);

	wg_future<std::unique_ptr<IView>> createView(const char* path, const Variant& context,
	                                             const char* uniqueName) override;

	wg_future<std::unique_ptr<IView>> createView(const char* path, const Variant& context, std::function<void(IView&)>,
	                                             const char* uniqueName) override;

	void createWindow(const char* path, const Variant& context, std::function<void(std::unique_ptr<IWindow>&)>,
	                  IUIFramework::ResourceType type) override;
};
} // end namespace wgt
#endif // UI_VIEW_CREATOR_HPP
