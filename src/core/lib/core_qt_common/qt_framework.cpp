#include "qt_framework.hpp"
#include "qt_preferences.hpp"
#include "core_data_model/i_item_role.hpp"

#include "core_qt_common/qt_types.hpp"
#include "core_qt_common/i_qt_type_converter.hpp"
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
#include "core_qt_common/string_qt_type_converter.hpp"
#include "core_qt_common/vector_qt_type_converter.hpp"
#include "core_qt_common/qt_image_provider.hpp"
#include "core_qt_common/qt_image_provider_old.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_qt_common/qt_scripting_engine.hpp"
#include "core_qt_common/qt_script_object.hpp"
#include "core_common/platform_env.hpp"
#include "core_qt_common/qt_progress_dialog.hpp"

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
#include "qt_action_manager.hpp"

#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/dialog/dialog_model.hpp"

#include "wg_types/string_ref.hpp"
#include "core_common/ngt_windows.hpp"

#include "private/qt_ui_worker.hpp"

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
#include <QDir>
#include <QMessageBox>
#include <QQmlIncubationController>
#include <QFileSystemWatcher>

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
}

// Temporary Dialog accessor
enum TemporaryDialogType
{
	DIALOG,
	CLOSED,
	CONNECTION
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
std::vector<int> QtFramework::ComponentVersion::tokenise(const char* version)
{
	static const size_t bufferSize = 256;

	std::vector<int> tokens;

	auto len = strlen(version);
	if (len >= bufferSize)
	{
		return tokens;
	}

	char buffer[bufferSize];
	strcpy(buffer, version);
	auto tok = strtok(buffer, ".");
	while (tok != nullptr)
	{
		tokens.push_back(strtol(tok, nullptr, 0));
		tok = strtok(nullptr, ".");
	}
	return tokens;
}

bool QtFramework::ComponentVersion::operator()(const std::vector<int>& a, const std::vector<int>& b) const
{
	auto count = std::min(a.size(), b.size());
	for (size_t i = 0; i < count; ++i)
	{
		if (a[i] == b[i])
		{
			continue;
		}

		return a[i] < b[i];
	}
	return a.size() < b.size();
}

QtFramework::QtFramework(IComponentContext& contextManager)
    : Depends(contextManager), qmlEngine_(new QQmlEngine()), qmlWatcher_(new QFileSystemWatcher()),
      scriptingEngine_(new QtScriptingEngine()), palette_(new QtPalette()), defaultQmlSpacing_(new QtDefaultSpacing()),
      globalQmlSettings_(new QtGlobalSettings()), actionManager_(new QtActionManager(contextManager)),
      context_(contextManager), worker_(new QtUIWorker()), incubationTime_(50),
      incubationController_(new QQmlIncubationController), preferences_(nullptr), useAsyncViewLoading_(true)
{
	char wgtHome[MAX_PATH];
	if (Environment::getValue<MAX_PATH>("WGT_HOME", wgtHome))
	{
		qmlEngine_->addPluginPath(wgtHome);
		qmlEngine_->addImportPath(wgtHome);
	}

	// Search Qt resource path or Url by default
	qmlEngine_->addImportPath("qrc:/");
	qmlEngine_->addImportPath(":/");
	qmlEngine_->setIncubationController(incubationController_.get());

	qRegisterMetaType<QtUIFunctionWrapper*>("QtUIFunctionWrapper*");
	qRegisterMetaType<IDialog::Result>("IDialog::Result");
}

QtFramework::~QtFramework()
{
}

void QtFramework::initialise(IComponentContext& contextManager)
{
	// This needs to be set after qtFramework has been constructed and QmlEngine has been created.
	// This will only occur when running from a plugin scenario such as Maya.
	IPluginContextManager* pPluginContextManager = contextManager.queryInterface<IPluginContextManager>();
	if (pPluginContextManager && pPluginContextManager->getExecutablePath())
	{
		qmlEngine_->addPluginPath(pPluginContextManager->getExecutablePath());
		qmlEngine_->addImportPath(pPluginContextManager->getExecutablePath());
	}

	SharedControls::init();

	registerReloadableFiles();
	registerDefaultComponents();
	registerDefaultComponentProviders();
	registerDefaultTypeConverters();

	scriptingEngine_->initialise(*this, contextManager);

	auto rootContext = qmlEngine_->rootContext();
	rootContext->setContextObject(scriptingEngine_.get());
	rootContext->setContextProperty("palette", palette_.get());
	rootContext->setContextProperty("defaultSpacing", defaultQmlSpacing_.get());
	rootContext->setContextProperty("globalSettings", globalQmlSettings_.get());

	Variant obj = &contextManager;
	rootContext->setContextProperty("componentContext", QtHelpers::toQVariant(obj, rootContext));

	qmlEngine_->addImageProvider(QtImageProvider::providerId(), new QtImageProvider());
	qmlEngine_->addImageProvider(QtImageProviderOld::providerId(), new QtImageProviderOld());

	auto commandManager = get<ICommandManager>();
	if (commandManager != nullptr)
	{
		commandEventListener_.reset(new QtFramework_Locals::QtCommandEventListener);
		commandManager->registerCommandStatusListener(commandEventListener_.get());
	}

	preferences_ = contextManager.registerInterface(new QtPreferences(contextManager));

	auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
	actionManager_->init();
	SharedControls::initDefs(*definitionManager);
	auto uiApplication = get<IUIApplication>();
	assert(uiApplication != nullptr);
	connections_ += uiApplication->signalStartUp.connect(std::bind(&QtFramework::onApplicationStartUp, this));
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

void QtFramework::registerQmlType(ObjectHandle type)
{
	QQmlPrivate::RegisterType* qtype = type.getBase<QQmlPrivate::RegisterType>();
	assert(qtype != nullptr);

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
	auto commandManager = get<ICommandManager>();
	if (commandManager != nullptr)
	{
		commandManager->deregisterCommandStatusListener(commandEventListener_.get());
	}
	if (shortcutDialog_ != nullptr)
	{
		auto uiApplication = get<IUIApplication>();
		assert(uiApplication != nullptr);
		uiApplication->removeWindow(*shortcutDialog_);
	}

	{
		std::lock_guard<std::mutex> lock(temporaryDialogMutex_);

		for (auto& tempDialog : temporaryDialogs_)
		{
			if (!std::get<CLOSED>(tempDialog))
			{
				std::get<CONNECTION>(tempDialog).disconnect();
				std::get<DIALOG>(tempDialog)->close(IDialog::INVALID_RESULT);
			}
		}

		temporaryDialogs_.clear();
	}

	unregisterResources();
	actionManager_->fini();
	shortcutDialog_ = nullptr;
	qmlEngine_->removeImageProvider(QtImageProviderOld::providerId());
	qmlEngine_->removeImageProvider(QtImageProvider::providerId());
	scriptingEngine_->finalise();
	qmlEngine_->setIncubationController(nullptr);
	incubationController_ = nullptr;
	globalQmlSettings_ = nullptr;
	defaultQmlSpacing_ = nullptr;
	palette_ = nullptr;
	qmlEngine_.reset();
	scriptingEngine_ = nullptr;

	context_.deregisterInterface(preferences_);
	preferences_ = nullptr;

	defaultTypeConverters_.clear();
	defaultComponentProviders_.clear();
	defaultComponents_.clear();
	connections_.clear();
}

QQmlEngine* QtFramework::qmlEngine() const
{
	return qmlEngine_.get();
}

QFileSystemWatcher* QtFramework::qmlWatcher() const
{
	return qmlWatcher_.get();
}

void QtFramework::setIncubationTime(int msecs)
{
	incubationTime_ = msecs;
}

void QtFramework::incubate()
{
	if (qmlEngine_ == nullptr)
	{
		return;
	}
	auto incubationCtrl = qmlEngine_->incubationController();
	if (incubationCtrl != nullptr)
	{
		if (incubationCtrl->incubatingObjectCount() > 0)
		{
			incubationCtrl->incubateFor(incubationTime_);
		}
		else
		{
			auto pAutomation = this->get<AutomationInterface>();
			if (pAutomation)
			{
				pAutomation->notifyLoadingDone();
			}
		}
	}
}

const QtPalette* QtFramework::palette() const
{
	return palette_.get();
}

void QtFramework::addImportPath(const QString& path)
{
	QDir importPath(path);
	if (importPath.exists() && importPath.isReadable())
	{
		qmlEngine_->addImportPath(path);
	}
}

QtGlobalSettings* QtFramework::qtGlobalSettings() const
{
	return globalQmlSettings_.get();
}

void QtFramework::registerTypeConverter(IQtTypeConverter& converter) /* override */
{
	typeConverters_.registerTypeConverter(converter);
}

void QtFramework::deregisterTypeConverter(IQtTypeConverter& converter) /* override */
{
	typeConverters_.deregisterTypeConverter(converter);
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
	QVariant qVariant(QVariant::Invalid);
	typeConverters_.toScriptType(variant, qVariant, parent);
	return qVariant;
}

Variant QtFramework::toVariant(const QVariant& qVariant) const
{
	Variant variant;
	typeConverters_.toVariant(qVariant, variant);
	return variant;
}

QQmlComponent* QtFramework::toQmlComponent(IComponent& component)
{
	// TODO replace this with a proper UI adapter interface
	auto qmlComponent = dynamic_cast<QmlComponent*>(&component);
	if (qmlComponent != nullptr)
	{
		return qmlComponent->component();
	}

	return nullptr;
}

QWidget* QtFramework::toQWidget(IView& view)
{
	// TODO replace this with a proper UI adapter interface
	auto qmlView = dynamic_cast<IQtView*>(&view);
	if (qmlView != nullptr)
	{
		auto widget = qmlView->releaseView();
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
		qmlView->retainView();
	}
}

std::unique_ptr<IAction> QtFramework::createAction(const char* id, std::function<void(IAction*)> func,
                                                   std::function<bool(const IAction*)> enableFunc,
                                                   std::function<bool(const IAction*)> checkedFunc)
{
	return actionManager_->createAction(id, func, enableFunc, checkedFunc);
}

std::unique_ptr<IAction> QtFramework::createAction(const char* id, const char* text, const char* path,
                                                   std::function<void(IAction*)> func,
                                                   std::function<bool(const IAction*)> enableFunc,
                                                   std::function<bool(const IAction*)> checkedFunc, int actionOrder)
{
	return actionManager_->createAction(id, text, path, actionOrder, func, enableFunc, checkedFunc);
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
		url = QtHelpers::resolveQmlPath(*qmlEngine_, resource);
		break;
	}

	auto qmlComponent = createComponent(url);
	if (type == IUIFramework::ResourceType::Buffer)
	{
		qmlComponent->component()->setData(resource, QUrl());
	}
	return std::unique_ptr<IComponent>(qmlComponent);
}

QmlComponent* QtFramework::createComponent(const QUrl& resource)
{
	auto qmlComponent = new QmlComponent(*qmlEngine_);
	if (!resource.isEmpty())
	{
		QmlComponentLoaderHelper helper(qmlComponent->component(), resource);
		helper.load(true);
	}
	return qmlComponent;
}

//------------------------------------------------------------------------------
void QtFramework::enableAsynchronousViewCreation(bool enabled)
{
	this->useAsyncViewLoading_ = enabled;
}

//------------------------------------------------------------------------------
std::unique_ptr<IView> QtFramework::createView(const char* resource, ResourceType type, const Variant& context)
{
	NGT_WARNING_MSG("Deprecated function call, please use async version instead");
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
	NGT_WARNING_MSG("Deprecated function call, please use async version instead");
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
		assert(device != nullptr);
		auto view = new QtView(uniqueName ? uniqueName : resource, *this, *device);
		device->close();
		// 			std::unique_ptr< IView > localView( view );
		std::promise<std::unique_ptr<IView>>* promise = new std::promise<std::unique_ptr<IView>>();
		wg_future<std::unique_ptr<IView>> wgFuture(
		promise->get_future(), []() { QApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 1); });
		loadedHandler(*view);
		promise->set_value(std::unique_ptr<IView>(view));
		delete promise;

