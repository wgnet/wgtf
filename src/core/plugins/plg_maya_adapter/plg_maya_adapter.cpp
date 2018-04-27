#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_qt_common/qt_copy_paste_manager.hpp"
#include "qt_framework_adapter.hpp"
#include "qt_application_adapter.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_common/platform_env.hpp"
#include "core_qt_common/private/ui_view_creator.hpp"
#include "core_common/platform_path.hpp"
#if defined(USE_Qt5_WEB_ENGINE)
#include <QtWebEngine/QtWebEngine>
#endif

#include <vector>
#include <QApplication>
#include <QLibraryInfo>
#include <QDir>
#include <QStringBuilder>

WGT_INIT_QRC_LOADER

namespace wgt
{
/**
* A plugin which registers the required interfaces for other plugins to communicate with the Maya plugin.
* Mutually exclusive with QtPluginApplication, QtPluginCommon and TestWindowPlugin.
*
* @ingroup plugins
* @ingroup coreplugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class MayaAdapterPlugin : public PluginMain, Depends<IUIFramework>
{
public:
	MayaAdapterPlugin(IComponentContext& contextManager)
	{
		contextManager.registerInterface(new UIViewCreator);
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		IPluginContextManager* pPluginContextManager = contextManager.queryInterface<IPluginContextManager>();

		if (pPluginContextManager && pPluginContextManager->getExecutablePath())
			QCoreApplication::addLibraryPath(pPluginContextManager->getExecutablePath());

		qtApplication_ = new QtApplicationAdapter();

#if defined(USE_Qt5_WEB_ENGINE)
		char wgtHome[MAX_PATH] = {};
		Environment::getValue<MAX_PATH>("WGT_HOME", wgtHome);

		QString dirPath = QCoreApplication::applicationDirPath();
		QString dataLocation = QLibraryInfo::location(QLibraryInfo::DataPath);
		dataLocation.remove(dirPath);
		dataLocation = wgtHome + dataLocation;

		QString fallback =
		QDir::homePath() % QLatin1String("/.") % QCoreApplication::applicationName() % QLatin1String("/");

		QDir dir(dataLocation);
		QFileInfoList infoList = dir.entryInfoList(QDir::Files);
		assert(infoList.count() > 0);

		for (QFileInfo& file : infoList)
		{
			QString src = file.absoluteFilePath();
			QString dest = fallback + file.fileName();
			QFile::copy(src, dest);
		}

		QtWebEngine::initialize();
#endif
		qtFramework_ = new QtFrameworkAdapter(contextManager);

		types_.push_back(contextManager.registerInterface(qtApplication_));
		types_.push_back(contextManager.registerInterface(qtFramework_));
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		auto commandsystem = contextManager.queryInterface<ICommandManager>();
		qtCopyPasteManager_ = new QtCopyPasteManager();
		types_.push_back(contextManager.registerInterface(qtCopyPasteManager_));

		qtFramework_->initialise(contextManager);

		SharedControls::init();

		qtApplication_->initialise();
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		qtApplication_->finalise();
		qtFramework_->finalise();
		qtCopyPasteManager_ = nullptr;
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}

		qtFramework_ = nullptr;
		qtApplication_ = nullptr;
	}

private:
	QtFramework* qtFramework_;
	QtApplication* qtApplication_;
	QtCopyPasteManager* qtCopyPasteManager_;
	std::vector<InterfacePtr> types_;
};

PLG_CALLBACK_FUNC(MayaAdapterPlugin)
} // end namespace wgt
