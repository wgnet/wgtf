#include "qt_framework.hpp"

#include "qt_preferences.hpp"
#include "qml_component_manager.hpp"

#include "core_data_model/i_item_role.hpp"

#include "core_qt_common/qt_types.hpp"
#include "core_qt_common/qml_component.hpp"
#include "core_qt_common/qml_view.hpp"
#include "core_qt_common/qt_view.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_qt_common/qt_default_spacing.hpp"
#include "core_qt_common/qt_global_settings.hpp"
#include "core_qt_common/qt_window.hpp"
#include "core_qt_common/qml_window.hpp"
#include "core_qt_common/qml_dialog.hpp"
#include "core_qt_common/qt_exe_icon_provider.hpp"
#include "core_qt_common/qt_image_provider.hpp"
#include "core_qt_common/qt_image_provider_old.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/qt_scripting_engine.hpp"
#include "core_qt_common/qt_script_object.hpp"
#include "core_qt_common/qt_framework_common.hpp"
#include "core_qt_common/qt_progress_dialog.hpp"
#include "core_qt_common/qt_system_tray_icon.hpp"
#include "core_qt_common/models/extensions/model_extension_manager.hpp"

#include "core_common/assert.hpp"
#include "core_common/platform_env.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_command_system/i_command_event_listener.hpp"
#include "core_command_system/i_command_manager.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_component_provider.hpp"
#include "core_ui_framework/generic_component_provider.hpp"

#include "core_logging/logging.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"

#include "qt_action_manager.hpp"

#include "core_data_model/dialog/dialog_model.hpp"
#include "core_object/managed_object.hpp"

#include "wg_types/string_ref.hpp"
#include "core_common/ngt_windows.hpp"

#include "private/qt_ui_worker.hpp"

#include "core_dependency_system/context_callback_helper.hpp"
#include "core_qt_common/reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include <thread>
#include <array>
#include <algorithm>

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QString>
#include <QWidget>
#include <QMessageBox>
#include <QQmlIncubationController>
#include <QFileSystemWatcher>
#include <QProcess>
#include <QClipboard>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include "core_serialization_xml/simple_api_for_xml.hpp"
#include "qt_thumbnail_provider.hpp"
#include "core_ui_framework/i_thumbnail_provider.hpp"

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE
{
#endif

bool qRegisterResourceData(int, const unsigned char*, const unsigned char*, const unsigned char*);

bool qUnregisterResourceData(int, const unsigned char*, const unsigned char*, const unsigned char*);

#ifdef QT_NAMESPACE
}
using namespace QT_NAMESPACE;
#endif

