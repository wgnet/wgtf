#include "qt_framework_common.hpp"

#include "qt_scripting_engine_base.hpp"
#include "qml_component_manager.hpp"

#include "qt_int64.hpp"
#include "int64_type_converter.hpp"

#include "core_ui_framework/i_dialog.hpp"
#include "core_ui_framework/string_component_provider.hpp"
#include "core_ui_framework/generic_component_provider.hpp"

#include "core_qt_common/i_qt_type_converter.hpp"
#include "core_qt_common/string_qt_type_converter.hpp"
#include "core_qt_common/vector_qt_type_converter.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"

#include "core_qt_common/interfaces/i_qt_helpers.hpp"
#include "core_qt_common/qml_view.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_qt_common/qml_component.hpp"
#include "core_qt_common/qt_default_spacing.hpp"
#include "core_qt_common/qt_global_settings.hpp"
#include "core_qt_common/qt_global_logger.hpp"
#include "core_qt_common/qt_file_path_helper.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_qt_common/models/extensions/model_extension_manager.hpp"

#include "core_common/platform_env.hpp"
#include "core_common/platform_path.hpp"

#include "core_logging/logging.hpp"

#include "core_script/type_converter_queue.hpp"

#include "core_variant/variant.hpp"

#include "private/qt_ui_worker.hpp"

#include "core_dependency_system/depends.hpp"

#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"

#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlIncubationController>
#include <QVariant>
#include <QObject>

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE
{
#endif

#ifdef QT_NAMESPACE
}
using namespace QT_NAMESPACE;
#endif

namespace wgt
{
ITEMROLE(staticString)

struct QtFrameworkCommon::Implementation
{
	Implementation(QtFrameworkCommon& self, std::unique_ptr<QQmlEngine> qmlEngine,
	               std::unique_ptr<QtScriptingEngineBase> scriptingEngine, IDefinitionManager* definitionManager);

	void registerDefaultComponents();
	void registerDefaultComponentProviders();
	void registerDefaultTypeConverters();

	QtFrameworkCommon& self_;

	std::unique_ptr<QtPalette> palette_;
	std::unique_ptr<QtDefaultSpacing> defaultQmlSpacing_;
	std::unique_ptr<QtGlobalSettings> globalQmlSettings_;
	std::unique_ptr<QtGlobalLogger> globalQmlLogger_;
	std::unique_ptr<QtFilePathHelper> filePathHelper_;

	std::unique_ptr<QQmlIncubationController> incubationController_;
	std::atomic<int> incubationTime_;

	std::vector<std::unique_ptr<IComponent>> defaultComponents_;
	std::vector<std::unique_ptr<IComponentProvider>> defaultComponentProviders_;

	std::unique_ptr<QQmlEngine> qmlEngine_;
	std::unique_ptr<QtScriptingEngineBase> scriptingEngine_;

	std::unique_ptr<QmlComponentManager> qmlComponentManager_;
	std::unique_ptr<ModelExtensionManager> modelExtensionManager_;

	TypeConverterQueue<IQtTypeConverter, QVariant> typeConverters_;
	std::vector<std::unique_ptr<IQtTypeConverter>> defaultTypeConverters_;

