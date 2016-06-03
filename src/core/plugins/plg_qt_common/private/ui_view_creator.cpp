#include "ui_view_creator.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

namespace wgt
{
//------------------------------------------------------------------------------
UIViewCreator::UIViewCreator(IComponentContext & context)
	: Depends( context )
{
}


//------------------------------------------------------------------------------
void UIViewCreator::createView(
	const char * path,
	const ObjectHandle & context,
	std::unique_ptr< IView > & o_ResultView,
	const char * uniqueName)
{
	auto uiFrameWork = get< IUIFramework >();
	if (uiFrameWork == nullptr)
	{
		return;
	}
	auto uiApplication = get< IUIApplication >();
	uiFrameWork->createViewAsync(
		uniqueName, path, IUIFramework::ResourceType::Url, context,
		[&o_ResultView, uiApplication](std::unique_ptr< IView > & view)
	{
		o_ResultView = std::move(view);
		if (o_ResultView != nullptr)
		{
			if (uiApplication)
			{
				uiApplication->addView(*o_ResultView);
			}
		}
		else
		{
			NGT_ERROR_MSG("Failed to load qml\n");
		}
	});
}

//------------------------------------------------------------------------------
void UIViewCreator::createView(
	const char * path,
	const ObjectHandle & context,
	std::function< void(std::unique_ptr< IView > &) > functor,
	const char * uniqueName )
{
	auto uiFrameWork = get< IUIFramework >();
	if (uiFrameWork == nullptr )
	{
		return;
	}

	auto uiApplication = get< IUIApplication >();

	uiFrameWork->createViewAsync(
		uniqueName,
		path, IUIFramework::ResourceType::Url, context,
		[uiApplication, functor ](std::unique_ptr< IView > & view)
	{
		if (view)
		{
			if (uiApplication)
			{
				uiApplication->addView(*view);
			}
			functor( view );
		}
		else
		{
			NGT_ERROR_MSG("Failed to load qml\n");
		}
	});
}
} // end namespace wgt
