#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "project/metadata/project.mpp"

#include "core_dependency_system/depends.hpp"
#include <vector>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates a project option in the menu bar that lets you create, open and save project environment
* settings
*
* @ingroup plugins
* @image html plg_environment_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class EnvrionmentTestPlugin : public PluginMain, public Depends<IViewCreator>
{
private:
	InterfacePtrs types_;
	std::unique_ptr<IAction> newProject_;
	std::unique_ptr<IAction> openProject_;
	std::unique_ptr<IAction> saveProject_;
	std::unique_ptr<IAction> closeProject_;
	std::unique_ptr<IWindow> newProjectDialog_;
	std::unique_ptr<IWindow> openProjectDialog_;
	IComponentContext* contextManager_;
	std::unique_ptr<ManagedObject<ProjectManager>> projectManager_;
	ConnectionHolder connections_;

public:
	//==========================================================================
	EnvrionmentTestPlugin(IComponentContext& contextManager) : contextManager_(&contextManager)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager) override
	{
		projectManager_ = std::make_unique<ManagedObject<ProjectManager>>();
		(*projectManager_)->init();

		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiFramework != nullptr && uiApplication != nullptr);
		uiFramework->loadActionData(":/TestingProjectControl/actions.xml", IUIFramework::ResourceType::File);

		newProject_ = uiFramework->createAction("NewProject", std::bind(&EnvrionmentTestPlugin::newProject, this));

		openProject_ = uiFramework->createAction("OpenProject", std::bind(&EnvrionmentTestPlugin::openProject, this),
		                                         std::bind(&EnvrionmentTestPlugin::canOpen, this));

		saveProject_ = uiFramework->createAction("SaveProject", std::bind(&EnvrionmentTestPlugin::saveProject, this),
		                                         std::bind(&EnvrionmentTestPlugin::canSave, this));

		closeProject_ = uiFramework->createAction("CloseProject", std::bind(&EnvrionmentTestPlugin::closeProject, this),
		                                          std::bind(&EnvrionmentTestPlugin::canClose, this));

		uiApplication->addAction(*newProject_);
		uiApplication->addAction(*openProject_);
		uiApplication->addAction(*saveProject_);
		uiApplication->addAction(*closeProject_);

		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			viewCreator->createWindow(
			"TestingProjectControl/NewProjectDialog.qml", projectManager_->getHandle(), [this](std::unique_ptr<IWindow>& window) {
				newProjectDialog_ = std::move(window);
				if (newProjectDialog_ != nullptr)
				{
					connections_ +=
					newProjectDialog_->signalClose.connect(std::bind(&EnvrionmentTestPlugin::onNewDlgClose, this));
				}
			});
		}
	}
	//==========================================================================
	bool Finalise(IComponentContext& contextManager) override
	{
		(*projectManager_)->fini();
		projectManager_.reset();
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);
		uiApplication->removeAction(*newProject_);
		uiApplication->removeAction(*openProject_);
		uiApplication->removeAction(*saveProject_);
		uiApplication->removeAction(*closeProject_);
		if (newProjectDialog_ != nullptr)
		{
			uiApplication->removeWindow(*newProjectDialog_);
			newProjectDialog_ = nullptr;
		}
		if (openProjectDialog_ != nullptr)
		{
			uiApplication->removeWindow(*openProjectDialog_);
			openProjectDialog_ = nullptr;
		}
		connections_.clear();
		newProject_ = nullptr;
		openProject_ = nullptr;
		saveProject_ = nullptr;
		closeProject_ = nullptr;
		return true;
	}
	//==========================================================================
	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}

	void newProject()
	{
		connections_.clear();
		if (newProjectDialog_ != nullptr)
		{
			connections_ +=
			newProjectDialog_->signalClose.connect(std::bind(&EnvrionmentTestPlugin::onNewDlgClose, this));
			newProjectDialog_->showModal();
		}
	}

	void onNewDlgClose()
	{
		(*projectManager_)->createProject();
	}

	void onOpenDlgClose()
	{
		(*projectManager_)->openProject();
	}
	void openProject()
	{
		connections_.clear();
		auto uiFramework = contextManager_->queryInterface<IUIFramework>();
		auto uiApplication = contextManager_->queryInterface<IUIApplication>();
		if (openProjectDialog_ != nullptr)
		{
			uiApplication->removeWindow(*openProjectDialog_);
			openProjectDialog_ = nullptr;
		}
		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			viewCreator->createWindow(
			"TestingProjectControl/OpenProjectDialog.qml", projectManager_->getHandle(), [this](std::unique_ptr<IWindow>& window) {
				openProjectDialog_ = std::move(window);
				if (openProjectDialog_ != nullptr)
				{
					connections_ +=
					openProjectDialog_->signalClose.connect(std::bind(&EnvrionmentTestPlugin::onOpenDlgClose, this));
				}
			});
		}
	}
	void saveProject()
	{
		(*projectManager_)->saveProject();
	}
	void closeProject()
	{
		(*projectManager_)->closeProject();
	}
	bool canOpen()
	{
		return (*projectManager_)->canOpen();
	}

	bool canSave()
	{
		return (*projectManager_)->canSave();
	}

	bool canClose()
	{
		return (*projectManager_)->canClose();
	}
};

PLG_CALLBACK_FUNC(EnvrionmentTestPlugin)
} // end namespace wgt
