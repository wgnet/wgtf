#include "qt_framework.hpp"
#include "qt_preferences.hpp"
#include "core_data_model/i_item_role.hpp"

#include "core_qt_common/qt_types.hpp"
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

#include "wg_types/string_ref.hpp"
#include "core_common/ngt_windows.hpp"

#include "private/qt_ui_worker.hpp"
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
#include <algorithm>
#include <QQmlIncubationController>

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
	: Depends( contextManager )
	, qmlEngine_( new QQmlEngine() )
	, scriptingEngine_( new QtScriptingEngine() )
	, palette_( new QtPalette() )
	, defaultQmlSpacing_( new QtDefaultSpacing() )
	, globalQmlSettings_( new QtGlobalSettings() )
	, actionManager_( new QtActionManager( contextManager ) )
	, context_( contextManager )
	, worker_( new QtUIWorker() )
	, incubationTime_( 50 )
	, incubationController_( new QQmlIncubationController )
	, preferences_( nullptr )
	, useAsyncViewLoading_ (true)
{

	char wgtHome[MAX_PATH];
	if (Environment::getValue<MAX_PATH>( "WGT_HOME", wgtHome ))
	{
		qmlEngine_->addPluginPath( wgtHome );
		qmlEngine_->addImportPath( wgtHome );
	}

	// Search Qt resource path or Url by default
	qmlEngine_->addImportPath("qrc:/");
	qmlEngine_->addImportPath(":/");
	qmlEngine_->setIncubationController(incubationController_.get());

	qRegisterMetaType<QtUIFunctionWrapper*>("QtUIFunctionWrapper*");
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

	auto commandManager = get< ICommandManager >();
	if (commandManager != nullptr)
	{
		commandEventListener_.reset( new QtFramework_Locals::QtCommandEventListener );
		commandManager->registerCommandStatusListener( commandEventListener_.get() );
	}

    preferences_ = contextManager.registerInterface( new QtPreferences( contextManager ) );

	auto definitionManager = contextManager.queryInterface< IDefinitionManager >();
	actionManager_->init();
	SharedControls::initDefs( *definitionManager );
	auto uiApplication = get< IUIApplication >();
	assert( uiApplication != nullptr );
	connections_ += uiApplication->signalStartUp.connect( std::bind( &QtFramework::onApplicationStartUp, this ) );
}

QMetaObject* QtFramework::constructMetaObject( QMetaObject* original )
{
	if ( original == nullptr ) 
	{
		return original;
	}

	char* originalName = (char*)original->d.stringdata->data();
	
	bool found = 
		std::binary_search( qtNames, qtNames + qtTypeCount, originalName, 
		[]( const char * const a, const char* const b )
	{
		return strcmp( a, b ) < 0;
	});


	if ( found )
	{
		return original;
	}

	for (size_t i = 0; i < registeredTypes_.size(); ++i)
	{
		char* name = (char*)registeredTypes_[i].get()->d.stringdata->data();
		if ( strcmp( name, originalName ) == 0)
		{
			return registeredTypes_[i].get();
		}
	}

	std::unique_ptr<QMetaObject> createdMeta( new QMetaObject );
	*createdMeta = *original;
	createdMeta->d.superdata = constructMetaObject( (QMetaObject*)createdMeta->d.superdata);
	registeredTypes_.push_back( std::move( createdMeta ) );

	return registeredTypes_.back().get();
}

void QtFramework::registerQmlType( ObjectHandle type )
{
	QQmlPrivate::RegisterType* qtype = type.getBase<QQmlPrivate::RegisterType>();
	assert( qtype != nullptr );


	if (qtype)
	{
		qtype->metaObject = constructMetaObject( (QMetaObject*)qtype->metaObject );
		qtype->extensionMetaObject = constructMetaObject( (QMetaObject*)qtype->extensionMetaObject );
		qtype->attachedPropertiesMetaObject = constructMetaObject( (QMetaObject*)qtype->attachedPropertiesMetaObject );

		QQmlPrivate::qmlregister( QQmlPrivate::TypeRegistration, qtype );
	}
}

