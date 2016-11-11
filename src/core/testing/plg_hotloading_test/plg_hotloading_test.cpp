#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qml_view.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_string_utils/file_path.hpp"
#include "hotloading_panel.hpp"
#include "metadata/hotloading_panel.mpp"

#include <memory>
#include <QFileSystemWatcher>
#include <QUrl>
#include <QQmlEngine>

namespace wgt
{
/**
* This plugin is used to test QML hotloading of multiple files
*
* @ingroup plugins
* @image html plg_hotloading_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class HotloadingTestPlugin : public PluginMain
{
public:
	HotloadingTestPlugin(IComponentContext& componentContext)
	{
	}

	bool PostLoad(IComponentContext& componentContext) override
	{
		auto qtFramework = componentContext.queryInterface<IQtFramework>();
		auto qmlEngine = qtFramework->qmlEngine();
		qmlEngine->addImportPath(QString(PROJECT_RESOURCE_FOLDER));
		return true;
	}

	void Initialise(IComponentContext& componentContext) override
	{
		IDefinitionManager& definitionManager = *componentContext.queryInterface<IDefinitionManager>();
		REGISTER_DEFINITION(HotloadingPanel);
		auto hotloadingPanel = definitionManager.create<HotloadingPanel>();

		auto qtFramework = componentContext.queryInterface<IQtFramework>();
		auto fileSystem = componentContext.queryInterface<IFileSystem>();
		auto qmlEngine = qtFramework->qmlEngine();
		hotloadingPanel->initialise(*qmlEngine, *fileSystem);

		auto viewCreator = componentContext.queryInterface<IViewCreator>();
		if (viewCreator != nullptr)
		{
			auto onViewLoad = [qtFramework, hotloadingPanel, this](IView& view) {
				std::vector<std::string> files;
				files.push_back("WGHotloadingPanel.qml");
				files.push_back("WGHotloadingBase.qml");
				files.push_back("WGHotloading.qml");
				files.push_back("wg_hotloading.js");

				const bool watchingFiles = isWatchingFiles(qtFramework, files);
				assert(watchingFiles);
				if (!watchingFiles)
				{
					hotloadingPanel->setErrorText("ERROR: Was not watching required files for hotloading");
				}

				std::vector<std::string> components;
				components.push_back("WGPanel");
				components.push_back("WGHotloadingBase");
				components.push_back("WGHotloading");

				const bool watchingComponents = isWatchingComponents(view, components);
				assert(watchingComponents);
				if (!watchingComponents)
				{
					hotloadingPanel->setErrorText("ERROR: Was not watching required components for hotloading");
				}
			};

			std::string path(PROJECT_RESOURCE_FOLDER);
			path += "WGHotloadingPanel.qml";
			hotloadingView_ = viewCreator->createView(path.c_str(), hotloadingPanel, onViewLoad);
		}
	}

	bool Finalise(IComponentContext& componentContext) override
	{
		auto uiApplication = componentContext.queryInterface<IUIApplication>();
		if (uiApplication == nullptr)
		{
			return false;
		}

		if (hotloadingView_.valid())
		{
			auto view = hotloadingView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}

		return true;
	}

	void Unload(IComponentContext& componentContext) override
	{
	}

private:
	bool isWatchingComponents(IView& view, const std::vector<std::string>& componentsToCheck)
	{
		auto* panel = dynamic_cast<QmlView*>(&view);
		if (!panel)
		{
			return false;
		}

		const std::set<QString>& components = panel->componentTypes();
		for (const std::string& component : componentsToCheck)
		{
			if (components.find(component.c_str()) == components.end())
			{
				return false;
			}
		}

		return true;
	}

	bool isWatchingFiles(IQtFramework* qtFramework, const std::vector<std::string>& filesToCheck)
	{
		const QStringList files = qtFramework->qmlWatcher()->files();

		for (const std::string& file : filesToCheck)
		{
			auto findFn = [file](const QString& watchedFile) {
				return FilePath::getFileWithExtension(watchedFile.toUtf8().constData()) == file;
			};

			if (std::find_if(files.begin(), files.end(), findFn) == files.end())
			{
				return false;
			}
		}
		return true;
	}

	wg_future<std::unique_ptr<IView>> hotloadingView_;
};

PLG_CALLBACK_FUNC(HotloadingTestPlugin)

} // end namespace wgt