		return wgFuture;
	}
	break;

	case IUIFramework::ResourceType::Url:
	{
		qUrl = QtHelpers::resolveQmlPath(*qmlEngine_, resource);
		auto view = new QmlView(uniqueName ? uniqueName : resource, *this, *qmlEngine_);
		auto scriptObject = scriptingEngine_->createScriptObject(context, view->view());

		if (scriptObject)
		{
			view->setContextObject(scriptObject);
		}
		else
		{
			auto source = toQVariant(context, view->view());
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
	return new QmlWindow(context_, *qmlEngine());
}

QtWindow* QtFramework::createQtWindow(QIODevice& source)
{
	return new QtWindow(*this, source);
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
	assert(callback);
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
	NGT_WARNING_MSG("Deprecated function call, please use async version instead\n");
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
		const auto path = QtHelpers::resolveFilePath(*qmlEngine_, resource);
		std::unique_ptr<QFile> device(new QFile(path));
		assert(device != nullptr);
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
		QUrl qUrl = QtHelpers::resolveQmlPath(*qmlEngine_, resource);
		auto qmlWindow = createQmlWindow();

		auto scriptObject = scriptingEngine_->createScriptObject(context, qmlWindow->window());

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
	std::unique_ptr<QIODevice> device;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
	{
		device.reset(new QFile(resource));
		device->open(QFile::ReadOnly);
	}
	break;

	default:
		return;
	}

	assert(device != nullptr);
	auto size = device->size();
	auto data = device->readAll();
	device->close();
	if (data.isEmpty())
	{
		NGT_WARNING_MSG("Read action data error from %s.\n", resource);
		return;
	}
	auto buffer = data.constData();
	FixedMemoryStream dataStream(buffer, size);
	actionManager_->loadActionData(dataStream);
}

