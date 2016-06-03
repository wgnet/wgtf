#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_variant/variant.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_window.hpp"
#include "project/metadata/project.mpp"
#include <vector>


namespace wgt
{
//==============================================================================
class EnvrionmentTestPlugin
	: public PluginMain
{
private:
	std::vector<IInterface*> types_;
    std::unique_ptr< IAction > newProject_;
    std::unique_ptr< IAction > openProject_;
    std::unique_ptr< IAction > saveProject_;
    std::unique_ptr< IAction > closeProject_;
    std::unique_ptr< IWindow > newProjectDialog_;
    std::unique_ptr< IWindow > openProjectDialog_;
    IComponentContext* contextManager_;
    ObjectHandle projectManager_;
    ConnectionHolder connections_;
public:
	//==========================================================================
	EnvrionmentTestPlugin(IComponentContext & contextManager )
        : contextManager_( & contextManager )
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager ) override
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );
		// register reflected type definition
		IDefinitionManager* defManager =
			contextManager.queryInterface< IDefinitionManager >();
		assert(defManager != nullptr);

		this->initReflectedTypes( *defManager );
        auto pDefinition = defManager->getDefinition(
            getClassIdentifier< ProjectManager >() );
        assert( pDefinition != nullptr );
        projectManager_ = pDefinition->create();
        projectManager_.getBase< ProjectManager >()->init( contextManager );

        auto uiFramework = contextManager.queryInterface<IUIFramework>();
        auto uiApplication = contextManager.queryInterface<IUIApplication>();
        assert( uiFramework != nullptr && uiApplication != nullptr );
        uiFramework->loadActionData( 
            ":/TestingProjectControl/actions.xml", IUIFramework::ResourceType::File );

        newProject_ = uiFramework->createAction(
            "NewProject", 
            std::bind( &EnvrionmentTestPlugin::newProject, this ) );

        openProject_ = uiFramework->createAction(
            "OpenProject", 
            std::bind( &EnvrionmentTestPlugin::openProject, this ),
            std::bind( &EnvrionmentTestPlugin::canOpen, this ) );

        saveProject_ = uiFramework->createAction(
            "SaveProject", 
            std::bind( &EnvrionmentTestPlugin::saveProject, this ),
            std::bind( &EnvrionmentTestPlugin::canSave, this ) );

        closeProject_ = uiFramework->createAction(
            "CloseProject", 
            std::bind( &EnvrionmentTestPlugin::closeProject, this ),
            std::bind( &EnvrionmentTestPlugin::canClose, this ) );

        uiApplication->addAction( *newProject_ );
        uiApplication->addAction( *openProject_ );
        uiApplication->addAction( *saveProject_ );
        uiApplication->addAction( *closeProject_ );

        newProjectDialog_ = uiFramework->createWindow( 
            "TestingProjectControl/NewProjectDialog.qml", 
            IUIFramework::ResourceType::Url, projectManager_ );
        if(newProjectDialog_ != nullptr)
        {
            connections_ += newProjectDialog_->signalClose.connect( std::bind( &EnvrionmentTestPlugin::onNewDlgClose, this ) );
            uiApplication->addWindow( *newProjectDialog_ );
        }
	}
	//==========================================================================
	bool Finalise( IComponentContext & contextManager ) override
	{
        projectManager_.getBase< ProjectManager >()->fini();
        projectManager_ = nullptr;
        auto uiApplication = contextManager.queryInterface<IUIApplication>();
        assert( uiApplication != nullptr );
        uiApplication->removeAction( *newProject_ );
        uiApplication->removeAction( *openProject_ );
        uiApplication->removeAction( *saveProject_ );
        uiApplication->removeAction( *closeProject_ );
        if(newProjectDialog_ != nullptr)
        {
            uiApplication->removeWindow( *newProjectDialog_ );
            newProjectDialog_ = nullptr;
        }
        if(openProjectDialog_ != nullptr)
        {
            uiApplication->removeWindow( *openProjectDialog_ );
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
	void Unload( IComponentContext & contextManager ) override
	{
		for (auto type: types_)
		{
			contextManager.deregisterInterface( type );
		}
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
        REGISTER_DEFINITION( ProjectManager )
        REGISTER_DEFINITION( ProjectData )
	}

    void newProject()
    {
        connections_.clear();
        if (newProjectDialog_ != nullptr)
        {
            connections_ += newProjectDialog_->signalClose.connect( std::bind( &EnvrionmentTestPlugin::onNewDlgClose, this ) );
            newProjectDialog_->showModal();
        }
    }

    void onNewDlgClose()
    {
        projectManager_.getBase< ProjectManager >()->createProject();
    }

    void onOpenDlgClose()
    {
        projectManager_.getBase< ProjectManager >()->openProject();
    }
    void openProject()
    {
        connections_.clear();
        auto uiFramework = contextManager_->queryInterface<IUIFramework>();
        auto uiApplication = contextManager_->queryInterface<IUIApplication>();
        if(openProjectDialog_ != nullptr)
        {
            uiApplication->removeWindow( *openProjectDialog_ );
            openProjectDialog_ = nullptr;
        }
        openProjectDialog_ = uiFramework->createWindow( 
            "TestingProjectControl/OpenProjectDialog.qml", 
            IUIFramework::ResourceType::Url, projectManager_ );
        if(openProjectDialog_ != nullptr)
        {
            connections_ += openProjectDialog_->signalClose.connect( std::bind( &EnvrionmentTestPlugin::onOpenDlgClose, this ) );
            uiApplication->addWindow( *openProjectDialog_ );
        }
    }
    void saveProject()
    {
        projectManager_.getBase< ProjectManager >()->saveProject();
    }
    void closeProject()
    {
        projectManager_.getBase< ProjectManager >()->closeProject();
    }
    bool canOpen()
    {
        return projectManager_.getBase< ProjectManager >()->canOpen();
    }

    bool canSave()
    {
        return projectManager_.getBase< ProjectManager >()->canSave();
    }

    bool canClose()
    {
        return projectManager_.getBase< ProjectManager >()->canClose();
    }

};


PLG_CALLBACK_FUNC( EnvrionmentTestPlugin )
} // end namespace wgt
