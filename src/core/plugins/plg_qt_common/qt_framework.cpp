#include "qt_framework.hpp"
#include "qt_preferences.hpp"
#include "core_data_model/i_item_role.hpp"

#include "core_qt_common/i_qt_type_converter.hpp"
#include "core_qt_common/qml_component.hpp"
#include "core_qt_common/qml_view.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_qt_common/qt_default_spacing.hpp"
#include "core_qt_common/qt_global_settings.hpp"
#include "core_qt_common/qt_window.hpp"
#include "core_qt_common/qml_window.hpp"
#include "core_qt_common/string_qt_type_converter.hpp"
#include "core_qt_common/vector_qt_type_converter.hpp"
#include "core_qt_common/qt_image_provider.hpp"
#include "core_qt_common/qt_image_provider_old.hpp"
#include "core_qt_common/shared_controls.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/qml_component_loader_helper.hpp"
#include "core_qt_script/qt_scripting_engine.hpp"
#include "core_qt_script/qt_script_object.hpp"
#include "core_common/platform_env.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/serializer/i_serialization_manager.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_command_system/i_command_event_listener.hpp"
#include "core_command_system/i_command_manager.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_component_provider.hpp"
#include "core_ui_framework/generic_component_provider.hpp"

#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_data_model/i_item.hpp"

#include "wg_types/string_ref.hpp"
#include "core_common/ngt_windows.hpp"

#include <array>
#include <QApplication>
#include <QFile>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QString>
#include <QWidget>
#include <QDir>
#include <QMessageBox>

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE {
#endif

	bool qRegisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);

	bool qUnregisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);

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
		void progressMade( const CommandInstance & commandInstance ) const override
		{
			QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
		}
	};
}

QtFramework::QtFramework( IComponentContext & contextManager )
	: qmlEngine_( new QQmlEngine() )
	, scriptingEngine_( new QtScriptingEngine() )
	, palette_( new QtPalette() )
	, defaultQmlSpacing_( new QtDefaultSpacing() )
	, globalQmlSettings_( new QtGlobalSettings() )
	, preferences_( nullptr )
	, commandManager_ ( contextManager )
{

	char ngtHome[MAX_PATH];
	if (Environment::getValue<MAX_PATH>( "NGT_HOME", ngtHome ))
	{
		qmlEngine_->addPluginPath( ngtHome );
		qmlEngine_->addImportPath( ngtHome );
	}

	// Search Qt resource path or Url by default
	qmlEngine_->addImportPath("qrc:/");
	qmlEngine_->addImportPath(":/");
}

QtFramework::~QtFramework()
{
}

void QtFramework::initialise( IComponentContext & contextManager )
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
	registerDefaultComponents();
	registerDefaultComponentProviders();
	registerDefaultTypeConverters();

	scriptingEngine_->initialise( *this, contextManager );

	auto rootContext = qmlEngine_->rootContext();
	rootContext->setContextObject( scriptingEngine_.get() );
	rootContext->setContextProperty( "palette", palette_.get() );
	rootContext->setContextProperty( "defaultSpacing", defaultQmlSpacing_.get() );
	rootContext->setContextProperty( "globalSettings", globalQmlSettings_.get() );
			
	ObjectHandle obj = ObjectHandle( &contextManager );
	rootContext->setContextProperty( "componentContext", QtHelpers::toQVariant( obj, rootContext ) );
	
	qmlEngine_->addImageProvider( QtImageProvider::providerId(), new QtImageProvider() );
	qmlEngine_->addImageProvider( QtImageProviderOld::providerId(), new QtImageProviderOld() );

	if (commandManager_ != nullptr)
	{
		commandEventListener_.reset( new QtFramework_Locals::QtCommandEventListener );
		commandManager_->registerCommandStatusListener( commandEventListener_.get() );
	}

	auto definitionManager = contextManager.queryInterface< IDefinitionManager >();
	preferences_.reset( new QtPreferences() );
    preferences_->init( contextManager );

	SharedControls::initDefs( *definitionManager );
}

