#include "core_generic_plugin/generic_plugin.hpp"

#include "history_object.hpp"
#include "metadata/history_object.mpp"
#include "metadata/display_object.mpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/i_history_panel.h"

#include "core_logging/logging.hpp"

#include "core_qt_common/i_qt_framework.hpp"

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/type_class_definition.hpp"

#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_action.hpp"

#include "core_dependency_system/depends.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
class HistoryPanel : public IHistoryPanel
{
public:
	void init(ObjectHandle object, IDefinitionManager& definitionManager)
	{
		const IClassDefinition* classDefinition = object.getDefinition(definitionManager);
		clearButtonVisibility = classDefinition->bindProperty("IsClearButtonVisible", object);
		makeMacroButtonVisibility = classDefinition->bindProperty("IsMakeMacroButtonVisible", object);
		assert(clearButtonVisibility.isValid());
		assert(makeMacroButtonVisibility.isValid());
	}

	void setClearButtonVisible(bool show) override
	{
		if (clearButtonVisibility.isValid())
		{
			clearButtonVisibility.setValue(show);
		}
	}

	void setMakeMacroButtonVisible(bool show) override
	{
		if (makeMacroButtonVisibility.isValid())
		{
			makeMacroButtonVisibility.setValue(show);
		}
	}

private:
	PropertyAccessor clearButtonVisibility;
	PropertyAccessor makeMacroButtonVisibility;
};

/**
* A plugin which creates a panel that shows command history that can be undone/redone.
* Only values set through reflection show history.
*
* @ingroup plugins
* @image html plg_history_ui.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - MacrosUIPlugin (If wanting to use the Make Macro button)
*/
class HistoryUIPlugin : public PluginMain, public Depends<IViewCreator>
{
public:
	HistoryUIPlugin(IComponentContext& contextManager) : Depends(contextManager)
	{
	}

	void createActions(IUIFramework& uiFramework, IUIApplication& uiApplication)
	{
		// hook undo/redo
		using namespace std::placeholders;

		undo_ = uiFramework.createAction("Undo", std::bind(&HistoryUIPlugin::undo, this, _1),
		                                 std::bind(&HistoryUIPlugin::canUndo, this, _1));

		redo_ = uiFramework.createAction("Redo", std::bind(&HistoryUIPlugin::redo, this, _1),
		                                 std::bind(&HistoryUIPlugin::canRedo, this, _1));

		uiApplication.addAction(*undo_);
		uiApplication.addAction(*redo_);
	}

	void destroyActions(IUIApplication& uiApplication)
	{
		uiApplication.removeAction(*redo_);
		uiApplication.removeAction(*undo_);
		redo_.reset();
		undo_.reset();
	}

	void undo(IAction* action)
	{
		assert(commandSystemProvider_);
		if (commandSystemProvider_ == nullptr)
		{
			return;
		}
		commandSystemProvider_->undo();
	}

	void redo(IAction* action)
	{
		assert(commandSystemProvider_);
		if (commandSystemProvider_ == nullptr)
		{
			return;
		}
		commandSystemProvider_->redo();
	}

	bool canUndo(const IAction* action) const
	{
		if (commandSystemProvider_ == nullptr)
		{
			return false;
		}
		return commandSystemProvider_->canUndo();
	}

	bool canRedo(const IAction* action) const
	{
		if (commandSystemProvider_ == nullptr)
		{
			return false;
		}
		return commandSystemProvider_->canRedo();
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		auto pDefinitionManager = contextManager.queryInterface<IDefinitionManager>();
		if (pDefinitionManager == nullptr)
		{
			return;
		}
		auto& definitionManager = *pDefinitionManager;
		REGISTER_DEFINITION(HistoryObject);
		REGISTER_DEFINITION(DisplayObject);

		commandSystemProvider_ = contextManager.queryInterface<ICommandManager>();
		if (commandSystemProvider_ == nullptr)
		{
			return;
		}

		auto pHistoryDefinition = pDefinitionManager->getDefinition(getClassIdentifier<HistoryObject>());
		assert(pHistoryDefinition != nullptr);
		history_ = pHistoryDefinition->create();
		history_.getBase<HistoryObject>()->init(*commandSystemProvider_, definitionManager);

		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			panel_ = viewCreator->createView("WGHistory/WGHistoryView.qml", history_);
		}

		auto pQtFramework = contextManager.queryInterface<IQtFramework>();
		if (pQtFramework == nullptr)
		{
			return;
		}
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		createActions(*pQtFramework, *uiApplication);
		HistoryPanel* historyPanelinterface = new HistoryPanel();
		historyPanelinterface->init(history_, *pDefinitionManager);
		historyPanelInterface_.reset(historyPanelinterface);
		historyPanelInterfaceID = contextManager.registerInterface(historyPanelInterface_.get(), false);
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		contextManager.deregisterInterface(historyPanelInterfaceID);
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		if (uiApplication == nullptr)
		{
			return true;
		}
		if (panel_.valid())
		{
			auto view = panel_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}
		destroyActions(*uiApplication);
		auto historyObject = history_.getBase<HistoryObject>();
		assert(historyObject != nullptr);
		historyObject->fini();
		return true;
	}

private:
	wg_future<std::unique_ptr<IView>> panel_;
	ObjectHandle history_;
	std::unique_ptr<IHistoryPanel> historyPanelInterface_;
	IInterface* historyPanelInterfaceID;

	ICommandManager* commandSystemProvider_;
	std::unique_ptr<IAction> undo_;
	std::unique_ptr<IAction> redo_;
};

PLG_CALLBACK_FUNC(HistoryUIPlugin)
} // end namespace wgt
