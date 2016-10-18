#include "qt_scripting_engine.hpp"

#include "qobject_qt_type_converter.hpp"
#include "qt_script_object.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/controls/wg_copy_controller.hpp"
#include "script_qt_type_converter.hpp"
#include "wg_list_iterator.hpp"
#include "collection_qt_type_converter.hpp"
#include "core_qt_common/image_qt_type_converter.hpp"
#include "core_qt_common/model_qt_type_converter.hpp"

#include "core_reflection/base_property.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_command_system/i_command_manager.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_copy_paste/i_copy_paste_manager.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"

#include <private/qmetaobjectbuilder_p.h>
#include <QVariant>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPointer>
#include <QMouseEvent>
#include <QApplication>

Q_DECLARE_METATYPE( wgt::ObjectHandle );

namespace wgt
{
struct QtScriptingEngine::Implementation
{
	Implementation( QtScriptingEngine& self )
		: self_( self )
		, defManager_( nullptr )
		, commandSystemProvider_( nullptr )
		, copyPasteManager_( nullptr )
		, uiApplication_( nullptr )
		, uiFramework_( nullptr )
		, contextManager_( nullptr )
	{
		propListener_ = std::make_shared<PropertyListener>( scriptObjects_ );
	}

	~Implementation()
	{
		propListener_ = nullptr;
		assert( scriptObjects_.empty() );
		std::lock_guard< std::mutex > guard( metaObjectsMutex_ );

		for (auto& metaObjectPair: metaObjects_)
		{
			free( metaObjectPair.second );
		}

		metaObjects_.clear();
	}

	void initialise( IQtFramework& qtFramework, IComponentContext& contextManager );

	QtScriptObject* createScriptObject(const Variant& object, QObject* parent);
	QMetaObject* getMetaObject( const IClassDefinition& classDefinition );

	QtScriptingEngine& self_;

	IDefinitionManager* defManager_;
	ICommandManager* commandSystemProvider_;
	ICopyPasteManager* copyPasteManager_;
	IUIApplication* uiApplication_;
	IUIFramework* uiFramework_;
	IComponentContext* contextManager_;

	std::mutex metaObjectsMutex_;
	std::unordered_map<std::string, QMetaObject*> metaObjects_;
	std::vector<std::unique_ptr< IQtTypeConverter>> qtTypeConverters_;

	struct ObjectHandleHash
	{
		size_t operator()(ObjectHandle const& node) const
		{
			return std::hash< void * >()(node.data());
		}
	};

	typedef std::unordered_map< QObject *, QPointer<QtScriptObject> > ParentMap;
	typedef std::unordered_map<
		ObjectHandle,
		std::pair< std::weak_ptr< QtScriptObjectData >,
	 	ParentMap >, ObjectHandleHash > ScriptObjectCollection;
	ScriptObjectCollection scriptObjects_;

	struct PropertyListener: public PropertyAccessorListener
	{
		PropertyListener( const ScriptObjectCollection & scriptObjects )
			: scriptObjects_( scriptObjects )
		{}

		void postSetValue( const PropertyAccessor& accessor, const Variant& value ) override;
		void postInvoke( const PropertyAccessor & accessor, Variant result, bool undo ) override;

		const ScriptObjectCollection & scriptObjects_;
	};