namespace wgt
{

namespace QtFramework_Locals
{
// Temporary command event listener to handle process events when the command
// thread blocks
class QtCommandEventListener : public ICommandEventListener
{
public:
	void progressMade(const CommandInstance& commandInstance) const override
	{
		QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	}
};

std::string searchInDirectory(const QStringList& execs, QString directory)
{
	const QChar slash = QLatin1Char('/');
	if (directory.isEmpty())
		return "";
	// Avoid turing / into // on windows which triggers windows to check
	// for network drives!
	if (!directory.endsWith(slash))
		directory += slash;

	foreach (const QString& exec, execs)
	{
		QFileInfo fi(directory + exec);
		if (fi.exists() && fi.isFile() && fi.isExecutable())
			return fi.absoluteFilePath().toUtf8().constData();
	}
	return "";
}

std::string explorerPath()
{
	QMap<QString, QString> values_map;
	auto values = QProcessEnvironment::systemEnvironment().toStringList();
	foreach (const QString& s, values)
	{
		int i = s.indexOf(QLatin1Char('='), 1);
		if (i >= 0)
		{
			values_map.insert(s.left(i).toUpper(), s.mid(i + 1));
		}
	}

	QString exec = QDir::cleanPath(QLatin1String("explorer.exe"));
	QFileInfo fi(exec);

	QStringList execs(exec);
	// Check all the executable extensions on windows:
	// PATHEXT is only used if the executable has no extension
	if (fi.suffix().isEmpty())
	{
		QStringList extensions = values_map.value(QLatin1String("PATHEXT")).split(QLatin1Char(';'));

		foreach (const QString& ext, extensions)
		{
			QString tmp = "explorer.exe" + ext.toLower();
			if (fi.isAbsolute())
			{
				if (QFile::exists(tmp))
					return tmp.toUtf8().constData();
			}
			else
			{
				execs << tmp;
			}
		}
	}

	if (fi.isAbsolute())
		return exec.toUtf8().constData();

	QSet<QString> alreadyChecked;
	QStringList list = values_map.value(QLatin1String("PATH")).split(";", QString::SkipEmptyParts);
	foreach (const QString& p, list)
	{
		if (alreadyChecked.contains(p))
			continue;
		alreadyChecked.insert(p);
		std::string tmp = searchInDirectory(execs, QDir::fromNativeSeparators(p));
		if (!tmp.empty())
			return tmp;
	}
	return "";
}

const std::string ROOT = "Icons";
const std::string ICON = "icon";

struct IconData
{
	std::string id_;
	std::string icon_;
};

class IconsReader : private SimpleApiForXml
{
	typedef SimpleApiForXml base;

public:
	IconsReader(TextStream& stream, std::unordered_map<std::string, std::unique_ptr<IconData>>& iconData, QtImageProvider& imageProvider)
		: base(stream), iconData_(iconData), imageProvider_(imageProvider)
	{
	}
	bool read()
	{
		return parse();
	}
	virtual void elementStart(const char* elementName, const char* const* attributes) override
	{
		if (elementName == ROOT)
		{
			return;
		}

		std::string id(elementName);
		std::transform(id.begin(), id.end(), id.begin(), ::tolower);
		IconData* iconData = nullptr;
		auto it = iconData_.find(id);
		if (it == iconData_.end())
		{
			iconData = new IconData;
			iconData_[id] = std::unique_ptr<IconData>(iconData);
		}
		else
		{
			iconData = it->second.get();
		}
		iconData->id_ = elementName;
		std::string tmp;
		// parse attributes
		for (auto attribute = attributes; *attribute; attribute += 2)
		{
			const char* attributeName = attribute[0];
			const char* attributeValue = attribute[1];

			if (attributeName == ICON)
			{
				auto imagePath = imageProvider_.encode(attributeValue);
				iconData->icon_ = imagePath.toUtf8().constData();
			}
			else
			{
				// ignore unknown attributes
				NGT_WARNING_MSG("Warning: unknown icon data attribute \"%s\".\n", attributeName);
			}
		}
	}

private:
	std::unordered_map<std::string, std::unique_ptr<IconData>>& iconData_;
	QtImageProvider& imageProvider_;
};


}

// Modeless Dialog accessor
enum ModelessDialogType
{
	DIALOG,
	CLOSED,
	CONNECTION
};

struct QtFramework::Impl : public Depends<ICommandManager, IUIApplication, IViewCreator,
                                          IPluginContextManager, IUIFramework, IDefinitionManager, ILoggingSystem>
	, public ContextCallBackHelper
{
	Impl()
	{
		registerCallback([ this ](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
			actionManager_ = std::make_unique<QtActionManager>();
		});
	}

	void finalise()
	{
		actionManager_.reset();
		iconData_.clear();
		thumbnailProviders_.clear();
		thumbnailCache_.clear();
	}

	std::unique_ptr<ActionManager> actionManager_;
	std::unordered_map<std::string, std::unique_ptr<QtFramework_Locals::IconData>> iconData_;
	std::set<std::weak_ptr<IThumbnailProvider>, std::owner_less<std::weak_ptr<IThumbnailProvider>>> thumbnailProviders_;
	QHash<QString, QImage> thumbnailCache_;
};

//// Ensure the QtFileDialogOptions enumeration matches so we can do a simple cast
static_assert(IUIFramework::QtFileDialogOptions::ShowDirsOnly == QFileDialog::ShowDirsOnly,
              "QtFileDialogOptions::ShowDirsOnly must match QFileDialog::ShowDirsOnly");
static_assert(IUIFramework::QtFileDialogOptions::DontResolveSymlinks == QFileDialog::DontResolveSymlinks,
              "QtFileDialogOptions::DontResolveSymlinks must match QFileDialog::DontResolveSymlinks");
static_assert(IUIFramework::QtFileDialogOptions::DontConfirmOverwrite == QFileDialog::DontConfirmOverwrite,
              "QtFileDialogOptions::DontConfirmOverwrite must match QFileDialog::DontConfirmOverwrite");
static_assert(IUIFramework::QtFileDialogOptions::DontUseSheet == QFileDialog::DontUseSheet,
              "QtFileDialogOptions::DontUseSheet must match QFileDialog::DontUseSheet");
static_assert(IUIFramework::QtFileDialogOptions::DontUseNativeDialog == QFileDialog::DontUseNativeDialog,
              "QtFileDialogOptions::DontUseNativeDialog must match QFileDialog::DontUseNativeDialog");
static_assert(IUIFramework::QtFileDialogOptions::ReadOnly == QFileDialog::ReadOnly,
              "QtFileDialogOptions::ReadOnly must match QFileDialog::ReadOnly");
static_assert(IUIFramework::QtFileDialogOptions::HideNameFilterDetails == QFileDialog::HideNameFilterDetails,
              "QtFileDialogOptions::HideNameFilterDetails must match QFileDialog::HideNameFilterDetails");
static_assert(IUIFramework::QtFileDialogOptions::DontUseCustomDirectoryIcons ==
              QFileDialog::DontUseCustomDirectoryIcons,
              "QtFileDialogOptions::DontUseCustomDirectoryIcons must match QFileDialog::DontUseCustomDirectoryIcons");

QtFramework::QtFramework(IComponentContext& contextManager)
    : impl_(new Impl()), qmlWatcher_(new QFileSystemWatcher()), context_(contextManager), worker_(new QtUIWorker())
{
	qtFrameworkBase_.reset(new QtFrameworkCommon(std::make_unique<QQmlEngine>(), std::make_unique<QtScriptingEngine>(),
	                                             impl_->get<IDefinitionManager>()));

	interfaces_.push_back(contextManager.registerInterface(new QtHelpers));
	interfaces_.push_back(contextManager.registerInterface(impl_->actionManager_.get(), false));
}

QtFramework::~QtFramework()
{
}

void QtFramework::initialise(IComponentContext& contextManager)
{
	// This needs to be set after qtFramework has been constructed and QmlEngine has been created.
	// This will only occur when running from a plugin scenario such as Maya.
	auto pPluginContextManager = impl_->get<IPluginContextManager>();
	if (pPluginContextManager && pPluginContextManager->getExecutablePath())
	{
		qmlEngine()->addPluginPath(pPluginContextManager->getExecutablePath());
		qmlEngine()->addImportPath(pPluginContextManager->getExecutablePath());
	}

	SharedControls::init();

	registerReloadableFiles();

	qtFrameworkBase_->initialise();

	auto rootContext = qmlEngine()->rootContext();
	Variant obj = &contextManager;
	rootContext->setContextProperty("componentContext", toQVariant(obj, rootContext));

	qmlEngine()->addImageProvider(QtImageProvider::providerId(), new QtImageProvider());
	qmlEngine()->addImageProvider(QtImageProviderOld::providerId(), new QtImageProviderOld());
	qmlEngine()->addImageProvider(QtThumbnailProvider::providerId(), new QtThumbnailProvider(*this));

#if defined( _WIN32 )
	// QQmlEngine::addImageProvider takes ownership
	auto pFileIconProvider = new QtExeIconProvider();
	qmlEngine()->addImageProvider(QtExeIconProvider::providerId(), pFileIconProvider);
#endif // defined( _WIN32 )

	auto commandManager = impl_->get<ICommandManager>();
	if (commandManager != nullptr)
	{
		commandEventListener_.reset(new QtFramework_Locals::QtCommandEventListener);
		commandManager->registerCommandStatusListener(commandEventListener_.get());
	}

	interfaces_.push_back(contextManager.registerInterface(new QtPreferences));
#if defined( _WIN32 )
	interfaces_.push_back(contextManager.registerInterface(pFileIconProvider,
		false /* transferOwnership */));
#endif // defined( _WIN32 )

	impl_->actionManager_->init();
	auto uiApplication = impl_->get<IUIApplication>();
	TF_ASSERT(uiApplication != nullptr);
	connections_ += uiApplication->signalStartUp.connect(std::bind(&QtFramework::onApplicationStartUp, this));
	connections_ += uiApplication->signalExit.connect(std::bind(&QtFramework::onApplicationExit, this));
}

QMetaObject* QtFramework::constructMetaObject(QMetaObject* original)
{
	if (original == nullptr)
	{
		return original;
	}

	char* originalName = (char*)original->d.stringdata->data();

	bool found = std::binary_search(qtNames, qtNames + qtTypeCount, originalName,
	                                [](const char* const a, const char* const b) { return strcmp(a, b) < 0; });

	if (found)
	{
		return original;
	}

	for (size_t i = 0; i < registeredTypes_.size(); ++i)
	{
		char* name = (char*)registeredTypes_[i].get()->d.stringdata->data();
		if (strcmp(name, originalName) == 0)
		{
			return registeredTypes_[i].get();
		}
	}

	std::unique_ptr<QMetaObject> createdMeta(new QMetaObject);
	*createdMeta = *original;
	createdMeta->d.superdata = constructMetaObject((QMetaObject*)createdMeta->d.superdata);
	registeredTypes_.push_back(std::move(createdMeta));

	return registeredTypes_.back().get();
}

void QtFramework::registerQmlType(const ObjectHandle& type)
{
	QQmlPrivate::RegisterType* qtype = type.getBase<QQmlPrivate::RegisterType>();
	TF_ASSERT(qtype != nullptr);

	if (qtype)
	{
		qtype->metaObject = constructMetaObject((QMetaObject*)qtype->metaObject);
		qtype->extensionMetaObject = constructMetaObject((QMetaObject*)qtype->extensionMetaObject);
		qtype->attachedPropertiesMetaObject = constructMetaObject((QMetaObject*)qtype->attachedPropertiesMetaObject);

		QQmlPrivate::qmlregister(QQmlPrivate::TypeRegistration, qtype);
	}
}

void QtFramework::finalise()
{
	auto commandManager = impl_->get<ICommandManager>();
	if (commandManager != nullptr)
	{
		commandManager->deregisterCommandStatusListener(commandEventListener_.get());
	}
	if (shortcutDialog_ != nullptr)
	{
		auto uiApplication = impl_->get<IUIApplication>();
		TF_ASSERT(uiApplication != nullptr);
		uiApplication->removeWindow(*shortcutDialog_);
	}

	unregisterResources();
	impl_->actionManager_->fini();
	shortcutDialog_ = nullptr;
	qmlEngine()->removeImageProvider(QtThumbnailProvider::providerId());
	qmlEngine()->removeImageProvider(QtImageProviderOld::providerId());
	qmlEngine()->removeImageProvider(QtImageProvider::providerId());

	for (auto i : interfaces_)
	{
		context_.deregisterInterface(i.get());
	}

#if defined( _WIN32 )
	// QtExeIconProvider is one of the interfaces_ to be deregistered.
	// Removing it before deregistration causes a use-after-free error.
	qmlEngine()->removeImageProvider(QtExeIconProvider::providerId());
#endif // defined( _WIN32 )

	interfaces_.clear();
	connections_.clear();
	qtFrameworkBase_->finalise();
	impl_->finalise();
}

QImage QtFramework::requestThumbnail(const QString& filePath, const QSize& requestedSize)
{
	auto&& it = impl_->thumbnailCache_.find(filePath);
	if (it != impl_->thumbnailCache_.end())
	{
		return requestedSize.isValid() ? it.value().scaled(requestedSize) : it.value();
	}
	
	QImage image;
	std::string path = filePath.toUtf8().constData();
	bool bOk = false;
	for (auto&& it : impl_->thumbnailProviders_)
	{
		if (it.expired())
		{
			continue;
		}
		auto provider = it.lock();
		TF_ASSERT(provider != nullptr);
		int width, height, rowPitch;
		BinaryBlock imageBlock;
		if (provider->getThumbnailData(path.c_str(), &width, &height, &rowPitch, &imageBlock))
		{
			bOk = true;
			QImage img(reinterpret_cast<const uchar*>(imageBlock.data()), width, height, rowPitch, QImage::Format_RGB32);
			image.swap(img);
			break;
		}
	}
	if (!bOk)
	{
		bOk = image.load(filePath);
	}
	if (!bOk)
	{
		return QImage(requestedSize.width(), requestedSize.height(), QImage::Format_ARGB32);
	}
	impl_->thumbnailCache_[filePath] = image;
	return requestedSize.isValid() ? image.scaled(requestedSize) : image;
}

QQmlEngine* QtFramework::qmlEngine() const
{
	return qtFrameworkBase_->qmlEngine();
}

QFileSystemWatcher* QtFramework::qmlWatcher() const
{
	return qmlWatcher_.get();
}

void QtFramework::setIncubationTime(int msecs)
{
	qtFrameworkBase_->setIncubationTime(msecs);
}

void QtFramework::incubate()
{
	if (qmlEngine() == nullptr)
	{
		return;
	}
	auto incubationCtrl = qmlEngine()->incubationController();
	if (incubationCtrl != nullptr)
	{
		if (incubationCtrl->incubatingObjectCount() > 0)
		{
			incubationCtrl->incubateFor(qtFrameworkBase_->incubationTime());
		}
			}
		}

QtPalette* QtFramework::palette() const
{
	return qtFrameworkBase_->palette();
}

void QtFramework::addImportPath(const QString& path)
{
	QDir importPath(path);
	if (importPath.exists() && importPath.isReadable())
	{
		qtFrameworkBase_->qmlEngine()->addImportPath(path);
	}
}

QtGlobalSettings* QtFramework::qtGlobalSettings() const
{
	return qtFrameworkBase_->qtGlobalSettings();
}

void QtFramework::registerTypeConverter(IQtTypeConverter& converter) /* override */
{
	qtFrameworkBase_->registerTypeConverter(converter);
}

void QtFramework::deregisterTypeConverter(IQtTypeConverter& converter) /* override */
{
	qtFrameworkBase_->deregisterTypeConverter(converter);
}

bool QtFramework::registerResourceData(const unsigned char* qrc_struct, const unsigned char* qrc_name,
                                       const unsigned char* qrc_data)
{
	if (!qRegisterResourceData(0x01, qrc_struct, qrc_name, qrc_data))
	{
		return false;
	}

	registeredResources_.push_back(std::make_tuple(qrc_struct, qrc_name, qrc_data));
	return true;
}

QVariant QtFramework::toQVariant(const Variant& variant, QObject* parent) const
{
	return qtFrameworkBase_->toQVariant(variant, parent);
}

Variant QtFramework::toVariant(const QVariant& qVariant) const
{
	return qtFrameworkBase_->toVariant(qVariant);
}

QQmlComponent* QtFramework::toQmlComponent(IComponent& component)
{
	return qtFrameworkBase_->qmlComponentManager()->toQmlComponent(component);
}

QWidget* QtFramework::toQWidget(IView& view)
{
	// TODO replace this with a proper UI adapter interface
	auto qmlView = dynamic_cast<IQtView*>(&view);
	if (qmlView != nullptr)
	{
		auto widget = qmlView->releaseWidget();
		widget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
		widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		widget->setFocusPolicy(Qt::StrongFocus);
		return widget;
	}

	auto widget = dynamic_cast<QWidget*>(&view);
	if (widget != nullptr)
	{
		return widget;
	}

	return nullptr;
}

void QtFramework::retainQWidget(IView& view)
{
	// TODO replace this with a proper UI adapter interface
	auto qmlView = dynamic_cast<IQtView*>(&view);
	if (qmlView != nullptr)
	{
		qmlView->retainWidget();
	}
}

QString QtFramework::resolveFilePath(const char* relativePath) const
{
	return qtFrameworkBase_->resolveFilePath(relativePath);
}

QUrl QtFramework::resolveQmlPath(const char* relativePath) const
{
	return qtFrameworkBase_->resolveQmlPath(relativePath);
}

QString QtFramework::resolveFilePath(const QQmlEngine& qmlEngine, const char* relativePath) const
{
	return qtFrameworkBase_->resolveFilePath(qmlEngine, relativePath);
}

QUrl QtFramework::resolveQmlPath(const QQmlEngine& qmlEngine, const char* relativePath) const
{
	return qtFrameworkBase_->resolveQmlPath(qmlEngine, relativePath);
}

std::unique_ptr<ISystemTrayIcon> QtFramework::createSystemTrayIcon(const char* iconPath)
{
	return std::make_unique<QtSystemTrayIcon>(iconPath);
}

std::unique_ptr<IAction> QtFramework::createAction(
	const char* id, 
	std::function<void(IAction*)> func,
	std::function<bool(const IAction*)> enableFunc,
	std::function<bool(const IAction*)> checkedFunc,
	std::function<bool(const IAction*)> visibleFunc)
{
	return impl_->actionManager_->createAction(id, func, enableFunc, checkedFunc, visibleFunc);
}

std::unique_ptr<IAction> QtFramework::createAction(
	const char* id, 
	const char* text, 
	const char* path,
	std::function<void(IAction*)> func,
	std::function<bool(const IAction*)> enableFunc,
	std::function<bool(const IAction*)> checkedFunc,
	std::function<bool(const IAction*)> visibleFunc, int actionOrder)
{
	return impl_->actionManager_->createAction(id, text, path, actionOrder, func, enableFunc, checkedFunc, visibleFunc);
}

std::unique_ptr<IAction> QtFramework::createAction(
	const char* id,
	const char* text,
	const char* path,
	std::function<void(IAction*)> func,
	int actionOrder)
{
	return impl_->actionManager_->createAction(id, text, path, actionOrder, func, 
		[](const IAction*) { return true; },
		std::function<bool(const IAction*)>(nullptr),
		[](const IAction*) { return true; });
}

std::unique_ptr<IAction> QtFramework::createSeperator(const char* id, const char* path, int actionOrder)
{
	return impl_->actionManager_->createSeperator(id, path, actionOrder);
}

std::unique_ptr<IAction> QtFramework::createSeperator(const char* id)
{
	return impl_->actionManager_->createSeperator(id);
}

std::unique_ptr<IComponent> QtFramework::createComponent(const char* resource, ResourceType type)
{
	QUrl url;
	switch (type)
	{
	case IUIFramework::ResourceType::File:
		url = QUrl::fromLocalFile(resource);
		break;

	case IUIFramework::ResourceType::Url:
		url = resolveQmlPath(resource);
		break;
	}

	auto qmlComponent = qtFrameworkBase_->createComponent(url);
	if (type == IUIFramework::ResourceType::Buffer)
	{
		qmlComponent->component()->setData(resource, QUrl());
	}
	return std::unique_ptr<IComponent>(qmlComponent);
}

//------------------------------------------------------------------------------
void QtFramework::enableAsynchronousViewCreation(bool enabled)
{
	this->useAsyncViewLoading_ = enabled;
}

//------------------------------------------------------------------------------
std::unique_ptr<IView> QtFramework::createView(const char* resource, ResourceType type, const Variant& context)
{
	NGT_DEBUG_MSG("Deprecated function call, please use async version instead");
	auto returnView = createViewInternal(nullptr, resource, type, context,
	                                     [](IView& view) {

		                                 },
	                                     false);
	return returnView.get();
}

//------------------------------------------------------------------------------
std::unique_ptr<IView> QtFramework::createView(const char* uniqueName, const char* resource, ResourceType type,
                                               const Variant& context)
{
	NGT_DEBUG_MSG("Deprecated function call, please use async version instead");
	auto returnView = createViewInternal(uniqueName, resource, type, context, [](IView& view) {}, false);
	return returnView.get();
}

//------------------------------------------------------------------------------
wg_future<std::unique_ptr<IView>> QtFramework::createViewAsync(const char* uniqueName, const char* resource,
                                                               ResourceType type, const Variant& context,
                                                               std::function<void(IView&)> loadedHandler)
{
	return createViewInternal(uniqueName, resource, type, context, loadedHandler, true);
}

//------------------------------------------------------------------------------
wg_future<std::unique_ptr<IView>> QtFramework::createViewInternal(const char* uniqueName, const char* resource,
                                                                  ResourceType type, const Variant& context,
                                                                  std::function<void(IView&)> loadedHandler, bool async)
{
	QUrl qUrl;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
	{
		std::unique_ptr<QFile> device(new QFile(resource));
		device->open(QFile::ReadOnly);
		TF_ASSERT(device != nullptr);
		auto view = new QtView(uniqueName ? uniqueName : resource, *device);
		device->close();

		std::promise<std::unique_ptr<IView>>* promise = new std::promise<std::unique_ptr<IView>>();
		wg_future<std::unique_ptr<IView>> wgFuture(
		promise->get_future(), []() { QApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 1); });
		promise->set_value(std::unique_ptr<IView>(view));
		delete promise;

		auto delayedLoadedHandler = [view, loadedHandler]() {
			loadedHandler(*view);
		};

		callLater(delayedLoadedHandler);

		return wgFuture;
	}
	break;

