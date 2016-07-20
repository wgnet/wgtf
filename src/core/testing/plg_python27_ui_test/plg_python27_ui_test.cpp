#include "core_generic_plugin/generic_plugin.hpp"

#include "ui_test_panel_context.hpp"
#include "python_panel.hpp"

#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/reflection_macros.hpp"

#include <memory>


namespace wgt
{
ObjectHandle createContextObject( IComponentContext& componentContext,
	const char * panelName,
	ObjectHandle & pythonObject )
{
	auto pDefinitionManager = componentContext.queryInterface< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
		return false;
	}
	auto & definitionManager = (*pDefinitionManager);

	auto controller = componentContext.queryInterface< IReflectionController >();
	if (controller == nullptr)
	{
		NGT_ERROR_MSG( "Failed to find IReflectionController\n" );
		return false;
	}

	const bool managed = true;
	auto contextObject = pDefinitionManager->create< PanelContext >( managed );
	contextObject->pContext_ = &componentContext;
	contextObject->panelName_ = panelName;
	contextObject->pythonObject_ = pythonObject;
	contextObject->treeModel_.reset(
		new ReflectedTreeModel( pythonObject, definitionManager, controller ) );

	return contextObject;
}

/**
* A plugin which queries the IPythonScriptingEngine to test adding components through python scripts
*
* @ingroup plugins
* @image html plg_python27_ui_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - Python27Plugin
*/
struct Python27TestUIPlugin
	: public PluginMain
{
	Python27TestUIPlugin( IComponentContext& componentContext )
	{
	}


	bool PostLoad( IComponentContext& componentContext ) override
	{
		return true;
	}


	void Initialise( IComponentContext& componentContext ) override
	{
		auto pDefinitionManager = componentContext.queryInterface< IDefinitionManager >();
		if (pDefinitionManager == nullptr)
		{
			NGT_ERROR_MSG( "Failed to find IDefinitionManager\n" );
			return;
		}
		auto & definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION( PanelContext );

		auto pScriptingEngine = componentContext.queryInterface< IPythonScriptingEngine >();
		if (pScriptingEngine == nullptr)
		{
			NGT_ERROR_MSG( "Failed to find IPythonScriptingEngine\n" );
			return;
		}
		auto & scriptingEngine = (*pScriptingEngine);

		const wchar_t * sourcePath = L"../../../src/core/testing/plg_python27_ui_test/scripts";
		const wchar_t * deployPath = L"./scripts/plg_python27_ui_test";
		const char * moduleName = "test_objects";
		const bool sourcePathSet = scriptingEngine.appendSourcePath( sourcePath );
		assert( sourcePathSet );
		const bool deployPathSet =  scriptingEngine.appendBinPath( deployPath );
		assert( deployPathSet );
		auto module = scriptingEngine.import( moduleName );
		if (!module.isValid())
		{
			NGT_ERROR_MSG( "Could not load from scripts\n" );
			return;
		}


		const char * panelName1 = "Python Test 1";
		auto contextObject1 = createContextObject( componentContext, panelName1, module );
		pythonPanel1_.reset( new PythonPanel( componentContext,
			contextObject1 ) );
		const char * panelName2 = "Python Test 2";
		auto contextObject2 = createContextObject( componentContext, panelName2, module );
		pythonPanel2_.reset( new PythonPanel( componentContext,
			contextObject2 ) );
	}


	bool Finalise( IComponentContext& componentContext ) override
	{
		pythonPanel2_.reset();
		pythonPanel1_.reset();
		return true;
	}


	void Unload( IComponentContext& componentContext ) override
	{
	}


	std::unique_ptr< PythonPanel > pythonPanel1_;
	std::unique_ptr< PythonPanel > pythonPanel2_;
};


PLG_CALLBACK_FUNC( Python27TestUIPlugin )
} // end namespace wgt
