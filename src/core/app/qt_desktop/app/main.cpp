#include "wg_memory/memory_overrides.hpp"

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
#include <csignal>

#include "core_common/platform_path.hpp"
#include "core_common/platform_dbg.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_qt_common/qt_framework.hpp"
#include "core_qt_common/qt_resource_system.hpp"
#include "core_qt_common/private/ui_view_creator.hpp"
#include "core_qt_common/qt_preferences.hpp"
#include "core_qt_common/qt_copy_paste_manager.hpp"
#include "core_logging/logging.hpp"
#include "core_file_logging/file_logger.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "common_include/env_pointer.hpp"
#include "common_include/qrc_loader.hpp"
#include "common_include/i_static_initializer.hpp"
#include "wg_memory/allocator.hpp"

#include "qt_desktop_application.hpp"

#if defined(USE_Qt5_WEB_ENGINE)
#include <QtWebEngine/QtWebEngine>
#endif

#include "core_wgtf_app/app_common.cpp"

WGT_INIT_QRC_LOADER

#ifndef APPLICATION_SETTINGS_NAME
#define APPLICATION_SETTINGS_NAME ""
#endif

namespace wgt
{
const std::wstring& getQtContextName()
{
	static const std::wstring QT_CONTEXT_NAME(L"Qt Context");
	return QT_CONTEXT_NAME;
}

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
#ifdef DEFAULT_PRE_CONFIG_FILE
		DEFAULT_PRE_CONFIG_FILE :
		DEFAULT_POST_CONFIG_FILE;
#else
#ifdef __APPLE__
		L"plugins_pre_qt_mac.txt" :
		L"plugins_post_qt_mac.txt";
#else
		L"plugins_pre_qt.txt" :
		L"plugins_post_qt.txt";
#endif
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

struct PluginManagerHelper
{
	PluginManagerHelper()
	{
		IPluginContextManager& contextManager = pluginManager.getContextManager();
		auto globalContext = contextManager.getGlobalContext();
		qtContext_ = contextManager.createContext(getQtContextName(), L"");
	}

	std::vector<std::wstring>& getPrePlugins()
	{
		return prePlugins;
	}

	std::vector<std::wstring>& getPostPlugins()
	{
		return postPlugins;
	}

	void init(CommandLineParser* clp, QApplication& app, QLocalServer* server);

	~PluginManagerHelper();

	IComponentContext* getContext()
	{
		return qtContext_;
	}

	int startApplication()
	{
		return desktopApp->startApplication();
	}

	void savePreferences()
	{
		qtContext_->queryInterface<IUIApplication>()->saveWindowPreferences();
	}

	std::vector<std::wstring> prePlugins;
	std::vector<std::wstring> postPlugins;

	QtDesktopApplication* desktopApp = nullptr;
	QtFramework* framework           = nullptr;
	QtCopyPasteManager* copyPaste    = nullptr;
	GenericPluginManager pluginManager;
	IComponentContext* qtContext_    = nullptr;
};

namespace AppCommonPrivate
{
static std::unique_ptr<FileLogger>& getFileLogger()
{
	static std::unique_ptr<FileLogger> s_FileLogger(new FileLogger());
	return s_FileLogger;
}
static std::unique_ptr<PluginManagerHelper>& getPluginManagerHelper()
{
    static std::unique_ptr<PluginManagerHelper> s_PluginManagerHelper(new PluginManagerHelper());
    return s_PluginManagerHelper;
}
void staticInitPlugin()
{
	s_Context = getPluginManagerHelper()->getContext();
	s_StaticInitPluginFunc = nullptr;
}
}

class QtPluginContextCreator : public Implements<IComponentContextCreator>
{
public:
	QtPluginContextCreator(QtFramework* qtFramework) : qtFramework_(qtFramework)
	{
	}

	InterfacePtr createContext(const wchar_t* contextId)
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