	case IUIFramework::ResourceType::Url:
	{
		qUrl = resolveQmlPath(resource);
		auto view = new QmlView(uniqueName ? uniqueName : resource, *qmlEngine());
		auto scriptObject = qtFrameworkBase_->scriptingEngine()->createScriptObject(context, view->widget());

		if (scriptObject)
		{
			view->setContextObject(scriptObject);
		}
		else
		{
			auto source = toQVariant(context, view->widget());
			view->setContextProperty(QString("source"), source);
		}
		std::promise<std::unique_ptr<IView>>* promise = new std::promise<std::unique_ptr<IView>>();
		wg_future<std::unique_ptr<IView>> wgFuture(
		promise->get_future(), []() { QApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 1); });
		view->load(qUrl,
		           [loadedHandler, view, promise]() {
			           loadedHandler(*view);
			           promise->set_value(std::unique_ptr<IView>(view));
			           delete promise;
			       },
		           [view, promise]() {
			           promise->set_value(std::unique_ptr<IView>());
			           view->deleteLater();
			       },
		           this->useAsyncViewLoading_ ? async : false);

		return wgFuture;
		break;
	}
	default:
		return std::future<std::unique_ptr<IView>>();
	}
}

QmlWindow* QtFramework::createQmlWindow()
{
	return new QmlWindow(*qmlEngine());
}