void QtFramework::finalise()
{
    auto commandManager = get< ICommandManager >();
    if (commandManager != nullptr)
    {
        commandManager->deregisterCommandStatusListener( commandEventListener_.get() );
    }
    if(shortcutDialog_ != nullptr)
    {
        auto uiApplication = get< IUIApplication >();
        assert( uiApplication != nullptr );
        uiApplication->removeWindow( *shortcutDialog_ );
    }

    unregisterResources();
    actionManager_->fini();
    shortcutDialog_ = nullptr;
    qmlEngine_->removeImageProvider( QtImageProviderOld::providerId() );
    qmlEngine_->removeImageProvider( QtImageProvider::providerId() );
    scriptingEngine_->finalise();
    qmlEngine_->setIncubationController( nullptr );
    incubationController_ = nullptr;
    globalQmlSettings_ = nullptr;
    defaultQmlSpacing_ = nullptr;
    palette_ = nullptr;
    qmlEngine_.reset();
    scriptingEngine_ = nullptr;

    context_.deregisterInterface( preferences_ );
    preferences_ = nullptr;

    defaultTypeConverters_.clear();
    defaultComponentProviders_.clear();
    defaultComponents_.clear();
    connections_.clear();
}

QQmlEngine * QtFramework::qmlEngine() const
{
	return qmlEngine_.get();
}

void QtFramework::setIncubationTime( int msecs )
{
	incubationTime_ = msecs;
}

void QtFramework::incubate()
{
	if(qmlEngine_ == nullptr)
	{
		return;
	}
	auto incubationCtrl = qmlEngine_->incubationController();
	if(incubationCtrl != nullptr)
	{
		if(incubationCtrl->incubatingObjectCount() > 0)
		{
			incubationCtrl->incubateFor( incubationTime_ );
		}
	}
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
	return actionManager_->createAction( id, func, enableFunc, checkedFunc );
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
void QtFramework::enableAsynchronousViewCreation( bool enabled )
{
	this->useAsyncViewLoading_ = enabled;
}


//------------------------------------------------------------------------------
std::unique_ptr< IView > QtFramework::createView(
	const char * resource, ResourceType type,
	const ObjectHandle & context)
{
	NGT_WARNING_MSG("Deprecated function call, please use async version instead" );
	auto returnView = createViewInternal(
		nullptr,
		resource, type,
		context,
		[] ( IView & view )
	{

	}, false);
	return returnView.get();
}


//------------------------------------------------------------------------------
std::unique_ptr< IView > QtFramework::createView(const char* uniqueName,
	const char * resource, ResourceType type,
	const ObjectHandle & context)
{
	NGT_WARNING_MSG("Deprecated function call, please use async version instead");
	auto returnView = createViewInternal(
		uniqueName,
		resource, type,
		context,
		[](IView & view)
	{
	}, false);
	return returnView.get();
}

//------------------------------------------------------------------------------
wg_future< std::unique_ptr< IView > > QtFramework::createViewAsync(
	const char* uniqueName,
	const char * resource, ResourceType type,
	const ObjectHandle & context,
	std::function< void( IView &) > loadedHandler)
{
    return createViewInternal( uniqueName, resource, type, context, loadedHandler
    , true );
}

//------------------------------------------------------------------------------
wg_future< std::unique_ptr< IView > > QtFramework::createViewInternal(
	const char * uniqueName,
	const char * resource, ResourceType type,
	const ObjectHandle & context,
	std::function< void ( IView & ) > loadedHandler, bool async )
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
		return std::future<std::unique_ptr<IView>>();
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
    std::shared_ptr<std::promise< std::unique_ptr< IView > >> promise = std::make_shared<std::promise< std::unique_ptr< IView > >>();
    wg_future<std::unique_ptr<IView>> wgFuture(promise->get_future(), 
        []()
    {
        QApplication::processEvents(QEventLoop::AllEvents, 1);
    });
    view->load(qUrl, [loadedHandler, view, promise ]()
	{
		loadedHandler( *view );
        promise->set_value( std::unique_ptr<IView>(view) );
    }, [view, promise ]()
    {
        promise->set_value( std::unique_ptr<IView>() );
        view->deleteLater();
    }, this->useAsyncViewLoading_ ? async : false );
	
	return wgFuture;
}