	IDefinitionManager* definitionManager_;
};

QtFrameworkCommon::Implementation::Implementation(QtFrameworkCommon& self, std::unique_ptr<QQmlEngine> qmlEngine,
                                                  std::unique_ptr<QtScriptingEngineBase> scriptingEngine,
                                                  IDefinitionManager* definitionManager)
    : self_(self)
	, qmlEngine_(std::move(qmlEngine))
	, scriptingEngine_(std::move(scriptingEngine))
	, palette_(new QtPalette())
	, defaultQmlSpacing_(new QtDefaultSpacing())
	, globalQmlSettings_(new QtGlobalSettings())
	, globalQmlLogger_(new QtGlobalLogger())
	, filePathHelper_(new QtFilePathHelper())
	, incubationController_(new QQmlIncubationController)
	, incubationTime_(50)
	, qmlComponentManager_(new QmlComponentManager())
	, definitionManager_(definitionManager)
{
	modelExtensionManager_.reset(new ModelExtensionManager(self));

	qInstallMessageHandler(QtGlobalLogger::messageHandler);
}

void QtFrameworkCommon::Implementation::registerDefaultComponents()
{
	// Find all (type)_component[version].qml components located in the .qrc or import path
	QStringList resourcePaths(":/WGControls/Private/");
	for (const auto& path : qmlEngine_->importPathList())
	{
		resourcePaths.append(path + "/WGControls/Private/");
	}

	// Search in all import paths
	QStringList entries;
	for (const auto& path : resourcePaths)
	{
		QDir resources(path);
		entries = resources.entryList(QStringList("*_component*.qml"));
		if (!entries.empty())
		{
			break;
		}
	}

	std::string componentSuffix = "_component";
	std::string dirPath("WGControls/Private/");
	std::set< std::string > noAsyncComponents;

	//TODO: HACK to allow async Loader support until "Object destroyed during incubation"
	//is fixed according to this Qt bug https://bugreports.qt.io/browse/QTBUG-50992
	noAsyncComponents.insert("enum_component20.qml");
	noAsyncComponents.insert("angle_component.qml");
	noAsyncComponents.insert("polystruct_component20.qml");
	noAsyncComponents.insert("colorhdr_component.qml");
	for (const auto& resource : entries)
	{
		std::string pathStr = resource.toUtf8().constData();
		bool supportsAsync = noAsyncComponents.find( pathStr ) == noAsyncComponents.end();
		QUrl url = self_.resolveQmlPath((dirPath + pathStr).c_str());
		if (IComponent* component = self_.createComponent(url))
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
			qmlComponentManager_->registerComponent(componentType.c_str(), version.c_str(), *component, supportsAsync );
		}
	}
}

template <class TArray, size_t size>
inline int countof(const TArray (&)[size])
{
	return size;
};