		return std::make_shared<InterfaceHolder<QtFramework>>(qtFramework_.get(), false);
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
	std::unique_ptr<QtFramework> qtFramework_;
};

//------------------------------------------------------------------------------
void PluginManagerHelper::init(CommandLineParser* clp, QApplication& app, QLocalServer* server)
{
	IPluginContextManager& contextManager = pluginManager.getContextManager();
	auto globalContext = contextManager.getGlobalContext();

	globalContext->registerInterface(clp);
	globalContext->registerInterface(new MemoryPluginContextCreator);

	auto staticInitializer = qtContext_->queryInterface<IStaticInitalizer>();
	if (staticInitializer)
	{
		staticInitializer->initStatics(*qtContext_);
	}

	pluginManager.runLoadStep(prePlugins);

	qtContext_->registerInterface(new QtResourceSystem());
	qtContext_->registerInterface(new UIViewCreator);

	AppCommonPrivate::getPluginManagerHelper()->desktopApp = new QtDesktopApplication(&app, server);
	AppCommonPrivate::getPluginManagerHelper()->desktopApp->setAppSettingsName(APPLICATION_SETTINGS_NAME);
	qtContext_->registerInterface(desktopApp);

	framework = new QtFramework(*qtContext_);
	qtContext_->registerInterface(new QtPluginContextCreator(framework));

	copyPaste = new QtCopyPasteManager();
	qtContext_->registerInterface(copyPaste);

	//Seems silly but needs to be called here to replicate 
	//regular plugin init order as the above Qt classes are usually 
	//called from within a plugin.
	if (staticInitializer)
	{
		staticInitializer->initStatics(*qtContext_);
	}

#if defined(USE_Qt5_WEB_ENGINE)
	QtWebEngine::initialize();
#endif

	pluginManager.runLoadStep(postPlugins);

	pluginManager.runInitiliseStep(prePlugins);

	auto definitionManager = globalContext->queryInterface<IDefinitionManager>();
	auto commandManager = globalContext->queryInterface<ICommandManager>();
	desktopApp->initialise();
	framework->initialise(*globalContext);

	pluginManager.runInitiliseStep(postPlugins);
}

//------------------------------------------------------------------------------
PluginManagerHelper::~PluginManagerHelper()
{
	pluginManager.runFinaliseStep(postPlugins);
	if (framework) framework->finalise();
	if (desktopApp) desktopApp->finalise();
	pluginManager.runFinaliseStep(prePlugins);

	pluginManager.runUnloadStep(postPlugins);

	IPluginContextManager& contextManager = pluginManager.getContextManager();
	pluginManager.runUnloadStep(prePlugins);

	std::vector<std::wstring> allPlugins;
	allPlugins.insert(allPlugins.end(), prePlugins.begin(), prePlugins.end());
	allPlugins.insert(allPlugins.end(), postPlugins.begin(), postPlugins.end());

	pluginManager.runDestroyStep(allPlugins);

	if (auto staticInitializer = qtContext_->queryInterface<IStaticInitalizer>())
	{
		staticInitializer->destroy();
	}

	contextManager.destroyContext(getQtContextName());
}

static bool IsMultipleInstance() 
{
	QString serverName = QString(MultiInstanceServer::getServerName().c_str());
	static QSharedMemory sharedMemory(serverName);
	return !(sharedMemory.create(4 /*sizeInBytes*/));
}

static bool SentArgvToOtherInstance(int argc, char **argv)
{
	QString serverName = QString(MultiInstanceServer::getServerName().c_str());
	QLocalSocket socket;
	const int NUM_RETRIES = 5;
	for (int i = 0; i < NUM_RETRIES; i++)
	{
		socket.connectToServer(serverName);
		if (socket.waitForConnected())
		{
			for (int i = 1; i < argc; i++) 
			{
				char *arg = argv[i];
				if (!arg[0] || !arg[1])             continue;
				if (arg[0] == '-' && arg[1] == '-') continue;

				QString data(arg);

				QByteArray byteArray;
				QDataStream out(&byteArray, QIODevice::WriteOnly);
				out.setVersion(QDataStream::Qt_5_6);
				out << data;

				socket.write(byteArray);
				socket.waitForBytesWritten();
			}
			socket.disconnectFromServer();
			return true;
		}
		else
		{
			Sleep(100 /*ms*/);
		}
	}

	NGT_WARNING_MSG("Detected master editor instance, "
		            "but could not connect to its' server: %s\n", serverName.toUtf8().data());
	return false;
}

#ifdef _WIN32
void purecallHandler(void)
{
	NGT_ERROR_MSG("Pure virtual function call\n");
	_exit(3);
}

void signalHandler(int signal)
{
	NGT_ERROR_MSG("Signal %d\n", signal);
	_exit(3);
}
#endif // _WIN32

int Main(int argc, char** argv)
{
	CommandLineParser* clp = new CommandLineParser(argc, argv);
	std::wstring postConfig = clp->getParamStrW("--postQtconfig");
	std::wstring preConfig = clp->getParamStrW("--preQtconfig");
	NGTAllocator::enableDebugOutput(clp->getFlag("--allocatorDebugOutput"));
	NGTAllocator::enableStackTraces(clp->getFlag("--allocatorStackTraces"));
	NGTAllocator::enableLeakDetection(clp->getFlag("--allocatorLeakDetection"));

	const bool unattended = clp->getFlag("-unattended");

#ifdef _WIN32
	if (unattended)
	{
		_set_purecall_handler(purecallHandler);

		typedef void(*SignalHandlerPointer)(int);
		SignalHandlerPointer previousHandler;
		previousHandler = signal(SIGINT, signalHandler);
		previousHandler = signal(SIGILL, signalHandler);
		previousHandler = signal(SIGFPE, signalHandler);
		previousHandler = signal(SIGSEGV, signalHandler);
		previousHandler = signal(SIGTERM, signalHandler);
		previousHandler = signal(SIGBREAK, signalHandler);
		previousHandler = signal(SIGABRT, signalHandler);
		previousHandler = signal(SIGABRT_COMPAT, signalHandler);

		_set_error_mode(_OUT_TO_STDERR);
		_set_abort_behavior(0, _WRITE_ABORT_MSG);
	}
#endif

	bool allowMultipleInstances = clp->getFlag("--allowMultipleInstances");
	if (!allowMultipleInstances)
	{
		if (IsMultipleInstance() && SentArgvToOtherInstance(argc, argv))
			return 1;
	}

	setCustomLoggingHandle([](LogLevel level, const char* message)
	{
		if (auto& logger = AppCommonPrivate::getFileLogger())
		{
			logger->log(level, message);
		}
	});

	QApplication app(argc, argv);
	GenericPluginManager pluginManager;
	IPluginContextManager& contextManager = pluginManager.getContextManager();
	auto globalContext = contextManager.getGlobalContext();

	if (s_StaticInitPluginFunc)
	{
		s_StaticInitPluginFunc();
	}

	std::vector<std::wstring>& prePlugins = AppCommonPrivate::getPluginManagerHelper()->getPrePlugins();
	if (!getPlugins(prePlugins, preConfig.c_str(), true) || prePlugins.empty())
	{
        return 2; // failed to find any plugins!
	}

	std::vector<std::wstring>& postPlugins = AppCommonPrivate::getPluginManagerHelper()->getPostPlugins();

	if (unattended)
	{
		postPlugins.push_back(L"plugins/plg_automation");
	}

	if (!getPlugins(postPlugins, postConfig.c_str(), false) || postPlugins.empty())
	{
        return 2; // failed to find any plugins!
	}

	int result = 1;
	{
		QLocalServer server;
		if (allowMultipleInstances) 
			AppCommonPrivate::getPluginManagerHelper()->init(clp, app, nullptr);
		else
			AppCommonPrivate::getPluginManagerHelper()->init(clp, app, &server);

		result = AppCommonPrivate::getPluginManagerHelper()->startApplication();
		AppCommonPrivate::getFileLogger()->onApplicationShutdown();
		AppCommonPrivate::getPluginManagerHelper()->savePreferences();
        AppCommonPrivate::getPluginManagerHelper().reset();

		AppCommonPrivate::getFileLogger()->onApplicationShutdown();
		setCustomLoggingHandle(nullptr);
	}
    return result;
}
} // namespace wgt

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
		__try
		{
			return wgt::Main(argc, argv);
		}
		__except (wgt::FileLogger::onFatalException(
			wgt::AppCommonPrivate::getFileLogger().get(),
			GetExceptionCode(), GetExceptionInformation()))
		{
			return EXIT_FAILURE;
		}
}