QtWindow* QtFramework::createQtWindow(QIODevice& source)
{
	return new QtWindow(source);
}

//------------------------------------------------------------------------------
void QtFramework::createWindowAsync(const char* resource, ResourceType type, const Variant& context,
                                    std::function<void(std::unique_ptr<IWindow>&)> loadedHandler)
{
	createWindowInternal(resource, type, context, loadedHandler, true);
}

//------------------------------------------------------------------------------
IProgressDialogPtr QtFramework::createProgressDialog(const std::string& title, const std::string& label,
                                                     const std::string& cancelText, uint32_t minimum, uint32_t maximum,
                                                     std::chrono::milliseconds duration)
{
	return IProgressDialogPtr(new QtProgressDialog(title, label, cancelText, minimum, maximum, duration, nullptr));
}

//------------------------------------------------------------------------------
IProgressDialog* QtFramework::createModelessProgressDialog(const std::string& title, const std::string& label,
                                                           const std::string& cancelText, uint32_t minimum,
                                                           uint32_t maximum, std::chrono::milliseconds duration,
                                                           ModelessProgressCallback callback)
{
	// Ensure there is a callback, otherwise no work can progress
	TF_ASSERT(callback);
	if (!callback)
		return nullptr;
	// The progress dialog will be destroyed once the dialog is completed
	return new QtProgressDialog(title, label, cancelText, minimum, maximum, duration, callback);
}

