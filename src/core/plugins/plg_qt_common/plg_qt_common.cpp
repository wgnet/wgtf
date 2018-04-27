#include "core_generic_plugin/generic_plugin.hpp"

#include "core_qt_common/qt_copy_paste_manager.hpp"
#include "core_qt_common/qt_framework.hpp"
#include "core_qt_common/qt_application.hpp"
#include "core_qt_common/qt_resource_system.hpp"

#include "core_common/assert.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include <vector>

#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_qt_common/private/ui_view_creator.hpp"
#include "common_include/env_pointer.hpp"
#include "common_include/qrc_loader_helper.hpp"
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#if defined(USE_Qt5_WEB_ENGINE)
#include <QtWebEngine/QtWebEngine>
#endif

WGT_INIT_QRC_LOADER

namespace wgt
{
class QtPluginContextCreator : public Implements<IComponentContextCreator>
{
public:
	QtPluginContextCreator(std::unique_ptr<QtFramework>&& qtFramework) : qtFrameWork_(std::move(qtFramework))
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
				qtFrameWork_->addImportPath(in.readLine());
			}
			resourcePathFile.close();
		}

		return std::make_shared<InterfaceHolder<QtFramework>>(qtFrameWork_.get(), false);
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
	std::unique_ptr<QtFramework> qtFrameWork_;
};

/**
* A plugin which creates and registers IUIFramework and IViewCreator interfaces to allow creation of UI Components from
* Qt resources.
* Mutually exclusive with MayaAdapterPlugin.
*
* @ingroup plugins
* @ingroup coreplugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class QtPluginCommon : public PluginMain
{
public:
	QtPluginCommon(IComponentContext& contextManager)
	{
		types_.push_back(contextManager.registerInterface(new UIViewCreator()));
		types_.push_back(contextManager.registerInterface(new QtResourceSystem()));
		auto clp = contextManager.queryInterface<ICommandLineParser>();
		TF_ASSERT(clp != nullptr);
		qtApplication_ = new QtApplication(clp->argc(), clp->argv());
		types_.push_back(contextManager.registerInterface(qtApplication_));
#if defined(USE_Qt5_WEB_ENGINE)
		QtWebEngine::initialize();
#endif
		std::unique_ptr<QtFramework> qtFramework(new QtFramework(contextManager));
		qtFramework_ = qtFramework.get();
		types_.push_back(contextManager.registerInterface(new QtPluginContextCreator(std::move(qtFramework))));
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		qtApplication_->initialise();
		auto commandsystem = contextManager.queryInterface<ICommandManager>();

		qtCopyPasteManager_ = new QtCopyPasteManager();
		types_.push_back(contextManager.registerInterface(qtCopyPasteManager_));
		qtFramework_->initialise(contextManager);
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		qtCopyPasteManager_ = nullptr;
		qtFramework_->finalise();
		qtApplication_->finalise();
		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
		qtApplication_ = nullptr;
	}

private:
	QtFramework* qtFramework_;
	QtCopyPasteManager* qtCopyPasteManager_;
	QtApplication* qtApplication_;
	InterfacePtrs types_;
};

PLG_CALLBACK_FUNC(QtPluginCommon)
} // end namespace wgt
