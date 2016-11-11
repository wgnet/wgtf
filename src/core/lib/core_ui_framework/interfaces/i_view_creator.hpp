#ifndef I_VIEW_CREATOR_HPP
#define I_VIEW_CREATOR_HPP

#include <memory>
#include <functional>

#include "core_ui_framework/i_ui_framework.hpp"

namespace wgt
{
class IView;
class IWindow;
class ObjectHandle;

class IViewCreator
{
public:
	virtual wg_future<std::unique_ptr<IView>> createView(const char* path, const Variant& context,
	                                                     const char* uniqueName = nullptr) = 0;

	virtual wg_future<std::unique_ptr<IView>> createView(const char* path, const Variant& context,
	                                                     std::function<void(IView&)>,
	                                                     const char* uniqueName = nullptr) = 0;

	virtual void createWindow(const char* path, const Variant& context, std::function<void(std::unique_ptr<IWindow>&)>,
	                          IUIFramework::ResourceType type = IUIFramework::ResourceType::Url) = 0;
};
} // end namespace wgt
#endif // I_VIEW_CREATOR_HPP