//------------------------------------------------------------------------------
IProgressDialogPtr QtFramework::createIndeterminateProgressDialog(const std::string& title, const std::string& label)
{
	return IProgressDialogPtr(new QtProgressDialog(title, label, "", 0, 0, std::chrono::milliseconds(0), nullptr));
}

//------------------------------------------------------------------------------
const std::vector<std::string> QtFramework::showOpenFileDialog(const std::string& caption, const std::string& directory,
                                                               const std::string& filter,
                                                               const QtFileDialogOptions& options)
{
	std::vector<std::string> selected;
	auto files = QFileDialog::getOpenFileNames(0, QString::fromStdString(caption), QString::fromStdString(directory),
	                                           QString::fromStdString(filter), 0, (QFileDialog::Options)options)
	             .toStdList();

	for (auto& i : files)
	{
		selected.emplace_back(i.toUtf8().constData());
	}

	return selected;
}

//------------------------------------------------------------------------------
const std::string QtFramework::showSaveAsFileDialog(const std::string& caption, const std::string& directory,
                                                    const std::string& filter, const QtFileDialogOptions& options)
{
	return QFileDialog::getSaveFileName(0, QString::fromStdString(caption), QString::fromStdString(directory),
	                                    QString::fromStdString(filter), 0, (QFileDialog::Options)options)
	.toUtf8()
	.constData();
}

//------------------------------------------------------------------------------
const std::string QtFramework::showSelectDirectoryDialog(const std::string& caption, const std::string& directory,
                                                         const QtFileDialogOptions& options)
{
	return QFileDialog::getExistingDirectory(0, QString::fromStdString(caption), QString::fromStdString(directory),
	                                         (QFileDialog::Options)options)
	.toUtf8()
	.constData();
}

//------------------------------------------------------------------------------
std::unique_ptr<IWindow> QtFramework::createWindow(const char* resource, ResourceType type, const Variant& context)
{
	NGT_DEBUG_MSG("Deprecated function call, please use async version instead\n");
	std::unique_ptr<IWindow> returnWindow;
	createWindowInternal(resource, type, context,
	                     [&returnWindow](std::unique_ptr<IWindow>& window) { returnWindow = std::move(window); },
	                     false);
	return returnWindow;
}

//------------------------------------------------------------------------------
void QtFramework::createWindowInternal(const char* resource, ResourceType type, const Variant& context,
                                       std::function<void(std::unique_ptr<IWindow>&)> loadedHandler, bool async)
{
	// TODO: This function assumes the resource is a ui file containing a QMainWindow
	switch (type)
	{
	case IUIFramework::ResourceType::File:
	{
		const auto path = resolveFilePath(resource);
		std::unique_ptr<QFile> device(new QFile(path));
		TF_ASSERT(device != nullptr);
		if (!device->open(QFile::ReadOnly))
		{
			NGT_WARNING_MSG("Could not open file %s", resource);
		}
		std::unique_ptr<IWindow> window(createQtWindow(*device));
		loadedHandler(window);
		device->close();
	}
	break;
	case IUIFramework::ResourceType::Url:
	{
		QUrl qUrl = resolveQmlPath(resource);
		auto qmlWindow = createQmlWindow();

		auto scriptObject = qtFrameworkBase_->scriptingEngine()->createScriptObject(context, qmlWindow->window());

		if (scriptObject)
		{
			qmlWindow->setContextObject(scriptObject);
		}
		else
		{
			auto source = toQVariant(context, qmlWindow->window());
			qmlWindow->setContextProperty(QString("source"), source);
		}

		qmlWindow->load(qUrl, async, [loadedHandler, qmlWindow]() {
			std::unique_ptr<IWindow> localWindow(qmlWindow);
			loadedHandler(localWindow);
		});
	}
	break;

	default:
		return;
	}
}

void QtFramework::loadActionData(const char* resource, ResourceType type)
{
	size_t bufferSize = 0;
	const char* buffer = nullptr;
	std::unique_ptr<QIODevice> device;
	QByteArray data;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
	{
		device.reset(new QFile(resource));
		device->open(QFile::ReadOnly);
		TF_ASSERT(device != nullptr);
		bufferSize = device->size();
		data = device->readAll();
		device->close();

		if (data.isEmpty())
		{
			NGT_WARNING_MSG("Read action data error from %s.\n", resource);
			return;
		}

		buffer = data.constData();
	}
	break;

	case IUIFramework::ResourceType::Buffer:
		bufferSize = strlen(resource);
		buffer = resource;
		break;

	default:
		return;
	}

	FixedMemoryStream dataStream(buffer, bufferSize);
	impl_->actionManager_->loadActionData(dataStream);
}

void QtFramework::registerComponent(const char* id, const char* version, IComponent& component, bool supportsAsync)
{
	qtFrameworkBase_->qmlComponentManager()->registerComponent(id, version, component, supportsAsync );
}

void QtFramework::registerComponentProvider(IComponentProvider& provider)
{
	qtFrameworkBase_->qmlComponentManager()->registerComponentProvider(provider);
}

IComponent* QtFramework::findComponent(const TypeId& typeId, std::function<bool(const ItemRole::Id&)>& predicate,
                                       const char* version) const
{
	return qtFrameworkBase_->qmlComponentManager()->findComponent(typeId, predicate, version);
}

IComponent* QtFramework::findComponent(const char* componentId, const char* version) const
{
	return qtFrameworkBase_->qmlComponentManager()->findComponent(componentId, version);
}