	std::shared_ptr<PropertyAccessorListener> propListener_;
};


void QtScriptingEngine::Implementation::initialise( IQtFramework& qtFramework, IComponentContext& contextManager )
{	
	contextManager_ = &contextManager;
	defManager_ = contextManager.queryInterface<IDefinitionManager>();
	commandSystemProvider_ = contextManager.queryInterface<ICommandManager>();
	copyPasteManager_ = contextManager.queryInterface<ICopyPasteManager>();
	uiApplication_ = contextManager_->queryInterface<IUIApplication>();
	uiFramework_ = contextManager_->queryInterface<IUIFramework>();
	assert( defManager_ );
	assert( commandSystemProvider_ );
	assert( copyPasteManager_ );
	assert( uiApplication_ );
	assert( uiFramework_ );

	// TODO: All but the scriptTypeConverter need to be moved to the qt app plugin.
	qtTypeConverters_.emplace_back(new GenericQtTypeConverter<ObjectHandle>());
	qtTypeConverters_.emplace_back( new ImageQtTypeConverter() );
	qtTypeConverters_.emplace_back( new ModelQtTypeConverter( contextManager ) );
	qtTypeConverters_.emplace_back( new QObjectQtTypeConverter() );
	qtTypeConverters_.emplace_back( new CollectionQtTypeConverter( contextManager ) );
	qtTypeConverters_.emplace_back( new ScriptQtTypeConverter( self_ ) );

	QMetaType::registerComparators<ObjectHandle>();

	for (auto& qtTypeConverter : qtTypeConverters_)
	{
		qtFramework.registerTypeConverter( *qtTypeConverter );
	}

	defManager_->registerPropertyAccessorListener( propListener_ );
}


void QtScriptingEngine::Implementation::PropertyListener::postSetValue(
	const PropertyAccessor& accessor, const Variant& value )
{
	const ObjectHandle& object = accessor.getObject();
	auto itr = scriptObjects_.find( object );

	if (itr == scriptObjects_.end())
	{
		return;
	}

	// Copy collection to accommodate re-entry
    auto scriptObjects = itr->second;
    for( auto & scriptObject : scriptObjects.second )
    {
        assert( !scriptObject.second.isNull() );
        if(!scriptObject.second.isNull())
        {
	        scriptObject.second->firePropertySignal( accessor.getProperty(), value );
        }
    }
}


void QtScriptingEngine::Implementation::PropertyListener::postInvoke(
	const PropertyAccessor & accessor, Variant result, bool undo )
{
	const ObjectHandle& object = accessor.getObject();
	auto itr = scriptObjects_.find( object );

	if (itr == scriptObjects_.end())
	{
		return;
	}
    auto& scriptObjects = itr->second;
    for( auto & scriptObject : scriptObjects.second )
    {
        assert( !scriptObject.second.isNull() );
        if(!scriptObject.second.isNull())
        {
	        scriptObject.second->fireMethodSignal( accessor.getProperty(), undo );
        }
    }
}

QtScriptObject* QtScriptingEngine::Implementation::createScriptObject(const Variant& variant, QObject* parent)
{
	ObjectHandle object;
	if (variant.typeIs<ObjectHandle>())
	{
		object = variant.cast<ObjectHandle>();
	}
	else
	{
		auto typeId = variant.type()->typeId();
		auto definition = defManager_->getDefinition(typeId.getName());
		if (definition != nullptr)
		{
			ObjectHandle obj(variant, definition);
			object = obj;
		}
	}
	if (!object.isValid())
	{
		return nullptr;
	}
	auto root = reflectedRoot( object, *defManager_ );

    auto parentItr = scriptObjects_.find( root );
	QtScriptObject * scriptObject = nullptr;
    if (parentItr == scriptObjects_.end())
    {
		auto classDefinition = root.getDefinition(*defManager_);
		if (classDefinition == nullptr)
		{
			return nullptr;
		}

		auto metaObject = getMetaObject(*classDefinition);
		if (metaObject == nullptr)
		{
			return nullptr;
		}

		assert(contextManager_); 
		auto data =
			std::make_shared< QtScriptObjectData >(*contextManager_, self_, *metaObject, root );
		scriptObject = new QtScriptObject( data, parent );
		ParentMap parentMap;
		parentMap.insert( 
			std::make_pair( parent, scriptObject )
		 );
		scriptObjects_.insert(
			std::make_pair(
				root, std::make_pair( data, parentMap )
			));
		return scriptObject;
    }

	auto & parentMap = parentItr->second.second;
	auto findIt = parentMap.find( parent );
	if (findIt == parentMap.end())
	{
		auto dataPtr = parentItr->second.first.lock();
		assert(dataPtr != nullptr);
		scriptObject = new QtScriptObject( dataPtr, parent );
		parentMap.insert(std::make_pair(parent, scriptObject));
		return scriptObject;
	}
	return findIt->second;
}


QMetaObject* QtScriptingEngine::Implementation::getMetaObject( const IClassDefinition& classDefinition )
{
	auto definition = classDefinition.getName();

	{
		std::lock_guard< std::mutex > guard( metaObjectsMutex_ );
		auto metaObjectIt = metaObjects_.find( definition );
		if ( metaObjectIt != metaObjects_.end() )
		{
			return metaObjectIt->second;
		}
	}

	QMetaObjectBuilder builder;
	builder.setClassName( definition );
	builder.setSuperClass( &QObject::staticMetaObject );

	auto thisProperty = builder.addProperty( "self", "QObject*" );
	thisProperty.setWritable( false );
	thisProperty.setConstant( true );

	// Add all the properties from the ClassDefinition to the builder
	auto properties = classDefinition.allProperties();
	auto it = properties.begin();

	for (; it != properties.end(); ++it)
	{
		if (it->isMethod())
		{
			continue;
		}

		auto property = builder.addProperty( it->getName(), "QVariant" );
		property.setWritable( !it->readOnly() );

		auto notifySignal = std::string( it->getName() ) + "Changed(QVariant)";
		property.setNotifySignal( builder.addSignal( notifySignal.c_str() ) );
	}

	std::vector<std::pair<std::string, std::string>> methodSignatures;
	std::string methodSignature;

	// TODO: Move these three to actual methods on the scripting engine.
	methodSignatures.emplace_back( "getMetaObject(QString)", "QVariant" );
	methodSignatures.emplace_back( "getMetaObject(QString,QString)", "QVariant" );
	methodSignatures.emplace_back( "containsMetaType(QString,QString)", "QVariant" );

	for (it = properties.begin(); it != properties.end(); ++it)
	{
		if (!it->isMethod())
		{
			continue;
		}

		methodSignature = it->getName();
		methodSignature += "(";

		for (size_t i = 0; i < it->parameterCount(); ++i)
		{
			methodSignature += "QVariant";

			if (i < it->parameterCount() - 1)
			{
				methodSignature += ",";
			}
		}

		methodSignature += ")";

		// TODO - determine if the function does not have a return type.
		// currently 'invoke' will always return a Variant regardless
		methodSignatures.emplace_back( std::move( methodSignature ), "QVariant" );
	}

	// skip index 0 as it has the same name as the one at index 1.
	for (size_t i = 1; i < methodSignatures.size(); ++i)
	{
		methodSignature =
			methodSignatures[i].first.substr( 0, methodSignatures[i].first.find( '(' ) ) +
			"Invoked(QVariant)";
		QMetaMethodBuilder method = builder.addSignal( methodSignature.c_str() );
		QList<QByteArray> parameterNames;
		parameterNames.append( "undo" );
		method.setParameterNames( parameterNames );
	}

	for (size_t i = 0; i < methodSignatures.size(); ++i)
	{
		builder.addMethod( methodSignatures[i].first.c_str(), methodSignatures[i].second.c_str() );
	}

	auto metaObject = builder.toMetaObject();
	if (metaObject == nullptr)
	{
		return nullptr;
	}

	{
		std::lock_guard< std::mutex > guard( metaObjectsMutex_ );
		auto inserted = metaObjects_.insert( 
			std::pair< std::string, QMetaObject * >( definition, metaObject ) );
		if (!inserted.second)
		{
			free( metaObject );
		}
		return inserted.first->second;
	}
}

QtScriptingEngine::QtScriptingEngine()
	: impl_( new Implementation( *this ) )
{
}

QtScriptingEngine::~QtScriptingEngine()
{
	impl_ = nullptr;
}

void QtScriptingEngine::initialise( IQtFramework & qtFramework, IComponentContext & contextManager )
{	
	impl_->initialise( qtFramework, contextManager );
}

void QtScriptingEngine::finalise()
{
	// normally, this assert should never fire
	// if it does, we need to figure out why
	// QScriptObject not get destroyed correctly.
    assert( impl_->scriptObjects_.empty() );
	impl_->scriptObjects_.clear();
}

void QtScriptingEngine::deregisterScriptObject( QtScriptObject & scriptObject )
{
	auto findIt = impl_->scriptObjects_.find( scriptObject.object() );
	assert (findIt != impl_->scriptObjects_.end());
    auto & parentMap = findIt->second.second;
	auto parent = scriptObject.parent();
	auto parentIt = parentMap.find( parent );
	assert( parentIt != parentMap.end() );
	parentMap.erase( parentIt );
    if( parentMap.empty() )
    {
	    impl_->scriptObjects_.erase( findIt->first );
    }
}

QtScriptObject* QtScriptingEngine::createScriptObject(
const Variant& object, QObject* parent)
{
	return impl_->createScriptObject( object, parent );
}

QObject * QtScriptingEngine::createObject( QString definition )
{
	auto className = std::string( "class " ) + definition.toUtf8().constData();

	if (impl_->defManager_ == nullptr)
	{
		qCritical( "Definition manager not found. Could not create object: %s \n", 
			className.c_str() );
		return nullptr;
	}

	auto classDefinition = impl_->defManager_->getDefinition( className.c_str() );
	if (classDefinition == nullptr)
	{
		qWarning( "No definition registered for type: %s \n", className.c_str() );
		return nullptr;
	}

	auto object = classDefinition->createManagedObject();
	if (object == nullptr)
	{
		qWarning( "Could not create C++ type: %s \n", className.c_str() );
		return nullptr;
	}

	// no parent as qml takes ownership of this object
	auto scriptObject = createScriptObject( object, nullptr );
	if (scriptObject == nullptr)
	{
		qWarning( "Could not create Qt type: %s \n", className.c_str() );
		return nullptr;
	}

	return scriptObject;
}


bool QtScriptingEngine::queueCommand( QString command )
{
	auto commandId = std::string( "class " ) + command.toUtf8().constData();
	Command * cmd = impl_->commandSystemProvider_->findCommand( commandId.c_str() );
	if(cmd == nullptr)
	{
		qWarning( "Could not find Command: %s \n", commandId.c_str() );
		return false;
	}
	impl_->commandSystemProvider_->queueCommand( commandId.c_str() );
	return true;
}

//NOTE(aidan): Qt doesn't send mouse release events correctly when a drag completes. To workaround,
//			   a fake event is posted when the drag is completed
//TODO(aidan): This bug might be specific to Qt's win32 message handling implementation. Test on Mac, Maya
void QtScriptingEngine::makeFakeMouseRelease()
{
	QMouseEvent* releaseEvent = 
		new QMouseEvent( QEvent::MouseButtonRelease, QPointF( 0, 0 ), Qt::MouseButton::LeftButton, 0, 0 );

	//NOTE(aidan): postEvent takes ownership of the release event
	QApplication::postEvent( (QObject*)QApplication::focusWidget(), releaseEvent );
}

void QtScriptingEngine::beginUndoFrame()
{
	impl_->commandSystemProvider_->beginBatchCommand();
}

void QtScriptingEngine::endUndoFrame()
{
	impl_->commandSystemProvider_->endBatchCommand();
}

void QtScriptingEngine::abortUndoFrame()
{
	impl_->commandSystemProvider_->abortBatchCommand();
}

void QtScriptingEngine::deleteMacro( QString command )
{
	std::string commandId = command.toUtf8().constData();
	Command * cmd = impl_->commandSystemProvider_->findCommand( commandId.c_str() );
	if(cmd == nullptr)
	{
		qWarning( "Delete macro failed: Could not find Macro: %s \n", commandId.c_str() );
		return;
	}
	impl_->commandSystemProvider_->deleteMacroByName( commandId.c_str() );
}

void QtScriptingEngine::selectControl( wgt::WGCopyController* control, bool append )
{
	impl_->copyPasteManager_->onSelect( control, append );
}

void QtScriptingEngine::deselectControl( wgt::WGCopyController* control, bool reset )
{
	impl_->copyPasteManager_->onDeselect( control, reset );
}

QObject * QtScriptingEngine::iterator( const QVariant & collection )
{
	int typeId = collection.type();
	if (typeId == QVariant::UserType)
	{
		typeId = collection.userType();
	}

	if (typeId == qMetaTypeId<Variant>())
	{
		auto variant = collection.value<Variant>();
		if (!variant.typeIs<IListModel>())
		{
			return nullptr;
		}

		auto listModel = const_cast<IListModel*>(variant.cast<const IListModel*>());
		if (listModel == nullptr)
		{
			return nullptr;
		}

		// QML will take ownership of this object
		return new WGListIterator(*listModel);
	}

	if (typeId == qMetaTypeId<ObjectHandle>())
	{
		auto handle = collection.value<ObjectHandle>();
		if (!handle.isValid())
		{
			return nullptr;
		}

		auto listModel = handle.getBase<IListModel>();
		if (listModel == nullptr)
		{
			return nullptr;
		}

		// QML will take ownership of this object
		return new WGListIterator(*listModel);
	}

	return nullptr;
}

QVariant QtScriptingEngine::getProperty( const QVariant & object, QString propertyPath )
{
	ObjectHandle handle;
	if(QtHelpers::toVariant(object).tryCast(handle))
	{
		auto definition = handle.getDefinition(*getDefinitionManager());
		if(definition != nullptr)
		{
			auto accessor = definition->bindProperty(propertyPath.toLocal8Bit().data(), handle);
			if(accessor.isValid())
			{
				return QtHelpers::toQVariant(accessor.getValue(), nullptr);
			}
		}
	}
	return QVariant();
}

bool QtScriptingEngine::setValueHelper( QObject * object, QString property, QVariant value )
{
	if (object == nullptr)
	{
		return false;
	}

	return object->setProperty( property.toUtf8(), value );
}

void QtScriptingEngine::closeWindow( const QString & windowId )
{
	std::string id = windowId.toUtf8().constData();
	auto windows = impl_->uiApplication_->windows();
	auto findIt = windows.find( id );
	if (findIt == windows.end())
	{
		qWarning( "Failed to close window: Could not find window: %s \n", id.c_str() );
		return;
	}
	findIt->second->close();
}

IDefinitionManager* QtScriptingEngine::getDefinitionManager()
{
	return impl_->defManager_;
}

void QtScriptingEngine::addPreference( const QString & preferenceId, const QString & propertyName, QVariant value )
{
	std::string id = preferenceId.toUtf8().constData();
	std::string name = propertyName.toUtf8().constData();
	std::string data = value.toString().toUtf8().constData();
	auto preference = impl_->uiFramework_->getPreferences()->getPreference( id.c_str() );
	preference->set( name.c_str(), data );
}


//------------------------------------------------------------------------------
void QtScriptingEngine::swapParent(QtScriptObject & scriptObject, QObject * newParent)
{
	const auto & object = scriptObject.object();
	auto findIt = impl_->scriptObjects_.find( object );
	assert(findIt != impl_->scriptObjects_.end());
	auto & parentMap = findIt->second.second;
	auto currentParent = scriptObject.parent();
	auto parentIt = parentMap.find( currentParent );
	assert(parentIt != parentMap.end());
	parentMap.erase(parentIt);
	parentMap.insert( std::make_pair(newParent, &scriptObject ) );
}
} // end namespace wgt