void QtFramework::finalise()
{
	if (commandManager_ != nullptr)
	{
		commandManager_->deregisterCommandStatusListener( commandEventListener_.get() );
	}

	unregisterResources();
    preferences_->fini();
	qmlEngine_->removeImageProvider( QtImageProviderOld::providerId() );
	qmlEngine_->removeImageProvider( QtImageProvider::providerId() );
	scriptingEngine_->finalise();
	globalQmlSettings_ = nullptr;
	defaultQmlSpacing_ = nullptr;
	palette_ = nullptr;
	qmlEngine_ = nullptr;
	scriptingEngine_ = nullptr;
	preferences_ = nullptr;

	defaultTypeConverters_.clear();
	defaultComponentProviders_.clear();
	defaultComponents_.clear();
}

QQmlEngine * QtFramework::qmlEngine() const
{
	return qmlEngine_.get();
}

const QtPalette * QtFramework::palette() const
{
	return palette_.get();
}

void QtFramework::addImportPath( const QString& path )
{
	QDir importPath( path );
	if (importPath.exists() && importPath.isReadable())
	{
		qmlEngine_->addImportPath( path );
	}
}

QtGlobalSettings * QtFramework::qtGlobalSettings() const
{
	return globalQmlSettings_.get();
}

void QtFramework::registerTypeConverter( IQtTypeConverter & converter ) /* override */
{
	typeConverters_.registerTypeConverter( converter );
}

void QtFramework::deregisterTypeConverter( IQtTypeConverter & converter ) /* override */
{
	typeConverters_.deregisterTypeConverter( converter );
}

bool QtFramework::registerResourceData( const unsigned char * qrc_struct, const unsigned char * qrc_name,
	const unsigned char * qrc_data )
{
	if (!qRegisterResourceData( 0x01, qrc_struct, qrc_name, qrc_data ))
	{
		return false;
	}

	registeredResources_.push_back( std::make_tuple( qrc_struct, qrc_name, qrc_data ) );
	return true;
}

QVariant QtFramework::toQVariant(const Variant & variant, QObject* parent) const
{
	QVariant qVariant( QVariant::Invalid );
	typeConverters_.toScriptType(variant, qVariant, parent);
	return qVariant;
}

Variant QtFramework::toVariant( const QVariant & qVariant ) const
{
	Variant variant;
	typeConverters_.toVariant( qVariant, variant );
	return variant;
}

QQmlComponent * QtFramework::toQmlComponent( IComponent & component )
{
	// TODO replace this with a proper UI adapter interface
	auto qmlComponent = dynamic_cast< QmlComponent * >( &component );
	if (qmlComponent != nullptr)
	{
		return qmlComponent->component();
	}

	return nullptr;
}

QWidget * QtFramework::toQWidget( IView & view )
{
	// TODO replace this with a proper UI adapter interface
	auto qmlView = dynamic_cast< IQtView * >( &view );
	if (qmlView != nullptr)
	{
		auto widget = qmlView->releaseView();
		widget->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
		widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		widget->setFocusPolicy( Qt::StrongFocus );
		return widget;
	}

	auto widget = dynamic_cast< QWidget * >( &view );
	if (widget != nullptr)
	{
		return widget;
	}

	return nullptr;
}

void QtFramework::retainQWidget( IView & view )
{
	// TODO replace this with a proper UI adapter interface
	auto qmlView = dynamic_cast< IQtView * >( &view );
	if (qmlView != nullptr)
	{
		qmlView->retainView();
	}
}

std::unique_ptr< IAction > QtFramework::createAction(
	const char * id, std::function<void( IAction* )> func, 
	std::function<bool( const IAction* )> enableFunc,
	std::function<bool( const IAction* )> checkedFunc )
{
	return actionManager_.createAction( id, func, enableFunc, checkedFunc );
}

