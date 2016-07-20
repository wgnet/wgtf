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

#include "application_proxy.hpp"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
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
char WGT_MAYA_COMMAND_SHOW[] = "WGTShow";
char WGT_MAYA_COMMAND_HIDE[] = "WGTHide";
char WGT_MAYA_COMMAND_START[] = "WGTStart";
char WGT_MAYA_COMMAND_STOP[] = "WGTStop";
const char * WGT_MAYA_PLUGIN_LIST_FILE = "\\plugins\\plugins_maya.txt";

#ifdef _DEBUG
const char * WGT_MAYA_PLUGIN_NAME = "maya_plugin_d.mll";
#else
	const char * WGT_MAYA_PLUGIN_NAME = "maya_plugin.mll";
#endif

static HMODULE hApp = ::GetModuleHandleA( WGT_MAYA_PLUGIN_NAME );

static char wgtHome[MAX_PATH];
static wchar_t exePath[MAX_PATH];

const char WGT_HOME[] = "WGT_HOME";
const char WGT_PATH[] = "PATH";

static ApplicationProxy * wgtApp = nullptr;
static GenericPluginManager * pluginManager = nullptr;

bool getWGTPlugins(std::vector< std::wstring >& plugins, const wchar_t* filepath)
{	
	return ConfigPluginLoader::getPlugins(plugins, std::wstring( filepath ));
}

bool loadWGT()
{
	MString filepath = wgtHome;
	filepath += WGT_MAYA_PLUGIN_LIST_FILE;

	std::vector< std::wstring > plugins;
	if (!getWGTPlugins(plugins, filepath.asWChar()) || plugins.empty())
	{
		return MStatus::kFailure; // failed to find any plugins!
	}

	auto& contextManager = pluginManager->getContextManager();
	contextManager.setExecutablePath( wgtHome );

	auto globalContext = contextManager.getGlobalContext();
	globalContext->registerInterface(new MemoryPluginContextCreator);

	pluginManager->loadPlugins(plugins);

	auto uiApp = globalContext->queryInterface< IUIApplication >();
	if (!uiApp)
	{
		return false;
	}

	wgtApp = new ApplicationProxy( uiApp );
	wgtApp->start();

	return true;
}

struct WGTShowCommand : public MTemplateAction< WGTShowCommand, WGT_MAYA_COMMAND_SHOW, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus WGTShowCommand::doIt(const MArgList& args)
{
	if (!wgtApp || !wgtApp->started())
	{
		return MStatus::kFailure;
	}

	if (!wgtApp->visible())
	{
		wgtApp->show();
	}

	return MStatus::kSuccess;
}

struct WGTHideCommand : public MTemplateAction< WGTShowCommand, WGT_MAYA_COMMAND_HIDE, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus WGTHideCommand::doIt(const MArgList& args)
{
	if (!wgtApp || !wgtApp->started())
	{
		return MStatus::kFailure;
	}

	wgtApp->hide();
	return MStatus::kSuccess;
}

struct WGTStartCommand : public MTemplateAction< WGTShowCommand, WGT_MAYA_COMMAND_HIDE, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus WGTStartCommand::doIt(const MArgList& args)
{
	if (!wgtApp)
	{
		return MStatus::kFailure;
	}

	if (!wgtApp->started())
	{
		wgtApp->start();
	}
	return MStatus::kSuccess;
}

struct WGTStopCommand : public MTemplateAction< WGTShowCommand, WGT_MAYA_COMMAND_STOP, MTemplateCommand_nullSyntax >
{
	MStatus doIt(const MArgList& args) override;
};

MStatus WGTStopCommand::doIt(const MArgList& args)
{
	if (!wgtApp || !wgtApp->started())
	{
		return MStatus::kFailure;
	}

	wgtApp->stop();
	return MStatus::kSuccess;
}

MStatus initializeMayaPlugin(MObject obj)
{
	MStatus status;


    if (!Environment::getValue<MAX_PATH>( WGT_HOME, wgtHome ))
    {
#ifdef _WIN32
        GetModuleFileNameA( hApp, wgtHome, MAX_PATH );
        PathRemoveFileSpecA( wgtHome );
        Environment::setValue( WGT_HOME, wgtHome );
#endif // _WIN32

#ifdef __APPLE__
        Dl_info info;
        if (!dladdr( reinterpret_cast<void*>(setContext), &info ))
        {
            WGT_ERROR_MSG( "Generic plugin manager: failed to get current module file name%s", "\n" );
        }
        strcpy(wgtHome, info.dli_fname);
        const char* dir = dirname(wgtHome);
        Environment::setValue( WGT_HOME, dir);

        std::string dlybs = dir;
        dlybs += "/../PlugIns";
        Environment::setValue( "LD_LIBRARY_PATH", dlybs.c_str() );
#endif // __APPLE__
    }

#ifdef __APPLE__
    Environment::setValue( "QT_QPA_PLATFORM_PLUGIN_PATH", (std::string( wgtHome ) + "/../PlugIns/platforms").c_str() );
#else
    Environment::setValue( "QT_QPA_PLATFORM_PLUGIN_PATH", (std::string( wgtHome ) + "/platforms").c_str() );
#endif

#ifdef _WIN32
    size_t convertedChars = 0;
    mbstowcs_s( &convertedChars, exePath, MAX_PATH, wgtHome, _TRUNCATE );
    assert( convertedChars );
#endif // _WIN32

#ifdef __APPLE__
    std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
    wcscpy(exePath, conv.from_bytes( wgtHome ).c_str());
#endif // __APPLE__

	QDir::setCurrent( wgtHome );

    char path[2048];
    if(Environment::getValue<2048>( WGT_PATH, path ))
    {
        std::string newPath( "\"" );
        newPath += wgtHome;
        newPath += "\";";
        newPath += path;
        Environment::setValue( WGT_PATH, newPath.c_str() );
    }

#ifdef _WIN32
    SetDllDirectoryA( wgtHome );
#endif // _WIN32



	pluginManager = new GenericPluginManager();

	WGTShowCommand::registerCommand( obj );
	WGTHideCommand::registerCommand( obj );
	WGTStartCommand::registerCommand( obj );
	WGTStopCommand::registerCommand( obj );

	return loadWGT() ? MStatus::kSuccess : MStatus::kFailure;
}

MStatus uninitializeMayaPlugin(MObject obj)
{
	delete wgtApp;
	wgtApp = nullptr;
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
