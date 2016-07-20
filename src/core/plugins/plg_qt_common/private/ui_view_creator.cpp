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
wg_future<std::unique_ptr< IView >> UIViewCreator::createView(
	const char * path,
	const ObjectHandle & context,
	const char * uniqueName)
{
	auto uiFrameWork = get< IUIFramework >();
	if (uiFrameWork == nullptr)
	{
        return std::future<std::unique_ptr<IView>>();
	}
	auto uiApplication = get< IUIApplication >();
	auto view = uiFrameWork->createViewAsync(
		uniqueName, path, IUIFramework::ResourceType::Url, context,
		[uiApplication](IView & view)
	{
		if (uiApplication)
		{
			uiApplication->addView(view);
		}
	});
	return view;
}

//------------------------------------------------------------------------------
wg_future<std::unique_ptr< IView >> UIViewCreator::createView(
	const char * path,
	const ObjectHandle & context,
	std::function< void(IView &) > functor,
	const char * uniqueName )
{
	auto uiFrameWork = get< IUIFramework >();
	if (uiFrameWork == nullptr )
	{
        return std::future<std::unique_ptr<IView>>();
	}

	auto uiApplication = get< IUIApplication >();

	auto view = uiFrameWork->createViewAsync(
		uniqueName,
		path, IUIFramework::ResourceType::Url, context,
		[uiApplication, functor ](IView & view)
	{
		if (uiApplication)
		{
			uiApplication->addView(view);
		}
		functor( view );
	});
	return view;
}


//------------------------------------------------------------------------------
void UIViewCreator::createWindow(
	const char * path,
	const ObjectHandle & context,
	std::function< void(std::unique_ptr< IWindow > &) > functor,
	IUIFramework::ResourceType type )
{
	auto uiFrameWork = get< IUIFramework >();
	if (uiFrameWork == nullptr )
	{
		return;
	}

	auto uiApplication = get< IUIApplication >();

	uiFrameWork->createWindowAsync(
		path, type, context,
		[uiApplication, functor ](std::unique_ptr< IWindow > & view)
	{
		if (view)
		{
			uiApplication->addWindow( *view );
			functor( view );
		}
		else
		{
			NGT_ERROR_MSG("Failed to load qml\n");
		}
	});
}
} // end namespace wgt