void QtFramework::registerDialog(const char* id, const char* version, std::shared_ptr<IDialog> dialog)
{
	auto& dialogs = dialogs_[id];
	auto tokens = ComponentVersion::tokenise(version);
	auto itr = dialogs.find(tokens);
	TF_ASSERT(itr == dialogs.end());
	dialogs[tokens] = dialog;
}

std::shared_ptr<IDialog> QtFramework::findDialog(const char* id, const char* version)
{
	auto tokens = ComponentVersion::tokenise(version);
	auto dialogsItr = dialogs_.find(id);

	if (dialogsItr == dialogs_.end())
	{
		return nullptr;
	}

	auto& dialogs = dialogsItr->second;

	if (dialogs.empty())
	{
		return nullptr;
	}

	auto dialogItr = dialogs.upper_bound(tokens);
	--dialogItr;

	return dialogItr->second.lock();
}

void QtFramework::registerModelExtension(const char* id, const char* version, std::function<ManagedObjectPtr()> creator)
{
	TF_ASSERT(qtFrameworkBase_.get());
	auto extensionManager = qtFrameworkBase_->modelExtensionManager();
	extensionManager->registerExtension(id, version, creator);
}

void QtFramework::setPluginPath(const std::string& path)
{
	pluginPath_ = path;
}

const std::string& QtFramework::getPluginPath() const
{
	return pluginPath_;
}

namespace
{
	QMessageBox::Icon getQtMessageBoxIcon(IUIFramework::MessageBoxIcon icon)
	{
		switch (icon)
		{
		case IUIFramework::MessageBoxIcon::Information:
			return QMessageBox::Icon::Information;
		case IUIFramework::MessageBoxIcon::Question:
			return QMessageBox::Icon::Question;
		case IUIFramework::MessageBoxIcon::Warning:
			return QMessageBox::Icon::Warning;
		case IUIFramework::MessageBoxIcon::Error:
			return QMessageBox::Icon::Critical;
		default:
			return QMessageBox::Icon::NoIcon;
		};
	}

	struct MessageBoxQtMapping
	{
		IUIFramework::MessageBoxButtons uiButton;
		QMessageBox::StandardButton qtButton;
	};

	MessageBoxQtMapping buttonMappings[] = {
		{ IUIFramework::Ok,			QMessageBox::StandardButton::Ok },
		{ IUIFramework::Cancel,		QMessageBox::StandardButton::Cancel },
		{ IUIFramework::Save,		QMessageBox::StandardButton::Save },
		{ IUIFramework::SaveAll,	QMessageBox::StandardButton::SaveAll },
		{ IUIFramework::Yes,		QMessageBox::StandardButton::Yes },
		{ IUIFramework::No,			QMessageBox::StandardButton::No },
		{ IUIFramework::Discard,	QMessageBox::StandardButton::Discard },
	};

	struct MappedQtButtons
	{
		QMessageBox::StandardButton qtButtons = QMessageBox::StandardButton::NoButton;
		int count = 0;	// The count of mapped buttons (not including NoButton)
	};

	MappedQtButtons getQtMessageBoxButtons(unsigned int buttonsBitfield)
	{
		MappedQtButtons out;
		int outButtons = 0;
		const size_t count = sizeof(buttonMappings) / sizeof(buttonMappings[0]);
		for (size_t i = 0; i < count; ++i)
		{
			if (buttonsBitfield & buttonMappings[i].uiButton)
			{
				outButtons |= buttonMappings[i].qtButton;
				++out.count;
			}
		}
		out.qtButtons = static_cast<QMessageBox::StandardButton>(outButtons);
		return out;
	}

	IUIFramework::MessageBoxButtons getSingleIUIFrameworkButton(int clickedQtButton)
	{
		const size_t count = sizeof(buttonMappings) / sizeof(buttonMappings[0]);
		for (size_t i = 0; i < count; ++i)
		{
			if (clickedQtButton == buttonMappings[i].qtButton)
			{
				return buttonMappings[i].uiButton;
			}
		}

		TF_ASSERT(false && "Could not find valid result");
		return IUIFramework::MessageBoxButtons::Cancel;
	}


} // end anonymous namespace

void QtFramework::redirectMessageBoxToLog(bool redirect)
{
	redirectMessageBoxToLog_ = redirect;
}

bool QtFramework::shouldRedirectMessageBoxToLog() const
{
	return redirectMessageBoxToLog_;
}

IUIFramework::MessageBoxButtons QtFramework::displayMessageBox(const char* title, 
															   const char* message,
															   unsigned int buttons, 
															   MessageBoxIcon icon,
															   const char* detail,
															   const char* customIcon,
															   unsigned int defaultButton)
{
	if(redirectMessageBoxToLog_)
	{
		if(auto logger = impl_->get<ILoggingSystem>())
		{
			const LogLevel level = icon == MessageBoxIcon::Error ? LogLevel::LOG_ERROR : 
				(icon == IUIFramework::MessageBoxIcon::Warning ? LogLevel::LOG_WARNING : LogLevel::LOG_INFO);
			logger->log(level, "%s: %s %s", title, message, detail ? detail : "");
		}
		return IUIFramework::MessageBoxButtons::Cancel;
	}
	else
	{
		const MappedQtButtons desiredButtons = getQtMessageBoxButtons(buttons);
		TF_ASSERT(desiredButtons.count > 0);

		const QMessageBox::Icon qtIcon = getQtMessageBoxIcon(icon);
	
		QMessageBox messageBox(qtIcon, title, message, desiredButtons.qtButtons);

		if (customIcon)
		{
			QPixmap customPixmap(customIcon);
			messageBox.setIconPixmap(customPixmap);
		}

		if (detail)
		{
			messageBox.setDetailedText(detail);
		}

		if (defaultButton != MessageBoxButtons::NoButton)
		{
			// Default button should be contained in provided buttons bitfield.
			TF_ASSERT(defaultButton & buttons);
			const MappedQtButtons defaultQtButton = getQtMessageBoxButtons(defaultButton);
			TF_ASSERT(defaultQtButton.count == 1);
			messageBox.setDefaultButton(defaultQtButton.qtButtons);
		}

		int retValue = 0;

		if (auto focusedWidget = QApplication::focusWidget())
		{
			bool focusWidgetBlocked = focusedWidget->signalsBlocked();
			focusedWidget->blockSignals(true);
			retValue = messageBox.exec();
			focusedWidget->blockSignals(focusWidgetBlocked);
		}
		else
		{
			retValue = messageBox.exec();
		}

		return getSingleIUIFrameworkButton(retValue);
	}
}

