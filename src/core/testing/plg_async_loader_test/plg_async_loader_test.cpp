#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "test_model.hpp"
#include <vector>
#include "core_reflection/i_definition_manager.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* This plugin tests the asynchronous QML loader which shows a
* busy indicator when loading component in an async mode
*
* @ingroup plugins
* @image html plg_async_loader_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class AsyncLoaderTestPlugin : public PluginMain,
                              public Depends<IViewCreator, IDefinitionManager, IUIApplication, IUIFramework>
{
private:
	InterfacePtrs types_;
	std::vector<wg_future<std::unique_ptr<IView>>> views_;
	std::unique_ptr<IAction> openViews_;
	std::unique_ptr<IAction> closeViews_;
	ListDataModel listViewModel1_;
	ListDataModel listViewModel2_;
	ListDataModel listViewModel3_;

public:
	//==========================================================================
	virtual bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	//==========================================================================
	virtual void Initialise(IComponentContext& contextManager) override
	{
		auto defManager = get<IDefinitionManager>();

		listViewModel1_.init(15);
		listViewModel2_.init(30);
		listViewModel3_.init(7);

		auto uiApplication = get<IUIApplication>();
		auto uiFramework = get<IUIFramework>();
		assert((uiFramework != nullptr) && (uiApplication != nullptr));
		uiFramework->loadActionData(":/plg_async_loader_test/actions.xml", IUIFramework::ResourceType::File);
		auto viewCreator = get<IViewCreator>();
		assert(viewCreator != nullptr);

		openViews_ = uiFramework->createAction(
		"OpenViews",
		[this, viewCreator](IAction*) {
			assert(views_.empty());
			std::string id = "plg_async_loader_test/list_tab_panel_2.0.qml" + std::to_string(1);
			auto view = viewCreator->createView("plg_async_loader_test/list_tab_panel_2.0.qml",
			                                    static_cast<AbstractListModel*>(&listViewModel1_), id.c_str());
			if (view.valid())
			{
				views_.push_back(std::move(view));
			}

			id = "plg_async_loader_test/list_tab_panel_2.0.qml" + std::to_string(2);
			view = viewCreator->createView("plg_async_loader_test/list_tab_panel_2.0.qml",
			                               static_cast<AbstractListModel*>(&listViewModel2_), id.c_str());
			if (view.valid())
			{
				views_.push_back(std::move(view));
			}

			id = "plg_async_loader_test/list_tab_panel_2.0.qml" + std::to_string(3);
			view = viewCreator->createView("plg_async_loader_test/list_tab_panel_2.0.qml",
			                               static_cast<AbstractListModel*>(&listViewModel3_), id.c_str());
			if (view.valid())
			{
				views_.push_back(std::move(view));
			}

			id = "plg_async_loader_test/list_dock_panel_2.0.qml" + std::to_string(1);
			view = viewCreator->createView("plg_async_loader_test/list_dock_panel_2.0.qml",
			                               static_cast<AbstractListModel*>(&listViewModel1_), id.c_str());
			if (view.valid())
			{
				views_.push_back(std::move(view));
			}

			id = "plg_async_loader_test/list_dock_panel_2.0.qml" + std::to_string(2);
			view = viewCreator->createView("plg_async_loader_test/list_dock_panel_2.0.qml",
			                               static_cast<AbstractListModel*>(&listViewModel2_), id.c_str());
			if (view.valid())
			{
				views_.push_back(std::move(view));
			}

			id = "plg_async_loader_test/list_dock_panel_2.0.qml" + std::to_string(3);
			view = viewCreator->createView("plg_async_loader_test/list_dock_panel_2.0.qml",
			                               static_cast<AbstractListModel*>(&listViewModel3_), id.c_str());
			if (view.valid())
			{
				views_.push_back(std::move(view));
			}
		},
		[this](const IAction*) { return views_.empty(); });
		closeViews_ =
		uiFramework->createAction("CloseViews", [this, uiApplication](IAction*) { closeViews(uiApplication); },
		                          [this](const IAction*) { return !views_.empty(); });

		uiApplication->addAction(*openViews_);
		uiApplication->addAction(*closeViews_);
	}

	//==========================================================================
	virtual bool Finalise(IComponentContext& contextManager) override
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);
		uiApplication->removeAction(*openViews_);
		uiApplication->removeAction(*closeViews_);
		closeViews(uiApplication);
		openViews_ = nullptr;
		closeViews_ = nullptr;
		return true;
	}
	//==========================================================================
	virtual void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}

	void closeViews(IUIApplication* uiApplication)
	{
		for (auto& it : views_)
		{
			if (it.valid())
			{
				auto view = it.get();
				uiApplication->removeView(*view);
				view = nullptr;
			}
		}
		views_.clear();
	}
};

PLG_CALLBACK_FUNC(AsyncLoaderTestPlugin)
} // end namespace wgt