std::unique_ptr< IComponent > QtFramework::createComponent( 
	const char * resource, ResourceType type )
{
	QUrl url;
	switch (type)
	{
	case IUIFramework::ResourceType::File:
		url = QUrl::fromLocalFile( resource );
		break;

	case IUIFramework::ResourceType::Url:
		url = QtHelpers::resolveQmlPath( *qmlEngine_, resource );
		break;
	}

	auto qmlComponent = createComponent( url );
	if (type == IUIFramework::ResourceType::Buffer)
	{
		qmlComponent->component()->setData( resource, QUrl() );
	}
	return std::unique_ptr< IComponent >( qmlComponent );
}


QmlComponent * QtFramework::createComponent( const QUrl & resource )
{
	auto qmlComponent = new QmlComponent( *qmlEngine_ );
	if (!resource.isEmpty())
	{
		QmlComponentLoaderHelper helper( qmlComponent->component(), resource );
		helper.load( true );
	}
	return qmlComponent;
}


//------------------------------------------------------------------------------
std::unique_ptr< IView > QtFramework::createView(
	const char * resource, ResourceType type,
	const ObjectHandle & context)
{
	NGT_WARNING_MSG("Deprecated function call, please use async version instead" );
	std::unique_ptr< IView > returnView;
	createViewInternal(
		nullptr,
		resource, type,
		context,
		[ &returnView ] ( std::unique_ptr< IView > & view )
	{
		returnView = std::move( view );
	}, false);
	return returnView;
}


//------------------------------------------------------------------------------
std::unique_ptr< IView > QtFramework::createView(const char* uniqueName,
	const char * resource, ResourceType type,
	const ObjectHandle & context)
{
	NGT_WARNING_MSG("Deprecated function call, please use async version instead");
	std::unique_ptr< IView > returnView;
	createViewInternal(
		uniqueName,
		resource, type,
		context,
		[&returnView](std::unique_ptr< IView > & view)
	{
		returnView = std::move(view);
	}, false);
	return returnView;
}

//------------------------------------------------------------------------------
void QtFramework::createViewAsync(
	const char* uniqueName,
	const char * resource, ResourceType type,
	const ObjectHandle & context,
	std::function< void(std::unique_ptr< IView > &) > loadedHandler)
{
	createViewInternal(
		uniqueName,
		resource, type,
		context,
		loadedHandler, true);
}

//------------------------------------------------------------------------------
void QtFramework::createViewInternal(
	const char * uniqueName,
	const char * resource, ResourceType type,
	const ObjectHandle & context,
	std::function< void ( std::unique_ptr< IView > & ) > loadedHandler, bool async )
{
	// TODO: This function assumes the resource is a qml file

	QUrl qUrl;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
		qUrl = QUrl::fromLocalFile( resource );
		break;

	case IUIFramework::ResourceType::Url:
		qUrl = QtHelpers::resolveQmlPath( *qmlEngine_, resource );
		break;

	default:
		return;
	}

	// by default using resource path as qml view id
	auto view = new QmlView( uniqueName ? uniqueName : resource, *this, *qmlEngine_ );
	auto scriptObject = scriptingEngine_->createScriptObject(context, view->view());

	if (scriptObject)
	{
		view->setContextObject( scriptObject );
	}
	else
	{
		auto source = toQVariant( context, view->view() );
		view->setContextProperty( QString( "source" ), source );
	}

    view->load(qUrl, [loadedHandler, view ]()
	{
        std::unique_ptr< IView > localView( view );
		loadedHandler( localView );
	}, async );
}

QmlWindow * QtFramework::createQmlWindow()
{
	return new QmlWindow( *this, *qmlEngine() );
}

QtWindow * QtFramework::createQtWindow( QIODevice & source )
{
	return new QtWindow( *this, source );
}