void QtFramework::findReloadableFiles(const QString& path, const QStringList& filter, QStringList& files)
{
	QDir directory(path);
	if (!directory.exists())
	{
		return;
	}

	const auto folders = directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	for (const auto& folder : folders)
	{
		const QString folderPath(path + FilePath::kNativeDirectorySeparator + folder);
		findReloadableFiles(folderPath, filter, files);
	}

	const auto filteredFiles = directory.entryList(filter);
	for (auto file : filteredFiles)
	{
		files.push_back(QString(path + FilePath::kNativeDirectorySeparator + file));
	}
}

void QtFramework::registerReloadableFiles()
{
	auto clearCache = [this]() { qmlEngine()->clearComponentCache(); };
	QObject::connect(qmlWatcher_.get(), &QFileSystemWatcher::fileChanged, clearCache);

	QStringList paths = qmlEngine()->importPathList();
	paths.removeAll(QDir::currentPath());
	paths.removeAll("qrc:/");

	QStringList filter;
	filter.push_back("*.qml");
	filter.push_back("*.js");

	QStringList files;
	for (const QString& path : paths)
	{
		findReloadableFiles(path, filter, files);
	}

	if (!files.isEmpty())
	{
		qmlWatcher_->addPaths(files);
	}
}

void QtFramework::registerDefaultDialogs()
{
	std::map<std::pair<std::string, std::string>, ObjectHandleT<DialogModel>> models;

	QDir resources(":/WGControls/Private/");
	std::string suffix = "_dialog";
	std::string folder("WGControls/Private/");
	QStringList filter("*_dialog*.qml");

	for (auto& resource : resources.entryList(filter))
	{
		auto path = folder + resource.toUtf8().constData();
		auto type = path.substr(0, path.rfind("_"));
		auto version = path.substr(path.rfind(suffix) + suffix.size());
		version = version.substr(0, version.size() - 4);

		if (version.size() > 1)
		{
			version.insert(version.size() - 1, ".");
		}

		auto modelItr = models.find(std::make_pair(type, version));
		ObjectHandleT<DialogModel> model = modelItr == models.end() ? nullptr : modelItr->second;
		std::shared_ptr<IDialog> dialog(createDialog(path.c_str(), model));

		if (dialog)
		{
			defaultDialogs_.emplace_back(dialog);
			registerDialog(type.c_str(), version.c_str(), dialog);
		}
	}
}

void QtFramework::unregisterResources()
{
	for (auto res : registeredResources_)
	{
		qUnregisterResourceData(0x01, std::get<0>(res), std::get<1>(res), std::get<2>(res));
	}
	registeredResources_.clear();
}

//------------------------------------------------------------------------------
IPreferences* QtFramework::getPreferences()
{
	return context_.queryInterface<IPreferences>();
}

//------------------------------------------------------------------------------
void QtFramework::doOnUIThread(std::function<void()> action)
{
	QMetaObject::invokeMethod(worker_.get(), "doJob", Qt::QueuedConnection,
	                          Q_ARG(QtUIFunctionWrapper*, new QtUIFunctionWrapper(action)));
}

