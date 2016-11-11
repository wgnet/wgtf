#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin_manager/folder_plugin_loader.hpp"
#include "core_generic_plugin_manager/config_plugin_loader.hpp"

#include "core_wgtf_app/command_line_parser.hpp"
#include "core_wgtf_app/memory_plugin_context_creator.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>
#include <QTextStream>

#include "core_common/platform_path.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_qt_common/qt_framework.hpp"
#include "core_qt_common/private/ui_view_creator.hpp"
#include "core_qt_common/qt_preferences.hpp"
#include "core_qt_common/qt_copy_paste_manager.hpp"
#include "qt_desktop_application.hpp"

#if defined(USE_Qt5_WEB_ENGINE)
#include <QtWebEngine/QtWebEngine>
#endif

namespace wgt
{
#ifdef _WIN32
static const wchar_t* const pluginsFolder = L"plugins\\";
#elif __APPLE__
static const wchar_t* const pluginsFolder = L"../Resources/plugins/";
#endif // __APPLE__

bool getPlugins(std::vector<std::wstring>& plugins, const wchar_t* configFile, bool preQtPlugins)
{
	wchar_t path[MAX_PATH];
	::GetModuleFileNameW(NULL, path, MAX_PATH);
	::PathRemoveFileSpecW(path);

	if ((configFile == NULL) || (wcscmp(configFile, L"") == 0))
	{
		::PathAppendW(path, pluginsFolder);

		const wchar_t* filename = preQtPlugins ?
#ifdef __APPLE__
		L"plugins_pre_qt_mac.txt" :
		L"plugins_post_qt_mac.txt";
#else
		L"plugins_pre_qt.txt" :
		L"plugins_post_qt.txt";
#endif
		return ConfigPluginLoader::getPlugins(plugins, std::wstring(path) + filename) ||
		FolderPluginLoader::getPluginsCustomPath(plugins, path);
	}
	else
	{
		::PathAppendW(path, pluginsFolder);
		::PathAppendW(path, configFile);
		return ConfigPluginLoader::getPlugins(plugins, path);
	}
}

class QtPluginContextCreator : public Implements<IComponentContextCreator>
{
public:
	QtPluginContextCreator(QtFramework* qtFramework) : qtFramework_(qtFramework)
	{
	}

	IInterface* createContext(const wchar_t* contextId)
	{
		QFile resourcePathFile(getResourcePathFile(contextId));
		if (resourcePathFile.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream in(&resourcePathFile);
			while (!in.atEnd())
			{
				qtFramework_->addImportPath(in.readLine());
			}
			resourcePathFile.close();
		}

		return new InterfaceHolder<QtFramework>(qtFramework_, false);
	}

	const char* getType() const
	{
		return typeid(QtFramework).name();
	}

private:
	QString getResourcePathFile(const wchar_t* contextId)
	{
		QString path = QString::fromWCharArray(contextId);
		path.remove(0, path.lastIndexOf('/'));
		path.prepend(":");
		path.append("/resource_paths.txt");
		return path;
	}
	QtFramework* qtFramework_;
};

int Main(int argc, char** argv)
{
	QApplication app(argc, argv);
	GenericPluginManager pluginManager;
	IPluginContextManager& contextManager = pluginManager.getContextManager();

	CommandLineParser* clp = new CommandLineParser(argc, argv);
	auto globalContext = contextManager.getGlobalContext();

	std::wstring postConfig;
	std::wstring preConfig;
	preConfig = clp->getParamStrW("--preQtconfig");
	postConfig = clp->getParamStrW("--postQtconfig");

	std::vector<std::wstring> prePlugins;
	if (!getPlugins(prePlugins, preConfig.c_str(), true) || prePlugins.empty())
	{
		return 2; // failed to find any plugins!
	}

	std::vector<std::wstring> postPlugins;
	if (!getPlugins(postPlugins, postConfig.c_str(), false) || postPlugins.empty())
	{
		return 2; // failed to find any plugins!
	}
	int result = 1;
	{
		GenericPluginManager pluginManager;
		IPluginContextManager& contextManager = pluginManager.getContextManager();

		auto globalContext = contextManager.getGlobalContext();
		globalContext->registerInterface(clp);
		globalContext->registerInterface(new MemoryPluginContextCreator);

		pluginManager.beginIncrementalPluginLoading();

		pluginManager.pushPlugins(prePlugins);

		globalContext->registerInterface(new UIViewCreator(*globalContext));
		auto framework = new QtFramework(*globalContext);
		globalContext->registerInterface(new QtPluginContextCreator(framework));

		auto desktopApp = new QtDesktopApplication(*globalContext, &app);
		globalContext->registerInterface(desktopApp);
		globalContext->registerInterface(new QtPreferences(*globalContext));
		auto copyPaste = new QtCopyPasteManager();
		globalContext->registerInterface(copyPaste);

#if defined(USE_Qt5_WEB_ENGINE)
		QtWebEngine::initialize();
#endif
		pluginManager.pushPlugins(postPlugins);

		auto definitionManager = globalContext->queryInterface<IDefinitionManager>();
		auto commandManager = globalContext->queryInterface<ICommandManager>();

		desktopApp->initialise();
		framework->initialise(*globalContext);
		copyPaste->init(definitionManager, commandManager);

		pluginManager.endIncrementalPluginLoading();

		result = desktopApp->startApplication();
	}
	return result;
}
}

#ifdef _WIN32
int STDMETHODCALLTYPE WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	int argc = __argc;
	char** argv = __argv;
#endif // _WIN32

#ifdef __APPLE__
	int main(int argc, char** argv, char** envp, char** apple)
	{
#endif // __APPLE__

		return wgt::Main(argc, argv);
	}
