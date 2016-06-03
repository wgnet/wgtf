#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_data_model/i_list_model.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_data_model/generic_list.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "tinyxml2.hpp"

#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"

#include "metadata/demo_objects.mpp"
#include "metadata/demo_objects_fix_mixin.mpp"

#include <stdio.h>
#include "core_command_system/i_env_system.hpp"
#include "core_serialization/i_file_system.hpp"

namespace wgt
{
namespace
{
	enum class ModelPropertyValueType : uint8_t
	{
		String = 0,
		Boolean,
		Integer,
		Double,
		EnumList,
		Vector2,
		Vector3,
		Vector4,
	};
}

class DemoDoc: public IViewEventListener
	, public Depends< IViewCreator >
{
public:
	DemoDoc( IComponentContext & context, const char* name, IEnvManager* envManager, IUIFramework* uiFramework,
		IUIApplication* uiApplication, ObjectHandle demo );
	~DemoDoc();

	// IViewEventListener
	virtual void onFocusIn( IView* view ) override;
	virtual void onFocusOut( IView* view ) override;
	void onLoaded(IView* view) override {}

private:
	IEnvManager* envManager_;
	IUIApplication* uiApplication_;
	std::unique_ptr< IView > centralView_;
	int envId_;
};

DemoDoc::DemoDoc(
	IComponentContext & context,
	const char* name, IEnvManager* envManager, IUIFramework* uiFramework,
								IUIApplication* uiApplication, ObjectHandle demo)
	: Depends( context )
	, envManager_( envManager )
	, uiApplication_(uiApplication)
{
	envId_ = envManager_->addEnv( name );
	envManager_->selectEnv( envId_ );

	auto viewCreator = get< IViewCreator >();
	if (viewCreator)
	{
		viewCreator->createView(
			"DemoTest/Demo.qml", demo,
			[ this ]( std::unique_ptr< IView > & view )
		{
			centralView_ = std::move( view );
			centralView_->registerListener( this );
		});
	}
}

DemoDoc::~DemoDoc()
{
	if (centralView_ != nullptr)
	{
		uiApplication_->removeView( *centralView_ );
		centralView_->deregisterListener( this );
	}

	envManager_->removeEnv( envId_ );
}

void DemoDoc::onFocusIn(IView* view)
{
	envManager_->selectEnv( envId_ );
}

void DemoDoc::onFocusOut(IView* view)
{
}

//==============================================================================
class DemoTestPlugin
	: public PluginMain
	, public Depends< IViewCreator >
{
private:
	
	std::unique_ptr< DemoDoc > demoDoc_;
	std::unique_ptr< DemoDoc > demoDoc2_;
	std::unique_ptr< IView > propertyView_;
	std::unique_ptr< IView > sceneBrowser_;
	std::unique_ptr< IView > viewport_;
	ObjectHandle demoModel_;

	IReflectionController* controller_;
	IDefinitionManager* defManager_;
	std::unique_ptr< IAction > createAction_;

public:
	//==========================================================================
	DemoTestPlugin(IComponentContext & contextManager )
		: Depends( contextManager )
	{
	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		Variant::setMetaTypeManager( 
			contextManager.queryInterface< IMetaTypeManager >() );

		defManager_ =
			contextManager.queryInterface< IDefinitionManager >();
		assert(defManager_ != nullptr);
		this->initReflectedTypes( *defManager_ );

		controller_ = contextManager.queryInterface< IReflectionController >();

		demoModel_ = defManager_->create<DemoObjectsFixMixIn>();
		demoModel_.getBase< DemoObjectsFixMixIn >()->init( contextManager );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		auto envManager = contextManager.queryInterface< IEnvManager >();
		if ( uiApplication == nullptr || uiFramework == nullptr || envManager == nullptr )
		{
			return;
		}

		demoDoc_.reset( new DemoDoc( contextManager, "sceneModel0", envManager, uiFramework, uiApplication, demoModel_) );
		demoDoc2_.reset( new DemoDoc( contextManager, "sceneModel1", envManager, uiFramework, uiApplication, demoModel_) );

		auto viewCreator = get< IViewCreator >();
		if (viewCreator)
		{
			viewCreator->createView(
				"DemoTest/DemoPropertyPanel.qml",
				demoModel_, propertyView_ );

			viewCreator->createView(
				"DemoTest/DemoListPanel.qml",
				demoModel_, sceneBrowser_);

			viewCreator->createView(
				"DemoTest/Framebuffer.qml",
				demoModel_, viewport_);
		}
		createAction_ = uiFramework->createAction(
			"New Object", 
			[&] (const IAction * action) { createObject(); },
			[&] (const IAction * action) { return canCreate(); } );

		uiApplication->addAction( *createAction_ );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return false;
		}
		if (propertyView_ != nullptr)
		{
			uiApplication->removeView( *propertyView_ );
		}
		if (sceneBrowser_ != nullptr)
		{
			uiApplication->removeView( *sceneBrowser_ );
		}
		if (viewport_ != nullptr)
		{
			uiApplication->removeView( *viewport_ );
		}

		propertyView_ = nullptr;
		sceneBrowser_ = nullptr;
		viewport_ = nullptr;
		demoDoc_ = nullptr;
		demoDoc2_ = nullptr;
		demoModel_.getBase< DemoObjectsFixMixIn >()->fini();
		demoModel_ = nullptr;
		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}

	void initReflectedTypes( IDefinitionManager & definitionManager )
	{
		REGISTER_DEFINITION( DemoObjects )
		REGISTER_DEFINITION( DemoObjectsFixMixIn )
	}

private:
	void createObject()
	{
		IClassDefinition* def = defManager_->getDefinition<DemoObjectsFixMixIn>();
		PropertyAccessor pa = def->bindProperty( "New Object", demoModel_ );
		assert( pa.isValid() );
		ReflectedMethodParameters parameters;
		parameters.push_back( Vector3( 0.f, 0.f, -10.f) );
		Variant returnValue = controller_->invoke( pa, parameters );

		/*std::unique_ptr<ReflectedMethodCommandParameters> commandParameters( new ReflectedMethodCommandParameters() );
		commandParameters->setId( key.first );
		commandParameters->setPath( key.second.c_str() );
		commandParameters->setParameters( parameters );

		commandManager_->queueCommand(
			getClassIdentifier<InvokeReflectedMethodCommand>(), ObjectHandle( std::move( commandParameters ),
			pa.getDefinitionManager()->getDefinition<ReflectedMethodCommandParameters>() ) )

		demoModel_.getBase< DemoObjects >()->createObject();*/
	}

	bool canCreate() { return true; }

};


PLG_CALLBACK_FUNC(DemoTestPlugin)
} // end namespace wgt