void QtFramework::registerComponent(const char* id, const char* version, IComponent& component)
{
	auto& components = components_[id];

	auto tokens = ComponentVersion::tokenise(version);
	auto it = components.find(tokens);
	if (it != components.end())
	{
		return;
	}

	components[tokens] = &component;
}

void QtFramework::registerComponentProvider(IComponentProvider& provider)
{
	componentProviders_.push_back(&provider);
}

IComponent* QtFramework::findComponent(const TypeId& typeId, std::function<bool(const ItemRole::Id&)>& predicate,
                                       const char* version) const
{
	auto tokens = ComponentVersion::tokenise(version);

	for (auto componentProviderIt = componentProviders_.rbegin(); componentProviderIt != componentProviders_.rend();
	     ++componentProviderIt)
	{
		auto id = (*componentProviderIt)->componentId(typeId, predicate);
		if (id == nullptr)
		{
			continue;
		}

		auto componentsIt = components_.find(id);
		if (componentsIt == components_.end())
		{
			continue;
		}

		auto& components = componentsIt->second;

		if (components.empty())
		{
			continue;
		}

		auto componentIt = components.upper_bound(tokens);
		--componentIt;

		return componentIt->second;
	}
	return nullptr;
}

void QtFramework::registerDialog(const char* id, const char* version, std::shared_ptr<IDialog> dialog)
{
	auto& dialogs = dialogs_[id];
	auto tokens = ComponentVersion::tokenise(version);
	auto itr = dialogs.find(tokens);
	assert(itr == dialogs.end());
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

void QtFramework::setPluginPath(const std::string& path)
{
	pluginPath_ = path;
}

const std::string& QtFramework::getPluginPath() const
{
	return pluginPath_;
}

int QtFramework::displayMessageBox(const char* title, const char* message, int buttons)
{
	struct MessageBoxQtMapping
	{
		MessageBoxButtons uiButton;
		QMessageBox::StandardButton qtButton;
	};

	MessageBoxQtMapping buttonMappings[] = {
		{ Ok, QMessageBox::StandardButton::Ok },     { Cancel, QMessageBox::StandardButton::Cancel },
		{ Save, QMessageBox::StandardButton::Save }, { SaveAll, QMessageBox::StandardButton::SaveAll },
		{ Yes, QMessageBox::StandardButton::Yes },   { No, QMessageBox::StandardButton::No },
	};

	size_t count = sizeof(buttonMappings) / sizeof(buttonMappings[0]);

	int desiredButtons = 0;

	for (size_t i = 0; i < count; ++i)
	{
		if (buttons & buttonMappings[i].uiButton)
		{
			desiredButtons |= buttonMappings[i].qtButton;
		}
	}

	assert(desiredButtons != 0);

	QMessageBox messageBox(QMessageBox::Icon::NoIcon, title, message, (QMessageBox::StandardButton)desiredButtons);

	int retValue = messageBox.exec();

	int result = 0;

	for (size_t i = 0; i < count; ++i)
	{
		if (retValue == buttonMappings[i].qtButton)
		{
			result = buttonMappings[i].uiButton;
			break;
		}
	}

	assert(result != 0);

	return result;
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
	auto clearCache = [this]() { qmlEngine_->clearComponentCache(); };
	QObject::connect(qmlWatcher_.get(), &QFileSystemWatcher::fileChanged, clearCache);

	QStringList paths = qmlEngine_->importPathList();
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

void QtFramework::registerDefaultComponents()
{
	// Find all (type)_component[version].qml components located in the .qrc
	QDir resources(":/WGControls/Private/");

	std::string componentSuffix = "_component";
	std::string dirPath("WGControls/Private/");
	for (auto& resource : resources.entryList(QStringList("*_component*.qml")))
	{
		std::string pathStr = resource.toUtf8().constData();
		QUrl url = QtHelpers::resolveQmlPath(*qmlEngine_, (dirPath + pathStr).c_str());
		if (IComponent* component = createComponent(url))
		{
			defaultComponents_.emplace_back(component);
			// Find the (type) from (type)_component[version].qml
			auto componentType = pathStr.substr(0, pathStr.rfind("_"));
			// Find the optional [version] from (type)_component[version].qml
			auto version = pathStr.substr(pathStr.rfind(componentSuffix) + componentSuffix.size());
			// Strip off the .qml part
			version = version.substr(0, version.size() - 4);
			// If there is a version the format is major followed by minor (only support for 0-9 minor version)
			//	Version 2.0 => (type)_component20.qml
			//	Version 2.5 => (type)_component25.qml
			//	Version 25.0 => (type)_component250.qml
			if (version.size() > 1)
			{
				version.insert(version.size() - 1, ".");
			}
			registerComponent(componentType.c_str(), version.c_str(), *component);
		}
	}
}

template <class TArray, size_t size>
inline int countof(const TArray (&)[size])
{
	return size;
};

void QtFramework::registerDefaultComponentProviders()
{
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<int8_t>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<uint8_t>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<short>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<unsigned short>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<int>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<unsigned int>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<long>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<unsigned long>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<long long>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<unsigned long long>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<float>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<double>("number"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<const char*>("string"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<const wchar_t*>("string"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<std::string>("string"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<std::wstring>("string"));

	defaultComponentProviders_.emplace_back(new GenericComponentProvider<bool>("boolean"));

	ItemRole::Id enumRoles[] = { IsEnumRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("enum", enumRoles, countof(enumRoles)));
	ItemRole::Id thumbnailRoles[] = { IsThumbnailRole::roleId_ };
	defaultComponentProviders_.emplace_back(
	new SimpleComponentProvider("thumbnail", thumbnailRoles, countof(thumbnailRoles)));
	ItemRole::Id sliderRoles[] = { IsSliderRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("slider", sliderRoles, countof(sliderRoles)));

	defaultComponentProviders_.emplace_back(new GenericComponentProvider<Vector2>("vector2"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<Vector3>("vector3"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<Vector4>("vector4"));

	ItemRole::Id colorRoles[] = { IsColorRole::roleId_ };
	defaultComponentProviders_.emplace_back(
	new GenericComponentProvider<Vector3>("color3", colorRoles, countof(colorRoles)));
	defaultComponentProviders_.emplace_back(
	new GenericComponentProvider<Vector4>("color4", colorRoles, countof(colorRoles)));

	ItemRole::Id urlRoles[] = { IsUrlRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("file", urlRoles, countof(urlRoles)));

	ItemRole::Id actionRoles[] = { IsActionRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("action", actionRoles, countof(actionRoles)));

	for (auto& defaultComponentProvider : defaultComponentProviders_)
	{
		registerComponentProvider(*defaultComponentProvider);
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
		std::shared_ptr<IDialog> dialog(createDialog(path.c_str(), model).release());

		if (dialog)
		{
			defaultDialogs_.emplace_back(dialog);
			registerDialog(type.c_str(), version.c_str(), dialog);
		}
	}
}

void QtFramework::registerDefaultTypeConverters()
{
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<Variant>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<bool>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<int>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<unsigned int>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<long int, qint64>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<long unsigned int, quint64>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<long long, qint64>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<unsigned long long, quint64>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<float>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<double>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<std::shared_ptr<BinaryBlock>>());
	defaultTypeConverters_.emplace_back(new StringQtTypeConverter());
	defaultTypeConverters_.emplace_back(new WGVectorQtTypeConverter());
	for (auto& defaultTypeConverter : defaultTypeConverters_)
	{
		registerTypeConverter(*defaultTypeConverter);
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
void QtFramework::showShortcutConfig() const
{
	if (shortcutDialog_ == nullptr)
	{
		return;
	}
	shortcutDialog_->showModal();
}

//------------------------------------------------------------------------------
std::unique_ptr<IDialog> QtFramework::createDialog(const char* resource, ObjectHandleT<DialogModel> model)
{
	{
		auto predicate = [](const TemporaryDialogData& dialog) -> bool { return std::get<CLOSED>(dialog); };

		std::lock_guard<std::mutex> lock(temporaryDialogMutex_);
		auto eraseFrom = std::remove_if(temporaryDialogs_.begin(), temporaryDialogs_.end(), predicate);
		temporaryDialogs_.erase(eraseFrom, temporaryDialogs_.end());
	}

	std::unique_ptr<IDialog> dialog(new QmlDialog(context_, *qmlEngine_, *this));
	dialog->setModel(model);
	dialog->load(resource);

	return dialog;
}

//------------------------------------------------------------------------------
void QtFramework::showDialog(const char* resource, const IDialog::Mode mode, const IDialog::ClosedCallback& callback)
{
	showDialog(resource, nullptr, mode, callback);
}

//------------------------------------------------------------------------------
void QtFramework::showDialog(const char* resource, ObjectHandleT<DialogModel> model, const IDialog::Mode mode,
                             const IDialog::ClosedCallback& callback)
{
	auto dialog = createDialog(resource, model);

	if (callback)
	{
		dialog->connectClosedCallback(callback);
	}

	dialog->show(mode);

	if (mode == IDialog::Mode::MODELESS)
	{
		auto markDialogClosedCallback = [this](IDialog& dialog) {
			auto dialogPointer = &dialog;
			auto predicate = [dialogPointer](const TemporaryDialogData& dialog) -> bool {
				return std::get<DIALOG>(dialog).get() == dialogPointer;
			};

			std::lock_guard<std::mutex> lock(temporaryDialogMutex_);
			auto itr = std::find_if(temporaryDialogs_.begin(), temporaryDialogs_.end(), predicate);

			if (itr != temporaryDialogs_.end())
			{
				std::get<CLOSED>(*itr) = true;
			}
		};

		auto connection = dialog->connectClosedCallback(markDialogClosedCallback);

		std::lock_guard<std::mutex> lock(temporaryDialogMutex_);
		temporaryDialogs_.emplace_back(std::make_tuple(std::move(dialog), false, connection));
	}
}

//------------------------------------------------------------------------------
bool QtFramework::getWGCopyableEnableStatus() const
{
	assert(globalQmlSettings_ != nullptr);
	return globalQmlSettings_->property("wgCopyableEnabled").toBool();
}

//------------------------------------------------------------------------------
void QtFramework::setWGCopyableEnableStatus(bool enabled)
{
	assert(globalQmlSettings_ != nullptr);
	globalQmlSettings_->setProperty("wgCopyableEnabled", enabled);
}

//------------------------------------------------------------------------------
void QtFramework::onApplicationStartUp()
{
	auto context = actionManager_->getContextObject();
	createWindowInternal("private/shortcut_dialog.qml", IUIFramework::ResourceType::Url, context,
	                     [this](std::unique_ptr<IWindow>& window) {
		                     shortcutDialog_ = std::move(window);
		                     if (shortcutDialog_ != nullptr)
		                     {
			                     shortcutDialog_->hide();
			                     actionManager_->registerEventHandler(shortcutDialog_.get());
		                     }
		                 },
	                     false);
}
} // end namespace wgt