std::unique_ptr< IWindow > QtFramework::createWindow( 
	const char * resource, ResourceType type,
	const ObjectHandle & context )
{
	// TODO: This function assumes the resource is a ui file containing a QMainWindow

	IWindow* window = nullptr;
	switch (type)
	{
	case IUIFramework::ResourceType::File:
		{
			std::unique_ptr< QFile > device( new QFile( resource ) );
			device->open( QFile::ReadOnly );
			assert( device != nullptr );
			window = createQtWindow( *device );
			device->close();
		}
		break;
	case IUIFramework::ResourceType::Url:
		{
			QUrl qUrl = QtHelpers::resolveQmlPath( *qmlEngine_, resource );
			auto qmlWindow = createQmlWindow();

			auto scriptObject = scriptingEngine_->createScriptObject(context, qmlWindow->window());

			if (scriptObject)
			{
				qmlWindow->setContextObject( scriptObject );
			}
			else
			{
				auto source = toQVariant( context, qmlWindow->window() );
				qmlWindow->setContextProperty( QString( "source" ), source );
			}

			qmlWindow->load( qUrl );
			window = qmlWindow;
		}
		break;

	default:
		return nullptr;
	}

	return std::unique_ptr< IWindow >( window );
}

void QtFramework::loadActionData( const char * resource, ResourceType type )
{
	std::unique_ptr< QIODevice > device;

	switch (type)
	{
	case IUIFramework::ResourceType::File:
		{
			device.reset( new QFile( resource ) );
			device->open( QFile::ReadOnly );
		}
		break;

	default:
		return;
	}

	assert( device != nullptr );
	actionManager_.loadActionData( *device );
	device->close();
}

void QtFramework::registerComponent( const char * id, IComponent & component )
{
	auto it = components_.find( id );
	if (it != components_.end())
	{
		return;
	}

	components_[id] = &component;
}

void QtFramework::registerComponentProvider( IComponentProvider & provider )
{
	componentProviders_.push_back( &provider );
}

IComponent * QtFramework::findComponent( const TypeId & typeId, 
	std::function< bool ( size_t ) > & predicate ) const
{
	for (auto componentProviderIt = componentProviders_.rbegin(); 
		componentProviderIt != componentProviders_.rend(); 
		++componentProviderIt)
	{
		auto id = ( *componentProviderIt )->componentId( typeId, predicate );
		if (id == nullptr)
		{
			continue;
		}

		auto componentIt = components_.find( id );
		if (componentIt != components_.end())
		{
			return componentIt->second;
		}
	}
	return nullptr;
}

void QtFramework::setPluginPath( const std::string& path )
{
	pluginPath_ = path;
}

const std::string& QtFramework::getPluginPath() const
{
	return pluginPath_;
}

int QtFramework::displayMessageBox( const char* title, const char* message, int buttons ) 
{
	struct MessageBoxQtMapping
	{
		MessageBoxButtons uiButton;
		QMessageBox::StandardButton qtButton;
	};

	MessageBoxQtMapping buttonMappings[] = 
	{
		{ Ok, QMessageBox::StandardButton::Ok },
		{ Cancel, QMessageBox::StandardButton::Cancel },
		{ Save, QMessageBox::StandardButton::Save },
		{ SaveAll, QMessageBox::StandardButton::SaveAll },
		{ Yes, QMessageBox::StandardButton::Yes },
		{ No, QMessageBox::StandardButton::No },
	};

	size_t count = sizeof( buttonMappings ) / sizeof( buttonMappings[0] );
	
	int desiredButtons = 0;

	for (size_t i = 0; i < count; ++i)
	{
		if (buttons & buttonMappings[i].uiButton)
		{
			desiredButtons |= buttonMappings[i].qtButton;
		}
	}

	assert( desiredButtons != 0 );

	QMessageBox messageBox( QMessageBox::Icon::NoIcon, title, message, (QMessageBox::StandardButton)desiredButtons );

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

	assert( result != 0 );

	return result;
}