void QtFrameworkCommon::Implementation::registerDefaultComponentProviders()
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
	defaultComponentProviders_.emplace_back(new StringComponentProvider("string"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<std::string>("string"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<std::wstring>("string"));
	defaultComponentProviders_.emplace_back(new StringComponentProvider("refobjectid"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<RefObjectId>("refobjectid"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<bool>("boolean"));
	defaultComponentProviders_.emplace_back(new StringComponentProvider("vector2"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<Vector2>("vector2"));
	defaultComponentProviders_.emplace_back(new StringComponentProvider("vector3"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<Vector3>("vector3"));
	defaultComponentProviders_.emplace_back(new StringComponentProvider("vector4"));
	defaultComponentProviders_.emplace_back(new GenericComponentProvider<Vector4>("vector4"));

	defaultComponentProviders_.emplace_back(new StringComponentProvider("angle"));
	defaultComponentProviders_.emplace_back(new StringComponentProvider("polystruct"));
	defaultComponentProviders_.emplace_back(new StringComponentProvider("time"));

	ItemRole::Id enumRoles[] = { IsEnumRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("enum", enumRoles, countof(enumRoles)));

	ItemRole::Id thumbnailRoles[] = { IsThumbnailRole::roleId_ };
	defaultComponentProviders_.emplace_back(
	new SimpleComponentProvider("thumbnail", thumbnailRoles, countof(thumbnailRoles)));

	ItemRole::Id sliderRoles[] = { IsSliderRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("slider", sliderRoles, countof(sliderRoles)));

	ItemRole::Id colorRoles[] = { IsColorRole::roleId_ };
	defaultComponentProviders_.emplace_back(
		new StringComponentProvider("vector3", "color3", colorRoles, countof(colorRoles)));
	defaultComponentProviders_.emplace_back(
		new GenericComponentProvider<Vector3>("color3", colorRoles, countof(colorRoles)));
	defaultComponentProviders_.emplace_back(
		new StringComponentProvider("vector4", "color4", colorRoles, countof(colorRoles)));
	defaultComponentProviders_.emplace_back(
		new GenericComponentProvider<Vector4>("color4", colorRoles, countof(colorRoles)));

	ItemRole::Id staticStringRoles[] = { ItemRole::staticStringId };
	defaultComponentProviders_.emplace_back(
	new SimpleComponentProvider("staticstring", staticStringRoles, countof(staticStringRoles)));

	ItemRole::Id urlRoles[] = { IsUrlRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("file", urlRoles, countof(urlRoles)));

	ItemRole::Id actionRoles[] = { IsActionRole::roleId_ };
	defaultComponentProviders_.emplace_back(new SimpleComponentProvider("action", actionRoles, countof(actionRoles)));

	for (auto& defaultComponentProvider : defaultComponentProviders_)
	{
		qmlComponentManager_->registerComponentProvider(*defaultComponentProvider);
	}
}

void QtFrameworkCommon::Implementation::registerDefaultTypeConverters()
{
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<Variant>());
	defaultTypeConverters_.emplace_back(new Int64TypeConverter<QtInt64>());
	defaultTypeConverters_.emplace_back(new Int64TypeConverter<QtUInt64>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<bool>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<int>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<unsigned int>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<float>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<double>());
	defaultTypeConverters_.emplace_back(new GenericQtTypeConverter<std::shared_ptr<BinaryBlock>>());
	defaultTypeConverters_.emplace_back(new StringQtTypeConverter());
	defaultTypeConverters_.emplace_back(new WGVectorQtTypeConverter());

	QMetaType::registerComparators<Variant>();

	// Automatic converters for 64 bit wrapper type.
	QMetaType::registerConverter<std::shared_ptr<QtInt64>, bool>(
	[](const std::shared_ptr<QtInt64> v) { return v->value(); });
	QMetaType::registerConverter<bool, std::shared_ptr<QtInt64>>(
	[](int v) { return std::shared_ptr<QtInt64>(new QtInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtInt64>, int>(
	[](const std::shared_ptr<QtInt64> v) { return v->value(); });
	QMetaType::registerConverter<int, std::shared_ptr<QtInt64>>(
	[](int v) { return std::shared_ptr<QtInt64>(new QtInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtInt64>, int64_t>(
	[](const std::shared_ptr<QtInt64> v) { return v->value(); });
	QMetaType::registerConverter<int64_t, std::shared_ptr<QtInt64>>(
	[](int64_t v) { return std::shared_ptr<QtInt64>(new QtInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtInt64>, double>(
	[](const std::shared_ptr<QtInt64> v) { return v->value(); });
	QMetaType::registerConverter<double, std::shared_ptr<QtInt64>>(
	[](double v) { return std::shared_ptr<QtInt64>(new QtInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtInt64>, QString>(
	[](const std::shared_ptr<QtInt64> v) { return v->stringValue(); });
	QMetaType::registerConverter<QString, std::shared_ptr<QtInt64>>(
	[](const QString& v) { return std::shared_ptr<QtInt64>(new QtInt64(v)); });
	QMetaType::registerComparators<std::shared_ptr<QtInt64>>();

	QMetaType::registerConverter<std::shared_ptr<QtUInt64>, bool>(
	[](std::shared_ptr<QtUInt64> v) { return v->value(); });
	QMetaType::registerConverter<bool, std::shared_ptr<QtUInt64>>(
	[](int v) { return std::shared_ptr<QtUInt64>(new QtUInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtUInt64>, int>(
	[](std::shared_ptr<QtUInt64> v) { return v->value(); });
	QMetaType::registerConverter<int, std::shared_ptr<QtUInt64>>(
	[](int v) { return std::shared_ptr<QtUInt64>(new QtUInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtUInt64>, uint64_t>(
	[](std::shared_ptr<QtUInt64> v) { return v->value(); });
	QMetaType::registerConverter<uint64_t, std::shared_ptr<QtUInt64>>(
	[](uint64_t v) { return std::shared_ptr<QtUInt64>(new QtUInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtUInt64>, double>(
	[](std::shared_ptr<QtUInt64> v) { return v->value(); });
	QMetaType::registerConverter<double, std::shared_ptr<QtUInt64>>(
	[](double v) { return std::shared_ptr<QtUInt64>(new QtUInt64(v)); });
	QMetaType::registerConverter<std::shared_ptr<QtUInt64>, QString>(
	[](std::shared_ptr<QtUInt64> v) { return v->stringValue(); });
	QMetaType::registerConverter<QString, std::shared_ptr<QtUInt64>>(
	[](QString v) { return std::shared_ptr<QtUInt64>(new QtUInt64(v)); });
	QMetaType::registerComparators<std::shared_ptr<QtUInt64>>();

	for (auto& defaultTypeConverter : defaultTypeConverters_)
	{
		self_.registerTypeConverter(*defaultTypeConverter);
	}
}

QtFrameworkCommon::QtFrameworkCommon(std::unique_ptr<QQmlEngine> qmlEngine,
                                     std::unique_ptr<QtScriptingEngineBase> scriptingEngine,
                                     IDefinitionManager* definitionManager)
    : impl_(new Implementation(*this, std::move(qmlEngine), std::move(scriptingEngine), definitionManager))
{
	char wgtHome[MAX_PATH];
	if (Environment::getValue<MAX_PATH>("WGT_HOME", wgtHome))
	{
		impl_->qmlEngine_->addPluginPath(wgtHome);
		impl_->qmlEngine_->addImportPath(wgtHome);
	}

	qRegisterMetaType<QtUIFunctionWrapper*>("QtUIFunctionWrapper*");
	qRegisterMetaType<IDialog::Result>("IDialog::Result");
	qRegisterMetaType<QVector<int>>("QVector<int>");

	// Search Qt resource path or Url by default
	impl_->qmlEngine_->addImportPath("qrc:/");
	impl_->qmlEngine_->addImportPath(":/");
	impl_->qmlEngine_->setIncubationController(impl_->incubationController_.get());
}

QtFrameworkCommon::~QtFrameworkCommon()
{
}

void QtFrameworkCommon::initialise()
{
	impl_->registerDefaultComponents();
	impl_->registerDefaultComponentProviders();
	impl_->registerDefaultTypeConverters();
	modelExtensionManager()->initialise();

	impl_->scriptingEngine_->initialise();

	auto rootContext = impl_->qmlEngine_->rootContext();
	rootContext->setContextObject(impl_->scriptingEngine_.get());
	rootContext->setContextProperty("palette", impl_->palette_.get());
	rootContext->setContextProperty("defaultSpacing", impl_->defaultQmlSpacing_.get());
	rootContext->setContextProperty("globalSettings", impl_->globalQmlSettings_.get());
	rootContext->setContextProperty("globalLogger", impl_->globalQmlLogger_.get());
	rootContext->setContextProperty("filePathHelper", impl_->filePathHelper_.get());
	rootContext->setContextProperty("modelExtensionManager", modelExtensionManager());
}

void QtFrameworkCommon::finalise()
{
	impl_->qmlEngine_->setIncubationController(nullptr);
	impl_->palette_ = nullptr;
	impl_->globalQmlSettings_ = nullptr;
	impl_->globalQmlLogger_ = nullptr;
	impl_->filePathHelper_ = nullptr;
	impl_->defaultQmlSpacing_ = nullptr;
	impl_->incubationController_ = nullptr;

	impl_->scriptingEngine_->finalise();

	impl_->qmlEngine_ = nullptr;
	impl_->scriptingEngine_ = nullptr;

	impl_->defaultComponentProviders_.clear();
	impl_->defaultComponents_.clear();

	impl_->defaultTypeConverters_.clear();
}

QmlComponentManager* QtFrameworkCommon::qmlComponentManager()
{
	return impl_->qmlComponentManager_.get();
}

const QmlComponentManager* QtFrameworkCommon::qmlComponentManager() const
{
	return impl_->qmlComponentManager_.get();
}

ModelExtensionManager* QtFrameworkCommon::modelExtensionManager()
{
	return impl_->modelExtensionManager_.get();
}

const ModelExtensionManager* QtFrameworkCommon::modelExtensionManager() const
{
	return impl_->modelExtensionManager_.get();
}

QQmlEngine* QtFrameworkCommon::qmlEngine() const
{
	return impl_->qmlEngine_.get();
}

QtScriptingEngineBase* QtFrameworkCommon::scriptingEngine()
{
	return impl_->scriptingEngine_.get();
}

QtPalette* QtFrameworkCommon::palette() const
{
	return impl_->palette_.get();
}

QtGlobalSettings* QtFrameworkCommon::qtGlobalSettings() const
{
	return impl_->globalQmlSettings_.get();
}

IDefinitionManager* QtFrameworkCommon::definitionManager() const
{
	return impl_->definitionManager_;
}

void QtFrameworkCommon::setIncubationTime(int msecs)
{
	impl_->incubationTime_ = msecs;
}

int QtFrameworkCommon::incubationTime() const
{
	return impl_->incubationTime_;
}

QString QtFrameworkCommon::resolveFilePath(const QQmlEngine& qmlEngine, const char* relativePath)
{
	if (relativePath == nullptr)
	{
		NGT_ERROR_MSG("QtHelpers::resolveFilePath(): relativePath is NULL.\n");
		return QString();
	}

	const std::string filepath(QtFilePathHelper::removeQRCPrefix(relativePath));

	QStringList paths = qmlEngine.importPathList();
	for (auto path : paths)
	{
		QFileInfo info(QDir(path), filepath.c_str());
		if (info.exists() && info.isFile())
		{
			return info.canonicalFilePath();
		}
	}

	return QString(relativePath);
}

QUrl QtFrameworkCommon::resolveQmlPath(const QQmlEngine& qmlEngine, const char* relativePath)
{
	QUrl url;

	if (relativePath == nullptr)
	{
		NGT_ERROR_MSG("QtHelpers::resolveQmlPath(): relativePath is NULL.\n");
		return url;
	}

	const std::string filepath(QtFilePathHelper::removeQRCPrefix(relativePath));

	QStringList paths = qmlEngine.importPathList();
	for (auto path : paths)
	{
		QFileInfo info(QDir(path), filepath.c_str());
		if (info.exists() && info.isFile())
		{
			url = QUrl::fromLocalFile(info.canonicalFilePath());
			break;
		}
	}

	// fallback to qrc
	if (url.isEmpty())
	{
		QString filePath = relativePath[0] != '/' ? QString("/") + relativePath : relativePath;
		QFile file(QString(":") + filePath);
		if (file.exists())
		{
			url.setScheme("qrc");
			url.setPath(filePath);
		}
	}

	return url;
}

QString QtFrameworkCommon::resolveFilePath(const char* relativePath) const
{
	return resolveFilePath(*impl_->qmlEngine_, relativePath);
}

QUrl QtFrameworkCommon::resolveQmlPath(const char* relativePath) const
{
	return resolveQmlPath(*impl_->qmlEngine_, relativePath);
}

QmlComponent* QtFrameworkCommon::createComponent(const QUrl& resource)
{
	auto qmlComponent = new QmlComponent(*impl_->qmlEngine_);
	if (!resource.isEmpty())
	{
		QmlComponentLoaderHelper helper(qmlComponent->component(), resource);
		helper.load(true);
	}
	return qmlComponent;
}

void QtFrameworkCommon::registerTypeConverter(IQtTypeConverter& converter)
{
	impl_->typeConverters_.registerTypeConverter(converter);
}

void QtFrameworkCommon::deregisterTypeConverter(IQtTypeConverter& converter)
{
	impl_->typeConverters_.deregisterTypeConverter(converter);
}

QVariant QtFrameworkCommon::toQVariant(const Variant& variant, QObject* parent) const
{
	QVariant qVariant = QVariant::Invalid;
	impl_->typeConverters_.toScriptType(variant, qVariant, parent);
	return qVariant;
}

Variant QtFrameworkCommon::toVariant(const QVariant& qVariant) const
{
	Variant variant;
	impl_->typeConverters_.toVariant(qVariant, variant);
	return variant;
}
} // end namespace wgt
