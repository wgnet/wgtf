#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "test_list_model.hpp"
#include <vector>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates various styles of lists with sample data
*
* @ingroup plugins
* @image html plg_list_model_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ListModelTestPlugin : public PluginMain, public Depends<IViewCreator>
{
private:
	std::vector<IInterface*> types_;
	wg_future<std::unique_ptr<IView>> listView_;
	wg_future<std::unique_ptr<IView>> shortListView_;
	wg_future<std::unique_ptr<IView>> multiColumnListView_;
	std::shared_ptr<AbstractListModel> listModel_;

public:
	//==========================================================================
	ListModelTestPlugin(IComponentContext& contextManager) : Depends(contextManager)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		assert((uiFramework != nullptr) && (uiApplication != nullptr));

		listModel_ = std::make_shared<TestListModel>();

		if (auto viewCreator = get<IViewCreator>())
		{
			listView_ = viewCreator->createView("plg_list_model_test/test_list_panel.qml", listModel_);
		}
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);

		if (listView_.valid())
		{
			auto view = listView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}

		listModel_ = nullptr;
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager)
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type);
		}
	}
};

PLG_CALLBACK_FUNC(ListModelTestPlugin)
} // end namespace wgt