QmlWindow * QtFramework::createQmlWindow()
{
	return new QmlWindow( context_, *qmlEngine() );
}

QtWindow * QtFramework::createQtWindow( QIODevice & source )
{
	return new QtWindow( *this, source );
}


//------------------------------------------------------------------------------
void QtFramework::createWindowAsync( 
	const char * resource, ResourceType type,
	const ObjectHandle & context,
	std::function< void(std::unique_ptr< IWindow > & ) > loadedHandler )
{
	createWindowInternal(
		resource, type,
		context,
		loadedHandler, true);
}


//------------------------------------------------------------------------------
std::unique_ptr< IWindow > QtFramework::createWindow( 
	const char * resource, ResourceType type,
	const ObjectHandle & context )
{
	NGT_WARNING_MSG("Deprecated function call, please use async version instead" );
	std::unique_ptr< IWindow > returnWindow;
	createWindowInternal(
		resource, type,
		context,
		[ &returnWindow ] ( std::unique_ptr< IWindow > & window )
	{
		returnWindow = std::move( window );
	}, false);
	return returnWindow;
}


//------------------------------------------------------------------------------
void QtFramework::createWindowInternal( 
	const char * resource, ResourceType type,
	const ObjectHandle & context,
	std::function< void(std::unique_ptr< IWindow > &) > loadedHandler, 
	bool async )
{
	// TODO: This function assumes the resource is a ui file containing a QMainWindow
	switch (type)
	{
	case IUIFramework::ResourceType::File:
		{
			std::unique_ptr< QFile > device( new QFile( resource ) );
			device->open( QFile::ReadOnly );
			assert( device != nullptr );
			std::unique_ptr< IWindow > window( createQtWindow( *device ) );
			loadedHandler( window );
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

			qmlWindow->load( qUrl, async, 
				[loadedHandler, qmlWindow ]()
			{
				std::unique_ptr< IWindow > localWindow( qmlWindow );
				loadedHandler( localWindow );
			});
		}
		break;

	default:
		return;
	}
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
	auto size = device->size();
	auto data = device->readAll();
	device->close();
	if(data.isEmpty())
	{
		NGT_WARNING_MSG( "Read action data error from %s.\n", resource );
		return;
	}
	auto buffer = data.constData();
	FixedMemoryStream dataStream(buffer, size);
	actionManager_->loadActionData( dataStream );
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

//------------------------------------------------------------------------------
IPreferences * QtFramework::getPreferences()
{
	return context_.queryInterface< IPreferences >();
}

//------------------------------------------------------------------------------
void QtFramework::doOnUIThread( std::function< void() > action ) 
{
	QMetaObject::invokeMethod(
		worker_.get(), "doJob", Qt::QueuedConnection, Q_ARG( QtUIFunctionWrapper*, new QtUIFunctionWrapper( action ) ) );
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
void QtFramework::onApplicationStartUp()
{
    auto preferences = context_.queryInterface< IPreferences >();
    if( preferences )
    {
        preferences->loadDefaultPreferences();
    }

	auto context = actionManager_->getContextObject();
	auto viewCreator = get< IViewCreator >();
	viewCreator->createWindow( 
		"private/shortcut_dialog.qml",context,
		[ this ]( std::unique_ptr< IWindow > & window )
	{
		shortcutDialog_ = std::move( window );
		if (shortcutDialog_ != nullptr)
		{
			shortcutDialog_->hide();
			actionManager_->registerEventHandler( shortcutDialog_.get() );
		}
	});
}
} // end namespace wgt