void QtFramework::registerDefaultComponents()
{
	std::array<std::string, 13> types =
	{
		{"boolean", "string", "number", "enum", "slider", "polystruct",
		"vector2", "vector3", "vector4", "color3", "color4", "thumbnail", "file" }
	};

	for (auto & type : types)
	{
		std::string componentFile( "WGControls/" );
		componentFile += type;
		componentFile += "_component.qml";

		QUrl url = QtHelpers::resolveQmlPath( *qmlEngine_, componentFile.c_str() );
		if (IComponent * component = createComponent( url ) )
		{
			defaultComponents_.emplace_back( component );
			registerComponent( type.c_str(), *component );
		}
	}
}

void QtFramework::registerDefaultComponentProviders()
{
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<char>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned char>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<short>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned short>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<int>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned int>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<long>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned long>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<long long>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<unsigned long long>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<float>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<double>( "number" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<const char *>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<const wchar_t *>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<std::string>( "string" ) );
	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<std::wstring>( "string" ) );

	defaultComponentProviders_.emplace_back( 
		new GenericComponentProvider<bool>( "boolean" ) );

	size_t enumRoles[] = { IsEnumRole::roleId_ };
	defaultComponentProviders_.emplace_back( 
		new SimpleComponentProvider( "enum", enumRoles, sizeof( enumRoles )/sizeof( size_t ) ) );
	size_t thumbnailRoles[] = { IsThumbnailRole::roleId_ };
	defaultComponentProviders_.emplace_back( 
		new SimpleComponentProvider( "thumbnail", thumbnailRoles, sizeof( thumbnailRoles )/sizeof( size_t ) ) );
	size_t sliderRoles[] = { IsSliderRole::roleId_ };
	defaultComponentProviders_.emplace_back( 
		new SimpleComponentProvider( "slider", sliderRoles, sizeof( sliderRoles )/sizeof( size_t ) ) );

    defaultComponentProviders_.emplace_back(
        new GenericComponentProvider<Vector2>("vector2"));
	defaultComponentProviders_.emplace_back(
		new GenericComponentProvider<Vector3>( "vector3" ) );
	defaultComponentProviders_.emplace_back(
		new GenericComponentProvider<Vector4>("vector4"));

	size_t colorRoles[] = { IsColorRole::roleId_ };
	defaultComponentProviders_.emplace_back(
		new GenericComponentProvider<Vector3>( "color3", colorRoles, sizeof( colorRoles )/sizeof( size_t ) ) );
	defaultComponentProviders_.emplace_back(
		new GenericComponentProvider<Vector4>( "color4", colorRoles, sizeof( colorRoles )/sizeof( size_t ) ) );

	size_t urlRoles[] = { IsUrlRole::roleId_ };
	defaultComponentProviders_.emplace_back( 
		new SimpleComponentProvider( "file", urlRoles, sizeof( urlRoles )/sizeof( size_t ) ) );

	for (auto & defaultComponentProvider : defaultComponentProviders_)
	{
		registerComponentProvider( *defaultComponentProvider );
	}
}

void QtFramework::registerDefaultTypeConverters()
{
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<bool>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<int>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<unsigned int>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<long int, qint64>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<long unsigned int, quint64>() );
    defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<long long, qint64>() );
    defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<unsigned long long, quint64>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<float>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<double>() );
	defaultTypeConverters_.emplace_back( new GenericQtTypeConverter<std::shared_ptr< BinaryBlock >>() );
	defaultTypeConverters_.emplace_back( new StringQtTypeConverter() );
	defaultTypeConverters_.emplace_back( new WGVectorQtTypeConverter() );
	for (auto & defaultTypeConverter : defaultTypeConverters_)
	{
		registerTypeConverter( *defaultTypeConverter );
	}
}


void QtFramework::unregisterResources()
{
	for(auto res:registeredResources_)
	{
		qUnregisterResourceData( 0x01, std::get<0>( res ), std::get<1>( res ), std::get<2>( res ) );
	}
	registeredResources_.clear();
}
IPreferences * QtFramework::getPreferences()
{
	return preferences_.get();
}
} // end namespace wgt
