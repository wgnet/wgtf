#include "viewport.hpp"

#include "core_common/assert.hpp"
#include "core_environment_system/i_env_system.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/layout_hint.hpp"
#include "core_ui_framework/layout_tags.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
Viewport::Viewport(std::string name, std::string resourcePath, Variant context,
                   IUIFramework::ResourceType resourceType)
    : name_(name), resourcePath_(resourcePath), resourceType_(resourceType), contextObj_(context)
{
}

Viewport::~Viewport()
{
	finalise();
}

void Viewport::finalise()
{
	if (view_.valid())
	{
		const auto& view = view_.get();
		get<IUIApplication>()->removeView(*view);
		focusChangedConnection_.disconnect();
		view_ = wg_shared_future<std::unique_ptr<IView>>();
	}
}

void Viewport::initialise(const std::string& id)
{
	id_ = id;
	if (isLoaded_)
	{
		return;
	}
	if (!resourcePath_.empty())
	{
		auto uiFramework = get<IUIFramework>();
		auto uiApplication = get<IUIApplication>();
		TF_ASSERT(uiFramework);
		TF_ASSERT(uiApplication);

		auto onViewLoad = [this, uiApplication](IView& view) {
			const auto hints = view.hint().hints();
			TF_ASSERT(hints.size() == 1);
			TF_ASSERT(hints.find(IViewport::viewportTag()) != hints.end());

			auto focusChangedCallback = [this](bool focus)
			{
				if (!focus)
				{
					return;
				}

				onFocusIn();
			};

			if (!name_.empty())
			{
				view.title(name_.c_str());
			}

			focusChangedConnection_ = view.connectFocusChanged(focusChangedCallback);
			uiApplication->addView(view);
			onViewportLoaded(view);

			if (isSelected_)
			{
				view.setFocus(true);
			}

			isLoaded_ = true;
		};

		view_ = uiFramework->createViewAsync(id != "" ? id_.c_str() : nullptr, resourcePath_.c_str(), resourceType_, contextObj_, onViewLoad);
	}
}

std::string Viewport::getId() const
{
	TF_ASSERT(!id_.empty());
	return id_;
}

void Viewport::reset()
{
	if (isLoaded_ && view_.valid())
	{
		const auto& view = view_.get();
		view->reload();

		if (!name_.empty())
		{
			view->title(name_.c_str());
		}
	}
}

void Viewport::onSelected(bool selected)
{
	isSelected_ = selected;
	if (isLoaded_ && view_.valid())
	{
		const auto& view = view_.get();
		view->setFocus(selected);
	}
}

void Viewport::onViewportLoaded(IView& view)
{
}

void Viewport::onFocusIn()
{
	if (!isSelected_)
	{
		get<IEnvManager>()->switchEnvironment(id_);
	}
}

uintptr_t Viewport::getNativeWindowHandle()
{
	if (!view_.valid())
	{
		return 0;
	}
	return view_.get()->getNativeWindowHandle();
}

void Viewport::setName(const std::string& name)
{
	name_ = name;
	if (isLoaded_ && view_.valid())
	{
		view_.get()->title(name_.c_str());
	}
}

const std::string& Viewport::getName() const
{
	return name_;
}

}