//------------------------------------------------------------------------------
void QtFramework::processEvents()
{
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void QtFramework::showShortcutConfig() const
{
	if (shortcutDialog_ == nullptr)
	{
		return;
	}
	shortcutDialog_->showModal();
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::createDialog(const char* resource, ManagedObjectPtr model)
{
	if (resource != nullptr)
	{
		model->getHandleT<DialogModel>()->setURL(resource);
	}
	std::shared_ptr<IDialog> dialog(new QmlDialog(*qmlEngine()), [](IDialog* dialog) {static_cast<QmlDialog*>(dialog)->deleteLater();});
	dialog->setModel(std::move(model));
	return createDialogInternal(dialog);
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::createDialog(const char* resource, const char* title)
{
	auto model = ManagedObject<DialogModel>::make_unique();
	if(title != nullptr)
	{
		model->getHandleT()->setTitle(title);
	}
	return createDialog(resource, std::move(model));
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::createDialog(const char* resource, ObjectHandleT<DialogModel> model)
{
	if(resource != nullptr)
	{
		model->setURL(resource);
	}
	std::shared_ptr<IDialog> dialog(new QmlDialog(*qmlEngine()), [](IDialog* dialog) {static_cast<QmlDialog*>(dialog)->deleteLater();});
	dialog->setModel(model);
	return createDialogInternal(dialog);
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::createDialogInternal(std::shared_ptr<IDialog> dialog)
{
	{
		auto predicate = [](const ModelessDialogData& dialog) -> bool { return std::get<CLOSED>(dialog); };

		std::lock_guard<std::mutex> lock(modelessDialogMutex_);
		auto eraseFrom = std::remove_if(modelessDialogs_.begin(), modelessDialogs_.end(), predicate);
		modelessDialogs_.erase(eraseFrom, modelessDialogs_.end());
	}
	dialog->load(dialog->model()->getUrl());
	return dialog;
}

//------------------------------------------------------------------------------
void QtFramework::iterateDialogs(std::function<void(const IDialog& dialog)> fn)
{
	std::lock_guard<std::mutex> lock(modelessDialogMutex_);
	for(const auto& dialogData : modelessDialogs_)
	{
		if(auto dialog = std::get<DIALOG>(dialogData))
		{
			if(dialog->isOpen())
			{
				fn(*dialog);
			}
		}
	}
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::showDialog(const char* resource, const IDialog::Mode mode,
                                                 const IDialog::ClosedCallback& callback,
                                                 const char* title)
{
	return showDialogInternal(createDialog(resource, title), mode, callback);
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::showDialog(ObjectHandleT<DialogModel> model, const IDialog::Mode mode,
                                                 const IDialog::ClosedCallback& callback)
{
	return showDialogInternal(createDialog(nullptr, model), mode, callback);
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::showDialog(ManagedObjectPtr model, const IDialog::Mode mode,
                                                 const IDialog::ClosedCallback& callback)
{
	return showDialogInternal(createDialog(nullptr, std::move(model)), mode, callback);
}

//------------------------------------------------------------------------------
std::shared_ptr<IDialog> QtFramework::showDialogInternal(std::shared_ptr<IDialog> dialog, const IDialog::Mode mode,
													     const IDialog::ClosedCallback& callback)
{
	if (callback)
	{
		dialog->connectClosedCallback(callback);
	}

	dialog->show(mode);

	if (mode == IDialog::Mode::MODELESS)
	{
		auto markDialogClosedCallback = [this](IDialog& dialog) {
			auto dialogPointer = &dialog;
			auto predicate = [dialogPointer](const ModelessDialogData& dialog) -> bool {
				return std::get<DIALOG>(dialog).get() == dialogPointer;
			};

			std::lock_guard<std::mutex> lock(modelessDialogMutex_);
			auto itr = std::find_if(modelessDialogs_.begin(), modelessDialogs_.end(), predicate);

			if (itr != modelessDialogs_.end())
			{
				std::get<CLOSED>(*itr) = true;
			}
		};

		auto connection = dialog->connectClosedCallback(markDialogClosedCallback);

		std::lock_guard<std::mutex> lock(modelessDialogMutex_);
		modelessDialogs_.emplace_back(std::make_tuple(dialog, false, connection));
	}

	return dialog;
}

//------------------------------------------------------------------------------
void QtFramework::onApplicationStartUp()
{
	auto context = impl_->actionManager_->getContextObject();
	createWindowInternal("private/shortcut_dialog.qml", IUIFramework::ResourceType::Url, context,
	                     [this](std::unique_ptr<IWindow>& window) {
		                     shortcutDialog_ = std::move(window);
		                     if (shortcutDialog_ != nullptr)
		                     {
			                     shortcutDialog_->hide();
			                     impl_->actionManager_->registerEventHandler(shortcutDialog_.get());
		                     }
		                 },
	                     false);
}

//------------------------------------------------------------------------------
void QtFramework::onApplicationExit()
{
	// Dialogs need to be closed before the qt application is closed
	std::lock_guard<std::mutex> lock(modelessDialogMutex_);
	for (auto& dialogData : modelessDialogs_)
	{
		if (!std::get<CLOSED>(dialogData))
		{
			std::get<CONNECTION>(dialogData).disconnect();
			std::get<DIALOG>(dialogData)->close(IDialog::INVALID_RESULT);
		}
	}
	modelessDialogs_.clear();
}

//------------------------------------------------------------------------------
void QtFramework::callLater(std::function<void(void)> function) const
{
	class FunctionCallEvent : public QEvent {
	public:
		FunctionCallEvent(std::function<void(void)>&& fun)
			: QEvent(QEvent::None)
			, function_(std::move(fun))
		{
		}

		~FunctionCallEvent() {
			function_();
		}

	private:
		std::function<void(void)> function_;
	};

	QCoreApplication::postEvent(qApp, new FunctionCallEvent(std::move(function)));
}

void QtFramework::openInGraphicalShell(const char* filePath)
{
	std::string pathIn = filePath;
// Mac, Windows support folder or file.
#if defined(Q_OS_WIN)
	const QString explorer = QtFramework_Locals::explorerPath().c_str();
	if (explorer.isEmpty())
	{
		NGT_ERROR_MSG("Cannot find explorer.exe.\n");
		return;
	}
	QString param;
	if (!QFileInfo(pathIn.c_str()).isDir())
		param = QLatin1String("/select,");
	param += QDir::toNativeSeparators(pathIn.c_str());
	QString command = explorer + " " + param;
	QProcess::startDetached(command);
#elif defined(Q_OS_MAC)
	QStringList scriptArgs;
	scriptArgs << QLatin1String("-e")
	           << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(pathIn.c_str());
	QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
	scriptArgs.clear();
	scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
	QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
	TF_ASSERT(false);
#endif
}

void QtFramework::copyTextToClipboard(const char* text)
{
	QClipboard* clipboard = QApplication::clipboard();
	if (clipboard)
	{
		clipboard->setText(text);
	}
}

void QtFramework::openInDefaultApp(const char* filePath)
{
	QFileInfo fi(filePath);
	if (!fi.exists())
	{
		NGT_ERROR_MSG("file doesn't exist.\n");
		return;
	}
	QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
}

void QtFramework::loadIconData(const char* resource, ResourceType type)
{
	TF_ASSERT(resource != nullptr);
	if (resource == nullptr)
	{
		return;
	}
	size_t bufferSize = 0;
	const char* buffer = nullptr;
	std::unique_ptr<QIODevice> device;
	QByteArray data;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
	{
		device.reset(new QFile(resource));
		device->open(QFile::ReadOnly);
		TF_ASSERT(device != nullptr);
		bufferSize = device->size();
		data = device->readAll();
		device->close();

		if (data.isEmpty())
		{
			NGT_WARNING_MSG("Read icon data error from %s.\n", resource);
			return;
		}

		buffer = data.constData();
	}
	break;

	case IUIFramework::ResourceType::Buffer:
		bufferSize = strlen(resource);
		buffer = resource;
		break;

	default:
		return;
	}
	auto imageProvider = dynamic_cast<QtImageProvider*>(qtFrameworkBase_->qmlEngine()->imageProvider(QtImageProvider::providerId()));
	TF_ASSERT(imageProvider != nullptr);
	FixedMemoryStream dataStream(buffer, bufferSize);
	TextStream stream(dataStream);
	QtFramework_Locals::IconsReader icons(stream, impl_->iconData_, *imageProvider);
	if (!icons.read())
	{
		NGT_ERROR_MSG("Failed to parse actions\n");
		return;
	}
}

const char* QtFramework::getIconUrlFromImageProvider(const char* key) const
{
	std::string strKey = key;
	std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
	auto& found = impl_->iconData_.find(strKey);
	if (found == impl_->iconData_.end())
	{
		return "";
	}
	return found->second->icon_.c_str();
}

Vector4 QtFramework::getPaletteColor(Palette::Color color) const
{
	const auto& c = palette()->getColor(color);
	return Vector4(c.redF(), c.greenF(), c.blueF(), c.alphaF());
}

Palette::Theme QtFramework::getPaletteTheme() const
{
	return palette()->getTheme();
}

void QtFramework::setPaletteTheme(Palette::Theme theme)
{
	palette()->setTheme(theme);
}

Connection QtFramework::connectPaletteThemeChanged(PaletteThemeChangedCallback cb)
{
	return palette()->connectPaletteThemeChanged(cb);
}

void QtFramework::registerThumbnailProvider(std::shared_ptr<IThumbnailProvider> thumbnailProvider)
{
	if (impl_->thumbnailProviders_.find(thumbnailProvider) != impl_->thumbnailProviders_.end())
	{
		return;
	}
	impl_->thumbnailProviders_.insert(thumbnailProvider);
}

bool QtFramework::hasThumbnail(const char* filePath) const
{
	for (auto&& it : impl_->thumbnailProviders_)
	{
		if (it.expired())
		{
			continue;
		}
		auto provider = it.lock();
		TF_ASSERT(provider != nullptr);
		if (provider->getThumbnailData(filePath))
		{
			return true;
		}
	}

	auto extension = FilePath::getExtension(filePath);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	static std::set<std::string> extensions = { "bmp", "gif", "jpg", "jpeg", "png", "pbm", "pgm", "ppm", "xbm", "xpm"};
	if (extensions.find(extension) == extensions.end())
	{
		return false;
	}
	return QFile::exists(filePath);
}

void QtFramework::makeFakeMouseRelease()
{
	if (qtFrameworkBase_ != nullptr && qtFrameworkBase_->scriptingEngine() != nullptr)
	{
		qtFrameworkBase_->scriptingEngine()->makeFakeMouseRelease();
	}
}

} // end namespace wgt
