//
// Copyright (C) Wargaming 
//
#pragma warning( push )
#pragma warning( disable: 4244 4100 4238 4239 4263 4245 4201 )
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
#if _WIN32_WINNT < 0x0502
#undef _WIN32_WINNT
#undef NTDDI_VERSION
#define _WIN32_WINNT _WIN32_WINNT_WS03 //required for SetDllDirectory
#define NTDDI_VERSION NTDDI_WS03
#include <windows.h>
#endif // _WIN32_WINNT
#endif // _WIN32

#include "core_ui_framework/i_ui_application.hpp"
#include "../../generic_app/app/memory_plugin_context_creator.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin_manager/config_plugin_loader.hpp"
#include "core_common/platform_env.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "ngt_application_proxy.hpp"
#include <QtCore/QCoreApplication>
#include <QtGui/QDockWidget>
#include <QtGui/QLayout>
#include <QtGui/QMainWindow>
#include "qwinhost.h"

#include <shlwapi.h>

#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MSyntax.h>
#include <maya/MGlobal.h>
#include <maya/MQtUtil.h>
#include <maya/MTemplateCommand.h>
#include <assert.h>

namespace wgt
{
char NGT_MAYA_COMMAND_SHOW[] = "NGTShow";
char NGT_MAYA_COMMAND_HIDE[] = "NGTHide";
char NGT_MAYA_COMMAND_START[] = "NGTStart";
char NGT_MAYA_COMMAND_STOP[] = "NGTStop";
const char * NGT_MAYA_PLUGIN_LIST_FILE = "\\plugins\\plugins_maya.txt";

#ifdef _DEBUG
const char * NGT_MAYA_PLUGIN_NAME = "maya_plugin_d.mll";
#else
	const char * NGT_MAYA_PLUGIN_NAME = "maya_plugin.mll";
#endif

static HMODULE hApp = ::GetModuleHandleA( NGT_MAYA_PLUGIN_NAME );

static char ngtHome[MAX_PATH];
static wchar_t exePath[MAX_PATH];

const char NGT_HOME[] = "NGT_HOME";
const char NGT_PATH[] = "PATH";

static NGTApplicationProxy * ngtApp = nullptr;
static GenericPluginManager * pluginManager = nullptr;

bool getNGTPlugins(std::vector< std::wstring >& plugins, const wchar_t* filepath)
{	
	return ConfigPluginLoader::getPlugins(plugins, std::wstring( filepath ));
}

bool loadNGT()
{
	MString filepath = ngtHome;
	filepath += NGT_MAYA_PLUGIN_LIST_FILE;

	std::vector< std::wstring > plugins;
	if (!getNGTPlugins(plugins, filepath.asWChar()) || plugins.empty())
	{
		return MStatus::kFailure; // failed to find any plugins!
	}

	auto& contextManager = pluginManager->getContextManager();
	contextManager.setExecutablePath( ngtHome );

	auto globalContext = contextManager.getGlobalContext();
	globalContext->registerInterface(new MemoryPluginContextCreator);

	pluginManager->loadPlugins(plugins);

	auto uiApp = globalContext->queryInterface< IUIApplication >();
	if (!uiApp)
	{
		return false;
	}

	ngtApp = new NGTApplicationProxy( uiApp );
	ngtApp->start();

	return true;
}

struct NGTShowCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_SHOW, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTShowCommand::doIt(const MArgList& args)
{
	if (!ngtApp || !ngtApp->started())
	{
		return MStatus::kFailure;
	}

	if (!ngtApp->visible())
	{
		ngtApp->show();
	}

	return MStatus::kSuccess;
}

struct NGTHideCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_HIDE, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTHideCommand::doIt(const MArgList& args)
{
	if (!ngtApp || !ngtApp->started())
	{
		return MStatus::kFailure;
	}

	ngtApp->hide();
	return MStatus::kSuccess;
}

struct NGTStartCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_HIDE, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTStartCommand::doIt(const MArgList& args)
{
	if (!ngtApp)
	{
		return MStatus::kFailure;
	}

	if (!ngtApp->started())
	{
		ngtApp->start();
	}
	return MStatus::kSuccess;
}

struct NGTStopCommand : public MTemplateAction< NGTShowCommand, NGT_MAYA_COMMAND_STOP, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus NGTStopCommand::doIt(const MArgList& args)
{
	if (!ngtApp || !ngtApp->started())
	{
		return MStatus::kFailure;
	}

	ngtApp->stop();
	return MStatus::kSuccess;
}

MStatus initializeMayaPlugin(MObject obj)
{
	MStatus status;


    if (!Environment::getValue<MAX_PATH>( NGT_HOME, ngtHome ))
    {
#ifdef _WIN32
        GetModuleFileNameA( hApp, ngtHome, MAX_PATH );
        PathRemoveFileSpecA( ngtHome );
        Environment::setValue( NGT_HOME, ngtHome );
#endif // _WIN32

#ifdef __APPLE__
        Dl_info info;
        if (!dladdr( reinterpret_cast<void*>(setContext), &info ))
        {
            NGT_ERROR_MSG( "Generic plugin manager: failed to get current module file name%s", "\n" );
        }
        strcpy(ngtHome, info.dli_fname);
        const char* dir = dirname(ngtHome);
        Environment::setValue( NGT_HOME, dir);

        std::string dlybs = dir;
        dlybs += "/../PlugIns";
        Environment::setValue( "LD_LIBRARY_PATH", dlybs.c_str() );
#endif // __APPLE__
    }

#ifdef __APPLE__
    Environment::setValue( "QT_QPA_PLATFORM_PLUGIN_PATH", (std::string( ngtHome ) + "/../PlugIns/platforms").c_str() );
#else
    Environment::setValue( "QT_QPA_PLATFORM_PLUGIN_PATH", (std::string( ngtHome ) + "/platforms").c_str() );
#endif

#ifdef _WIN32
    size_t convertedChars = 0;
    mbstowcs_s( &convertedChars, exePath, MAX_PATH, ngtHome, _TRUNCATE );
    assert( convertedChars );
#endif // _WIN32

#ifdef __APPLE__
    std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
    wcscpy(exePath, conv.from_bytes( ngtHome ).c_str());
#endif // __APPLE__

    char path[2048];
    if(Environment::getValue<2048>( NGT_PATH, path ))
    {
        std::string newPath( "\"" );
        newPath += ngtHome;
        newPath += "\";";
        newPath += path;
        Environment::setValue( NGT_PATH, newPath.c_str() );
    }

#ifdef _WIN32
    SetDllDirectoryA( ngtHome );
#endif // _WIN32



	pluginManager = new GenericPluginManager();

	NGTShowCommand::registerCommand( obj );
	NGTHideCommand::registerCommand( obj );
	NGTStartCommand::registerCommand( obj );
	NGTStopCommand::registerCommand( obj );

	return loadNGT() ? MStatus::kSuccess : MStatus::kFailure;
}

MStatus uninitializeMayaPlugin(MObject obj)
{
	delete ngtApp;
	ngtApp = nullptr;
	delete pluginManager;
	pluginManager = nullptr;
	// TODO: Maya crashes if return MStatus::kSuccess here
	return MStatus::kFailure;
}

}

PLUGIN_EXPORT MStatus initializePlugin(MObject obj)
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	return wgt::initializeMayaPlugin(obj);
}

PLUGIN_EXPORT MStatus uninitializePlugin(MObject obj)
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	return wgt::uninitializeMayaPlugin(obj);
} // end namespace wgt

#pragma warning( pop )
