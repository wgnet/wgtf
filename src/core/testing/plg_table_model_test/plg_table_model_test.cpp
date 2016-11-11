#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "test_table_model.hpp"
#include <vector>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates various styles of tables with sample data
*
* @ingroup plugins
* @image html plg_table_model_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class TableModelTestPlugin : public PluginMain, public Depends<IViewCreator>
{
private:
	std::vector<IInterface*> types_;
	wg_future<std::unique_ptr<IView>> tableView_;
	std::shared_ptr<AbstractTableModel> tableModel_;

public:
	//==========================================================================
	TableModelTestPlugin(IComponentContext& contextManager) : Depends(contextManager)
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

		tableModel_ = std::make_shared<TestTableModel>();

		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			tableView_ = viewCreator->createView("plg_table_model_test/test_table_panel.qml", tableModel_);
		}
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);
		if (tableView_.valid())
		{
			auto view = tableView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}
		tableModel_ = nullptr;
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

PLG_CALLBACK_FUNC(TableModelTestPlugin)
} // end namespace wgt
